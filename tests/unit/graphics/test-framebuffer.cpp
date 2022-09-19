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
#include <utils/Random.h>

using namespace rsp::graphics;
using namespace rsp::utils;

inline void CheckPixel(const Point &aPoint, const Color &aColour, const Framebuffer &fb)
{
    if (fb.IsInsideCanvas(aPoint)) {
        CHECK_EQ(fb.GetPixel(aPoint), aColour);
    } else {
        CHECK_EQ(fb.GetPixel(aPoint), 0);
    }
}

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Framebuffer")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    Random::Seed(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});

    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    srand(ms.count()); // generates random seed val
    Color col( Random::Roll(56u, 200u), Random::Roll(56u, 200u), Random::Roll(56u, 200u), 0xff);
    MESSAGE("Color: " << col.AsUint());

    SUBCASE("Clear")
    {
        fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
        fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
    }

    SUBCASE("Drawing Lines")
    {
        // Arrange
        Point pointA(Random::Roll(0, fb.GetWidth()), Random::Roll(0, fb.GetHeight()));
        Point pointB(Random::Roll(0, fb.GetWidth()), Random::Roll(0, fb.GetHeight()));

        // Act
        CHECK_NOTHROW(fb.DrawLine(pointA, pointB, col));

        // Assert
        int deltaX = static_cast<int>(pointB.GetX() - pointA.GetX());
        int deltaY = static_cast<int>(pointB.GetY() - pointA.GetY());
        int absDeltaX = abs(deltaX);
        int absDeltaY = abs(deltaY);
        int signumX = (deltaX > 0) ? 1 : -1;
        int signumY = (deltaY > 0) ? 1 : -1;
        int x = absDeltaX >> 1;
        int y = absDeltaY >> 1;
        int px = static_cast<int>(pointA.GetX());
        int py = static_cast<int>(pointA.GetY());
        if (absDeltaX >= absDeltaY) {
            for (int i = 0; i < absDeltaX; i++) {
                y += absDeltaY;
                if (y >= absDeltaX) {
                    y -= absDeltaX;
                    py += signumY;
                }
                px += signumX;
                CHECK_EQ(fb.GetPixel(Point(px, py), false), col.AsUint());
            }
        } else {
            for (int i = 0; i < absDeltaY; i++) {
                x += absDeltaX;
                if (x >= absDeltaY) {
                    x -= absDeltaY;
                    px += signumX;
                }
                py += signumY;
                CHECK_EQ(fb.GetPixel(Point(px, py), false), col.AsUint());
            }
        }

        SUBCASE("Lines are Inclusive")
        {
            CHECK_EQ(fb.GetPixel(pointA, false), col.AsUint());
            CHECK_EQ(fb.GetPixel(pointB, false), col.AsUint());
        }
        fb.SwapBuffer();
    }

    SUBCASE("Drawing Rectangles")
    {
        // Arrange
        // Generate random values in the LEFT and TOP halves of the screen
        Point leftTop(Random::Roll(0, fb.GetWidth() / 2), Random::Roll(0, fb.GetHeight() / 2));
        // Generate random values in the RIGHT and BOTTOM halves of the screen
        Point rightBottom(
            Random::Roll(0, fb.GetWidth() / 2) + (fb.GetWidth() / 2),
            Random::Roll(0, fb.GetHeight() / 2) + (fb.GetHeight() / 2));
        Rect rect(leftTop, rightBottom.GetX() - leftTop.GetX(), rightBottom.GetY() - leftTop.GetY());

        // Act
        CHECK_NOTHROW(fb.DrawRectangle(rect, col));

        // Assert
        // Expect all four side to hold values
        for (GuiUnit_t i = 0; i < rect.GetWidth(); i++) {
            // Check top side
            CHECK_EQ(col.AsUint(), fb.GetPixel(Point(leftTop.GetX() + i, leftTop.GetY()), false));
            // Check bottom side
            CHECK_EQ(col.AsUint(), fb.GetPixel(Point(leftTop.GetX() + i, rightBottom.GetY()-1), false));
        }
        for (GuiUnit_t i = 0; i < rect.GetHeight(); i++) {
            // Check left side
            CHECK_EQ(col.AsUint(), fb.GetPixel(Point(leftTop.GetX(), rightBottom.GetY()-1 - i), false));
            // Check right side
            CHECK_EQ(col.AsUint(), fb.GetPixel(Point(rightBottom.GetX()-1, rightBottom.GetY()-1 - i), false));
        }
        fb.SwapBuffer();

        Rect small(10, 10, 1, 1);
        Color white(Color::White);
        CHECK_NOTHROW(fb.DrawRectangle(small, white));
        CHECK_EQ(fb.GetPixel(Point(10, 10)), white);
        CHECK_NE(fb.GetPixel(Point(11, 10)), white);
        CHECK_NE(fb.GetPixel(Point(10, 11)), white);
        CHECK_NE(fb.GetPixel(Point(11, 11)), white);
        fb.SwapBuffer();

        Rect medium(20, 20, 10, 10);
        CHECK_NOTHROW(fb.DrawRectangle(medium, white));
        CHECK_EQ(fb.GetPixel(Point(20, 20)), white);
        CHECK_EQ(fb.GetPixel(Point(20, 29)), white);
        CHECK_NE(fb.GetPixel(Point(20, 30)), white);
        CHECK_EQ(fb.GetPixel(Point(29, 20)), white);
        CHECK_NE(fb.GetPixel(Point(30, 20)), white);
        CHECK_EQ(fb.GetPixel(Point(29, 29)), white);
        CHECK_NE(fb.GetPixel(Point(30, 30)), white);
        fb.SwapBuffer();
    }

    SUBCASE("Drawing Circles")
    {
        // Arrange
        Point centerPoint(Random::Roll(0, fb.GetWidth()), Random::Roll(0, fb.GetHeight()));
        GuiUnit_t radius = Random::Roll(0, fb.GetWidth() / 2);

        // Act
        CHECK_NOTHROW(fb.DrawCircle(centerPoint, radius, col));

        // Assert
        int error = -static_cast<int>(radius);
        GuiUnit_t y = 0;
        while (radius >= y) {
            CheckPixel(Point(centerPoint.GetX() + radius, centerPoint.GetY() + y), col, fb);
            CheckPixel(Point(centerPoint.GetX() - radius, centerPoint.GetY() + y), col, fb);
            CheckPixel(Point(centerPoint.GetX() + radius, centerPoint.GetY() - y), col, fb);
            CheckPixel(Point(centerPoint.GetX() - radius, centerPoint.GetY() - y), col, fb);
            CheckPixel(Point(centerPoint.GetX() + y, centerPoint.GetY() + radius), col, fb);
            CheckPixel(Point(centerPoint.GetX() - y, centerPoint.GetY() + radius), col, fb);
            CheckPixel(Point(centerPoint.GetX() + y, centerPoint.GetY() - radius), col, fb);
            CheckPixel(Point(centerPoint.GetX() - y, centerPoint.GetY() - radius), col, fb);
            error += static_cast<int>(y);
            y++;
            error += static_cast<int>(y);

            if (error >= 0) {
                error += -static_cast<int>(radius);
                radius--;
                error += -static_cast<int>(radius);
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
        GuiUnit_t height = testImgMap.GetHeight();
        GuiUnit_t width = testImgMap.GetWidth();
        Point topLeft(0, 0);
        Point topRight(width - 1, 0);
        Point botLeft(0, height - 1);
        Point botRight(width - 1, height - 1);

        // Assert
        CHECK(testImgMap.IsInsideCanvas(topLeft));
        CHECK(testImgMap.IsInsideCanvas(topRight));
        CHECK(testImgMap.IsInsideCanvas(botLeft));
        CHECK(testImgMap.IsInsideCanvas(botRight));
        CHECK_EQ(testImgMap.GetHeight(), height);
        CHECK_EQ(testImgMap.GetWidth(), width);
        CHECK_EQ(testImgMap.GetPixelData().GetDataSize(), (width * height * 3));

        SUBCASE("Draw image from file")
        {
            // Act
            CHECK_NOTHROW(fb.DrawImage(topLeftImgCorner, testImgMap));

            // Assert
            CHECK_EQ(fb.GetPixel(topLeftImgCorner + Point(4, 4), false), Color(0xFF020F92));

            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        }
        SUBCASE("Draw edited image file")
        {
            // Arrange
            auto offset = [](Point &p, int val) { return (p + Point(val, val)); };

            // Act
            for (int i=0; i < 16 ;++i){
                uint8_t val = 0x77+(i*8);
                Color grey(val, val, val, 0xFF);
                CHECK_NOTHROW(testImgMap.DrawRectangle(Rect(offset(topLeft, i), width-(i*2), height-(i*2)), grey));
            }

            CHECK_NOTHROW(testImgMap.DrawLine(topLeft, botRight, Color::Red));
            CHECK_NOTHROW(testImgMap.DrawLine(botLeft, topRight, Color::Red));

            CHECK_NOTHROW(fb.DrawImage(topLeftImgCorner, testImgMap));
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));

            // Assert
            CHECK_EQ(testImgMap.GetPixel(Point(width / 2, 0)), 0xFF777777);
            CHECK_EQ(testImgMap.GetPixel(Point(0, height / 2)), 0xFF777777);
            CHECK_EQ(testImgMap.GetPixel(Point(width - 12, height / 2)), 0xFFCFCFCF);
            CHECK_EQ(testImgMap.GetPixel(Point(width / 2, height - 4)), 0xFF8F8F8F);

            CHECK_EQ(fb.GetPixel(topLeftImgCorner + Point(width / 2, 0), true), 0xFF777777);
            CHECK_EQ(fb.GetPixel(topLeftImgCorner + Point(0, height / 2), true), 0xFF777777);
            CHECK_EQ(fb.GetPixel(topLeftImgCorner + Point(width - 12, height / 2), true), 0xFFCFCFCF);
            CHECK_EQ(fb.GetPixel(topLeftImgCorner + Point(width / 2, height - 4), true), 0xFF8F8F8F);
        }
        SUBCASE("Draw memory created image")
        {
            // Arrange
            Bitmap emptyMap(height, width, 4);
            Point randomPoint(Random::Roll(0, width-1), Random::Roll(0, height-1));

//            MESSAGE("randomPoint: " << randomPoint);
//            MESSAGE("topLeftImgCorner: " << topLeftImgCorner);
//            MESSAGE("Combined: " << (topLeftImgCorner + randomPoint));

            // Act
            CHECK_NOTHROW(emptyMap.DrawRectangle(Rect(0, 0, width, height), col, true) ); //.SetPixel(randomPoint, col));
            CHECK_NOTHROW(fb.DrawImage(topLeftImgCorner, emptyMap));
            CHECK_NOTHROW(fb.SetPixel(topLeftImgCorner + randomPoint, Color::White));
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));

            // Assert
            CHECK_EQ(fb.GetPixel(topLeftImgCorner, true), col.AsUint());
            CHECK_EQ(fb.GetPixel(topLeftImgCorner + randomPoint, true), Color::White);
        }
    }

    SUBCASE("Drawing image larger than screen")
    {
        // Arrange
        Point topLeft(0, 0);
        Point randomPoint(Random::Roll(0, fb.GetWidth()), Random::Roll(0, fb.GetHeight()));
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
            CHECK_NE(fb.GetPixel(randomPoint), 0);
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
            CHECK_NOTHROW(fb.DrawImage(Point(topLeftPoint.GetX(), topLeftPoint.GetY() - static_cast<GuiUnit_t>(i)), imgSimple));
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
        Color mcl[5] = {
            Color::White,
            Color::Blue,
            Color::Red,
            Color::Green,
            Color::Yellow
        };

        // Act
        rsp::utils::StopWatch sw;
        for (int i = 0; i < iterations; i++) {
            CHECK_NOTHROW(fb.DrawImage(Point(topLeftPoint.GetX(), topLeftPoint.GetY() - static_cast<GuiUnit_t>(i)), imgSimple, mcl[(i / 20) % 5]));
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
//            std::this_thread::sleep_for(std::chrono::milliseconds(25));
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

        MESSAGE("FPS Test");
        CHECK_NOTHROW(text.SetScaleToFit(true).SetLineSpacing(50));
//        Rect screen(0, 0, 480, 800);
        rsp::utils::StopWatch sw;
        for (int i = 0 ; i < 1000 ; i++) {
//            fb.DrawRectangle(screen, rainbow[i & 0x07], true);
            int fps = (1000 * i) / (sw.Elapsed<std::chrono::milliseconds>() + 1);
            std::stringstream ss;
            ss << "FPS:\n" << fps;
            CHECK_NOTHROW(text.SetValue(ss.str()));
            CHECK_NOTHROW(fb.DrawText(text.Reload(), Color::Black));
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

        CHECK_NOTHROW(text.SetValue("Bold").GetFont().SetStyle(FontStyles::Bold));
        CHECK_NOTHROW(fb.DrawText(text.Reload()));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        CHECK_NOTHROW(text.SetValue("Italic").GetFont().SetStyle(FontStyles::Italic));
        CHECK_NOTHROW(fb.DrawText(text.Reload()));
        CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        CHECK_NOTHROW(text.SetValue("Bold Italic").GetFont().SetStyle(FontStyles::BoldItalic));
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
        Color blue(Color::Blue);
        Color green(Color::Green);
        Color red(Color::Red);

        for (std::uint32_t a = 0; a < 200 ; a += 5) {
            blue.SetAlpha(static_cast<std::uint8_t>(a));
            green.SetAlpha(static_cast<std::uint8_t>(a));
            red.SetAlpha(static_cast<std::uint8_t>(a));
            CHECK_NOTHROW(fb.DrawRectangle(Rect(135,170, 100, 100), red, true));
            CHECK_NOTHROW(fb.DrawRectangle(Rect(100,100, 100, 100), blue, true));
            CHECK_NOTHROW(fb.DrawRectangle(Rect(170,100, 100, 100), green, true));
            CHECK_NOTHROW(fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        CHECK(fb.GetPixel(Point(185, 185), true) == Color(0xFF0B612D));
    }

}

TEST_SUITE_END();
