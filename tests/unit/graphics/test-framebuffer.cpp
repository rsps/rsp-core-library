/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <chrono>
#include <doctest.h>
#include <graphics/Framebuffer.h>
#include <graphics/primitives/Bitmap.h>
#include <thread>
#include <filesystem>
#include <posix/FileSystem.h>
#include <utils/StopWatch.h>
#include <TestHelpers.h>

using namespace rsp::graphics;

inline void CheckPixel(const Point &aPoint, const Color &aColour, const Framebuffer &fb)
{
    if (fb.IsInsideScreen(aPoint)) {
        CHECK(fb.GetPixel(aPoint) == aColour);
    } else {
        CHECK(fb.GetPixel(aPoint) == 0);
    }
}

static unsigned int urand()
{
    return static_cast<unsigned int>(rand());
}

TEST_CASE("Framebuffer")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});

    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    srand(ms.count()); // generates random seed val
    Color col(urand() % 200 + 56, urand() % 200 + 56, urand() % 200 + 56, 0xff);
    MESSAGE("Color: " << col);

    SUBCASE("Clear")
    {
        fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
        fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
    }

    SUBCASE("Drawing Lines")
    {
        // Arrange
        Point pointA(urand() % fb.GetWidth(), urand() % fb.GetHeight());
        Point pointB(urand() % fb.GetWidth(), urand() % fb.GetHeight());

        // Act
        CHECK_NOTHROW(fb.DrawLine(pointA, pointB, col));

        // Assert
        int deltaX = pointB.GetX() - pointA.GetX();
        int deltaY = pointB.GetY() - pointA.GetY();
        int absDeltaX = abs(deltaX);
        int absDeltaY = abs(deltaY);
        int signumX = (deltaX > 0) ? 1 : -1;
        int signumY = (deltaY > 0) ? 1 : -1;
        int x = absDeltaX >> 1;
        int y = absDeltaY >> 1;
        int px = pointA.GetX();
        int py = pointA.GetY();
        if (absDeltaX >= absDeltaY) {
            for (int i = 0; i < absDeltaX; i++) {
                y += absDeltaY;
                if (y >= absDeltaX) {
                    y -= absDeltaX;
                    py += signumY;
                }
                px += signumX;
                CHECK(fb.GetPixel(Point(px, py), false) == col);
            }
        } else {
            for (int i = 0; i < absDeltaY; i++) {
                x += absDeltaX;
                if (x >= absDeltaY) {
                    x -= absDeltaY;
                    px += signumX;
                }
                py += signumY;
                CHECK(fb.GetPixel(Point(px, py), false) == col);
            }
        }

        SUBCASE("Lines are Inclusive")
        {
            CHECK_EQ(fb.GetPixel(pointA, false), col);
            CHECK_EQ(fb.GetPixel(pointB, false), col);
        }
        fb.SwapBuffer();
    }

    SUBCASE("Drawing Rectangles")
    {
        // Arrange
        // Generate random values in the LEFT and TOP halves of the screen
        Point leftTop(urand() % (fb.GetWidth() / 2),
                      urand() % (fb.GetHeight() / 2));
        // Generate random values in the RIGHT and BOTTOM halves of the screen
        Point rightBottom(urand() % (fb.GetWidth() + 1 - (fb.GetWidth() / 2)) + (fb.GetWidth() / 2),
                          urand() % (fb.GetHeight() + 1 - (fb.GetHeight() / 2)) + (fb.GetHeight() / 2));
        Rect rect(leftTop, rightBottom);

        // Act
        CHECK_NOTHROW(fb.DrawRectangle(rect, col));

        // Assert
        // Expect all four side to hold values
        for (int i = 0; i <= rect.GetWidth(); i++) {
            // Check top side
            CHECK(col == fb.GetPixel(Point(leftTop.GetX() + i, leftTop.GetY()), false));
            // Check bottom side
            CHECK(col == fb.GetPixel(Point(leftTop.GetX() + i, rightBottom.GetY()), false));
        }
        for (int i = 0; i <= rect.GetHeight(); i++) {
            // Check left side
            CHECK(col == fb.GetPixel(Point(leftTop.GetX(), rightBottom.GetY() - i), false));
            // Check right side
            CHECK(col == fb.GetPixel(Point(rightBottom.GetX(), rightBottom.GetY() - i), false));
        }
        fb.SwapBuffer();
    }

    SUBCASE("Drawing Circles")
    {
        // Arrange
        Point centerPoint(urand() % fb.GetWidth(), urand() % fb.GetHeight());
        int radius = static_cast<int>(urand() % (fb.GetWidth() / 2));

        // Act
        CHECK_NOTHROW(fb.DrawCircle(centerPoint, radius, col));

        // Assert
        int error = -radius;
        int y = 0;
        while (radius >= y) {
            CheckPixel(Point(centerPoint.GetX() + radius, centerPoint.GetY() + y), col, fb);
            CheckPixel(Point(centerPoint.GetX() - radius, centerPoint.GetY() + y), col, fb);
            CheckPixel(Point(centerPoint.GetX() + radius, centerPoint.GetY() - y), col, fb);
            CheckPixel(Point(centerPoint.GetX() - radius, centerPoint.GetY() - y), col, fb);
            CheckPixel(Point(centerPoint.GetX() + y, centerPoint.GetY() + radius), col, fb);
            CheckPixel(Point(centerPoint.GetX() - y, centerPoint.GetY() + radius), col, fb);
            CheckPixel(Point(centerPoint.GetX() + y, centerPoint.GetY() - radius), col, fb);
            CheckPixel(Point(centerPoint.GetX() - y, centerPoint.GetY() - radius), col, fb);
            error += y;
            y++;
            error += y;

            if (error >= 0) {
                error += -radius;
                radius--;
                error += -radius;
            }
        }
        fb.SwapBuffer();
    }

    SUBCASE("Set/Get pixel outside screen")
    {
        // Arrange
        Point outSideXAxis(-1, 0);
        Point outSideYAxis(0, -1);

        // Act
        CHECK_NOTHROW(fb.SetPixel(outSideXAxis, col));
        CHECK_NOTHROW(fb.SetPixel(outSideYAxis, col));

        // Assert
        CHECK_EQ(fb.GetPixel(outSideXAxis), 0);
        CHECK_EQ(fb.GetPixel(outSideYAxis), 0);
    }

    SUBCASE("Drawing Images")
    {
        // Arrange
        Point topLeftImgCorner(100, 200);
        std::string testImage = "testImages/testImageCross.bmp";

        // Act
        Bitmap testImgMap(testImage);
        unsigned int height = testImgMap.GetHeight();
        unsigned int width = testImgMap.GetWidth();
        Point topLeft(0, 0);
        Point topRight(width - 1, 0u);
        Point botLeft(0u, height - 1);
        Point botRight(width - 1, height - 1);

        // Assert
        CHECK(testImgMap.IsInsideScreen(topLeft));
        CHECK(testImgMap.IsInsideScreen(topRight));
        CHECK(testImgMap.IsInsideScreen(botLeft));
        CHECK(testImgMap.IsInsideScreen(botRight));

        SUBCASE("Draw image from file")
        {
            // Act
            CHECK_NOTHROW(fb.DrawImage(topLeftImgCorner, testImgMap));

            // Assert
            CHECK(testImgMap.GetHeight() == height);
            CHECK(testImgMap.GetWidth() == width);
            CHECK(testImgMap.GetPixelData().GetDataSize() == (width * height * 3));

            fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
        }
        SUBCASE("Draw edited image file")
        {
            // Arrange
            Color red(Color::Red);
            Color green(Color::Green);
            Color blue(Color::Blue);
            Color thisColor(0xFF777777);

            // Act
            CHECK_NOTHROW(testImgMap.DrawLine(topLeft, topRight, red));
            CHECK_NOTHROW(testImgMap.DrawLine(topLeft, botLeft, green));
            CHECK_NOTHROW(testImgMap.DrawLine(topRight, botRight, blue));
            CHECK_NOTHROW(testImgMap.DrawLine(botLeft, botRight, thisColor));

            CHECK_NOTHROW(fb.DrawImage(topLeftImgCorner, testImgMap));
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));

            // Assert
            CHECK(testImgMap.GetPixel(Point(width / 2, 0u)) == red);
            CHECK(testImgMap.GetPixel(Point(0u, height / 2)) == green);
            CHECK(testImgMap.GetPixel(Point(width - 1, height / 2)) == blue);
            CHECK(testImgMap.GetPixel(Point(width / 2, height - 1)) == thisColor);
        }
        SUBCASE("Draw memory created image")
        {
            // Arrange
            Bitmap emptyMap(height, width, 4);
            Point randomPoint(urand() % emptyMap.GetWidth(), urand() % emptyMap.GetHeight());

            // Act
            CHECK_NOTHROW(emptyMap.SetPixel(randomPoint, col));
            CHECK_NOTHROW(fb.DrawImage(topLeftImgCorner, emptyMap));
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));

            // Assert
            CHECK(emptyMap.GetPixel(randomPoint) == col);
        }
    }

    SUBCASE("Drawing image larger than screen")
    {
        // Arrange
        Point topLeft(0, 0);
        Point randomPoint(urand() % fb.GetWidth(), urand() % fb.GetHeight());
        std::string largeImg = "testImages/largeTestImg.bmp";
        Bitmap largeImgMap(largeImg);
        // Make sure screen is empty
        fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);

        // Act
        CHECK_NOTHROW(fb.DrawImage(topLeft, largeImgMap));

        // Assert
        CHECK(largeImgMap.GetHeight() > fb.GetHeight());
        CHECK(largeImgMap.GetWidth() > fb.GetWidth());
        CHECK(fb.GetPixel(randomPoint) != 0);

        SUBCASE("Spill large imgage into screen")
        {
            // Arrange
            topLeft = {-100, -100};

            // Act
            CHECK_NOTHROW(fb.DrawImage(topLeft, largeImgMap));

            // Assert
            CHECK(fb.GetPixel(randomPoint) != 0);
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        }
    }

    SUBCASE("Moving image")
    {
        // Arrange
        Point topLeftPoint(100, 200);
        Bitmap imgSimple("testImages/testImage.bmp");
        int iterations = 100;

        // Act
        rsp::utils::StopWatch sw;
        for (int i = 0; i < iterations; i++) {
            CHECK_NOTHROW(fb.DrawImage(Point(topLeftPoint.GetX(), topLeftPoint.GetY() - i), imgSimple));
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        }
        int fps = (1000 * iterations) / (sw.Elapsed<std::chrono::milliseconds>() + 1);

        // Assert
        CHECK(fps > 10);
        MESSAGE("Fps: " << fps);
    }

    SUBCASE("Moving monochrome image")
    {
        // Arrange
        Point topLeftPoint(100, 200);
        Bitmap imgSimple("testImages/Monochrome.bmp");
        int iterations = 100;

        // Act
        rsp::utils::StopWatch sw;
        for (int i = 0; i < iterations; i++) {
            CHECK_NOTHROW(fb.DrawImage(Point(topLeftPoint.GetX(), topLeftPoint.GetY() - i), imgSimple));
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        }
        int fps = (1000 * iterations) / (sw.Elapsed<std::chrono::milliseconds>() + 1);

        // Assert
        CHECK(fps > 10);
        MESSAGE("Fps: " << fps);
    }

    SUBCASE("Draw text")
    {
        const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
        Font::RegisterFont(cFontFile);
        Rect r(100, 200, 280, 200);

        CHECK_NOTHROW(fb.DrawRectangle(r, Color::White));
        Text text("Exo 2", "Hello World");
        CHECK_NOTHROW(text.GetFont().SetSize(30));
        CHECK_NOTHROW(text.SetArea(r).GetFont().SetColor(Color::Red));
        CHECK_NOTHROW(fb.DrawText(text.Reload()));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        CHECK_NOTHROW(fb.DrawRectangle(r, Color::Grey));
        CHECK_NOTHROW(text.SetScaleToFit().SetValue("Hello\nWorld"));
        CHECK_NOTHROW(fb.DrawText(text.Reload(), Color::Green));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        CHECK_NOTHROW(fb.DrawRectangle(r, Color::Purple));
        CHECK_NOTHROW(text.SetValue("Hello\nWorld\nHELLO\nMOON"));
        CHECK_NOTHROW(fb.DrawText(text.Reload(), Color::Grey));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        CHECK_NOTHROW(fb.DrawRectangle(r, Color::White));
        CHECK_NOTHROW(text.SetValue("RED"));
        CHECK_NOTHROW(fb.DrawText(text.Reload(), Color::Red));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        CHECK_NOTHROW(fb.DrawRectangle(r, Color::White));
        CHECK_NOTHROW(text.SetValue("GREEN"));
        CHECK_NOTHROW(fb.DrawText(text.Reload(), Color::Green));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        CHECK_NOTHROW(fb.DrawRectangle(r, Color::White));
        CHECK_NOTHROW(text.SetValue("BLUE"));
        CHECK_NOTHROW(fb.DrawText(text.Reload(), Color::Blue));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        const Color rainbow[] = { Color::White, Color::Red, Color::Yellow, Color::Green, Color::Aqua, Color::Lime, Color::Blue, Color::Silver };

        CHECK_NOTHROW(text.SetScaleToFit(true).SetLineSpacing(50));
//        Rect screen(0, 0, 480, 800);
        rsp::utils::StopWatch sw;
        for (int i = 0 ; i < 1000 ; i++) {
//            fb.DrawRectangle(screen, rainbow[i & 0x07], true);
            int fps = (1000 * i) / (sw.Elapsed<std::chrono::milliseconds>() + 1);
            std::stringstream ss;
            ss << "FPS:\n" << fps;
            CHECK_NOTHROW(text.SetValue(ss.str()));
            CHECK_NOTHROW(fb.DrawText(text.Reload(), rainbow[(i + 3) & 0x07]));
            CHECK_NOTHROW(text.SetScaleToFit(false)); // Only scale first time to speed it up
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear, rainbow[i & 0x07]));
        }
        MESSAGE(text.GetValue());

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
    }

    SUBCASE("Text Alignment")
    {
        const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
        Font::RegisterFont(cFontFile);
//        Rect r(10, 10, 60, 80);
        Rect r(10, 10, 460, 780);

        CHECK_NOTHROW(fb.DrawRectangle(r, Color::White));
        Text text("Exo 2", "Hello\nWorld");
        CHECK_NOTHROW(text.SetArea(r).GetFont().SetSize(50).SetColor(Color::Yellow));
        CHECK_NOTHROW(fb.DrawText(text.Reload()));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        const Text::VAlign cVertical[] = { Text::VAlign::Top, Text::VAlign::Center, Text::VAlign::Bottom };
        const Text::HAlign cHorizontal[] = { Text::HAlign::Left, Text::HAlign::Center, Text::HAlign::Right };

        for (int h = 0 ; h < 3 ; h++) {
            for (int v = 0 ; v < 3 ; v++) {
                CHECK_NOTHROW(fb.DrawRectangle(r, Color::White));
                CHECK_NOTHROW(fb.DrawText(text.SetVAlignment(cVertical[v]).SetHAlignment(cHorizontal[h]).Reload()));
                CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    SUBCASE("Font Styles")
    {
        CHECK_NOTHROW(Font::RegisterFont("fonts/Exo2-Italic-VariableFont_wght.ttf"));
        CHECK_NOTHROW(Font::RegisterFont("fonts/Exo2-VariableFont_wght.ttf"));
        Rect r(10, 10, 460, 280);

        CHECK_NOTHROW(fb.DrawRectangle(r, Color::White));
        Text text("Exo 2", "Regular");
        CHECK_NOTHROW(text.SetArea(r).SetScaleToFit(true).GetFont().SetSize(50).SetColor(Color::Yellow));
        CHECK_NOTHROW(fb.DrawText(text.Reload()));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        CHECK_NOTHROW(text.SetValue("Bold").GetFont().SetStyle(Font::Styles::Bold));
        CHECK_NOTHROW(fb.DrawText(text.Reload()));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        CHECK_NOTHROW(text.SetValue("Italic").GetFont().SetStyle(Font::Styles::Italic));
        CHECK_NOTHROW(fb.DrawText(text.Reload()));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        CHECK_NOTHROW(text.SetValue("Bold Italic").GetFont().SetStyle(Font::Styles::BoldItalic));
        CHECK_NOTHROW(fb.DrawText(text.Reload()));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    SUBCASE("Swapbuffer")
    {
        fb.SwapBuffer();
    }

    SUBCASE("Draw Fullscreen") {
        std::string testImage = "testImages/Asset2WithAlpha.bmp";
        Bitmap testImgMap(testImage);
        CHECK_NOTHROW(fb.DrawImage(Point(0,0), testImgMap));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
    }

    SUBCASE("Draw Transparent") {
        for (std::uint32_t a = 0; a < 200 ; a += 5) {
            Color blue(Color::Blue);
            Color green(Color::Green);
            Color red(Color::Red);
            blue.SetAlpha(static_cast<std::uint8_t>(a));
            green.SetAlpha(static_cast<std::uint8_t>(a));
            red.SetAlpha(static_cast<std::uint8_t>(a));
            CHECK_NOTHROW(fb.DrawRectangle(Rect(135,170, 100, 100), red, true));
            CHECK_NOTHROW(fb.DrawRectangle(Rect(100,100, 100, 100), blue, true));
            CHECK_NOTHROW(fb.DrawRectangle(Rect(170,100, 100, 100), green, true));
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

}
