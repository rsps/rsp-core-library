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
#include <graphics/Font.h>
#include <graphics/Bitmap.h>
#include <graphics/Text.h>
#include <graphics/SW/SWRenderer.h>
#include <thread>
#include <filesystem>
#include <posix/FileSystem.h>
#include <utils/StopWatch.h>
#include <TestHelpers.h>
#include <utils/Random.h>

using namespace rsp::graphics;
using namespace rsp::utils;

inline void CheckPixel(GuiUnit_t aX, GuiUnit_t aY, Color aColor, const Framebuffer &fb)
{
    if (fb.IsHit(aX, aY)) {
        CHECK_EQ(fb.GetPixel(aX, aY), aColor.AsUint());
    } else {
        CHECK_EQ(fb.GetPixel(aX, aY), 0);
    }
}

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Framebuffer")
{
    using namespace std::chrono;
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    Random::Seed(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});

    SWRenderer renderer(p);
    Canvas canvas(renderer.GetWidth(), renderer.GetHeight());
    auto _tx = renderer.CreateTexture();
    SWTexture* texture = dynamic_cast<SWTexture*>(_tx.get());
    CHECK(texture != nullptr);

    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    srand(ms.count()); // generates random seed val
    Color col( Random::Roll(56u, 200u), Random::Roll(56u, 200u), Random::Roll(56u, 200u), 0xff);
    MESSAGE("Color: " << col.AsUint());

    SUBCASE("Clear")
    {
        renderer.Fill(Color::None);
        renderer.Present();
        renderer.Fill(Color::None);
        renderer.Present();
        canvas.Fill(Color::None);
        texture->Fill(Color::None);
    }

    SUBCASE("Drawing Lines")
    {
        // Arrange
        Point pointA(Random::Roll(0, renderer.GetWidth()-1), Random::Roll(0, renderer.GetHeight()-1));
        Point pointB(Random::Roll(0, renderer.GetWidth()-1), Random::Roll(0, renderer.GetHeight()-1));

        // Act
        CHECK_NOTHROW(canvas.DrawLine(pointA, pointB, col));
        CHECK_NOTHROW(texture->Update(canvas, col));
        CHECK_NOTHROW(renderer.Render(*texture));

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
                CHECK_EQ(canvas.GetPixelAt(px, py).AsUint(), col.AsUint());
                CHECK_EQ(texture->GetPixelData().GetPixelAt(px, py).AsUint(), col.AsUint());
                CHECK_EQ(renderer.GetPixel(px, py, false), col.AsUint());
            }
        } else {
            for (int i = 0; i < absDeltaY; i++) {
                x += absDeltaX;
                if (x >= absDeltaY) {
                    x -= absDeltaY;
                    px += signumX;
                }
                py += signumY;
                CHECK_EQ(canvas.GetPixelAt(px, py), col.AsUint());
                CHECK_EQ(texture->GetPixelData().GetPixelAt(px, py), col.AsUint());
                CHECK_EQ(renderer.GetPixel(px, py, false), col.AsUint());
            }
        }

        SUBCASE("Lines are Inclusive")
        {
            CHECK_EQ(renderer.GetPixel(pointA.GetX(), pointA.GetY(), false), col.AsUint());
            CHECK_EQ(renderer.GetPixel(pointB.GetX(), pointB.GetY(), false), col.AsUint());
        }

        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Drawing Rectangles")
    {
        // Arrange
        // Generate random values in the LEFT and TOP halves of the screen
        Point leftTop(Random::Roll(0, renderer.GetWidth() / 2), Random::Roll(0, renderer.GetHeight() / 2));
        // Generate random values in the RIGHT and BOTTOM halves of the screen
        Point rightBottom(
            Random::Roll(0, renderer.GetWidth() / 2) + (renderer.GetWidth() / 2),
            Random::Roll(0, renderer.GetHeight() / 2) + (renderer.GetHeight() / 2));
        Rect rect(leftTop, rightBottom.GetX() - leftTop.GetX(), rightBottom.GetY() - leftTop.GetY());

        // Act
        CHECK_NOTHROW(canvas.DrawRectangle(rect, col));
        CHECK_NOTHROW(texture->Update(canvas, col));
        CHECK_NOTHROW(renderer.Render(*texture));

        // Assert
        // Expect all four side to hold values
        for (GuiUnit_t i = 0; i < rect.GetWidth(); i++) {
            // Check top side
            CHECK_EQ(canvas.GetPixelAt(rect.GetLeft() + i, rect.GetTop()).AsUint(), col.AsUint());
            CHECK_EQ(col.AsUint(), renderer.GetPixel(leftTop.GetX() + i, leftTop.GetY(), false));
            // Check bottom side
            CHECK_EQ(canvas.GetPixelAt(rect.GetLeft() + i, rect.GetBottom() - 1).AsUint(), col.AsUint());
            CHECK_EQ(col.AsUint(), renderer.GetPixel(leftTop.GetX() + i, rightBottom.GetY()-1, false));
        }
        for (GuiUnit_t i = 0; i < rect.GetHeight(); i++) {
            // Check left side
            CHECK_EQ(col.AsUint(), renderer.GetPixel(leftTop.GetX(), rightBottom.GetY()-1 - i, false));
            // Check right side
            CHECK_EQ(col.AsUint(), renderer.GetPixel(rightBottom.GetX()-1, rightBottom.GetY()-1 - i, false));
        }
        CHECK_NOTHROW(renderer.Present());

        Rect small(10, 10, 1, 1);
        Color white(Color::White);
        CHECK_NOTHROW(canvas.DrawRectangle(small, white));
        CHECK_NOTHROW(texture->Update(canvas, white));
        CHECK_NOTHROW(renderer.Render(*texture));
        CHECK_EQ(renderer.GetPixel(10, 10), white);
        CHECK_NE(renderer.GetPixel(11, 10), white);
        CHECK_NE(renderer.GetPixel(10, 11), white);
        CHECK_NE(renderer.GetPixel(11, 11), white);
        CHECK_NOTHROW(renderer.Present());

        Rect medium(20, 20, 10, 10);
        CHECK_NOTHROW(canvas.DrawRectangle(medium, white));
        CHECK_NOTHROW(texture->Update(canvas, white));
        CHECK_NOTHROW(renderer.Render(*texture));
        CHECK_EQ(renderer.GetPixel(20, 20), white);
        CHECK_EQ(renderer.GetPixel(20, 29), white);
        CHECK_NE(renderer.GetPixel(20, 30), white);
        CHECK_EQ(renderer.GetPixel(29, 20), white);
        CHECK_NE(renderer.GetPixel(30, 20), white);
        CHECK_EQ(renderer.GetPixel(29, 29), white);
        CHECK_NE(renderer.GetPixel(30, 30), white);
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Drawing Circles")
    {
        // Arrange
        Point centerPoint(Random::Roll(0, renderer.GetWidth()), Random::Roll(0, renderer.GetHeight()));
        GuiUnit_t radius = Random::Roll(0, renderer.GetWidth() / 2);

        // Act
        CHECK_NOTHROW(canvas.DrawCircle(centerPoint, radius, col));
        CHECK_NOTHROW(texture->Update(canvas, col));
        CHECK_NOTHROW(renderer.Render(*texture));

        // Assert
        int error = -static_cast<int>(radius);
        GuiUnit_t y = 0;
        while (radius >= y) {
            CheckPixel(centerPoint.GetX() + radius, centerPoint.GetY() + y, col, renderer);
            CheckPixel(centerPoint.GetX() - radius, centerPoint.GetY() + y, col, renderer);
            CheckPixel(centerPoint.GetX() + radius, centerPoint.GetY() - y, col, renderer);
            CheckPixel(centerPoint.GetX() - radius, centerPoint.GetY() - y, col, renderer);
            CheckPixel(centerPoint.GetX() + y, centerPoint.GetY() + radius, col, renderer);
            CheckPixel(centerPoint.GetX() - y, centerPoint.GetY() + radius, col, renderer);
            CheckPixel(centerPoint.GetX() + y, centerPoint.GetY() - radius, col, renderer);
            CheckPixel(centerPoint.GetX() - y, centerPoint.GetY() - radius, col, renderer);
            error += static_cast<int>(y);
            y++;
            error += static_cast<int>(y);

            if (error >= 0) {
                error += -static_cast<int>(radius);
                radius--;
                error += -static_cast<int>(radius);
            }
        }
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Set/Get pixel outside screen")
    {
        // Arrange
        Point outSideXAxis(-1, 0);
        Point outSideYAxis(0, -1);

        // Act
        CHECK_NOTHROW(renderer.SetPixel(outSideXAxis.GetX(), outSideXAxis.GetY(), col));
        CHECK_NOTHROW(renderer.SetPixel(outSideYAxis.GetX(), outSideYAxis.GetY(), col));

        // Assert
        CHECK_EQ(renderer.GetPixel(outSideXAxis.GetX(), outSideXAxis.GetY()), 0);
        CHECK_EQ(renderer.GetPixel(outSideYAxis.GetX(), outSideYAxis.GetY()), 0);
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
        CHECK(testImgMap.IsHit(topLeft));
        CHECK(testImgMap.IsHit(topRight));
        CHECK(testImgMap.IsHit(botLeft));
        CHECK(testImgMap.IsHit(botRight));
        CHECK_EQ(testImgMap.GetHeight(), height);
        CHECK_EQ(testImgMap.GetWidth(), width);
        CHECK_EQ(testImgMap.GetPixelData().GetDataSize(), (width * height * 3));

        SUBCASE("Draw image from file")
        {
            // Act
            CHECK_NOTHROW(canvas.DrawPixelData(topLeftImgCorner, testImgMap, testImgMap.GetRect(), Color::White));
            CHECK_NOTHROW(texture->Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Render(*texture));

            // Assert
            CHECK_EQ(renderer.GetPixel(topLeftImgCorner.GetX() + 4, topLeftImgCorner.GetY() + 4, false), Color(0xFF020F92));

            CHECK_NOTHROW(renderer.Present());
        }
        SUBCASE("Draw edited image file")
        {
            // Arrange
            auto offset = [](Point &point, int val) { return (point + Point(val, val)); };

            // Act
            for (int i=0; i < 16 ;++i){
                uint8_t val = 0x77+(i*8);
                Color grey(val, val, val, 0xFF);
                CHECK_NOTHROW(testImgMap.DrawRectangle(Rect(offset(topLeft, i), width-(i*2), height-(i*2)), grey));
            }

            CHECK_NOTHROW(testImgMap.DrawLine(topLeft, botRight, Color::Red));
            CHECK_NOTHROW(testImgMap.DrawLine(botLeft, topRight, Color::Red));

            CHECK_NOTHROW(canvas.DrawPixelData(topLeftImgCorner, testImgMap, testImgMap.GetRect(), Color::White));
            CHECK_NOTHROW(texture->Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Render(*texture, &canvas.GetRect()));
            CHECK_NOTHROW(renderer.Present());

            // Assert
            CHECK_EQ(testImgMap.GetPixelAt(width / 2, 0).AsUint(), 0xFF777777);
            CHECK_EQ(testImgMap.GetPixelAt(0, height / 2).AsUint(), 0xFF777777);
            CHECK_EQ(testImgMap.GetPixelAt(width - 12, height / 2).AsUint(), 0xFFCFCFCF);
            CHECK_EQ(testImgMap.GetPixelAt(width / 2, height - 4).AsUint(), 0xFF8F8F8F);

            CHECK_EQ(renderer.GetPixel(topLeftImgCorner.GetX() + width / 2 , topLeftImgCorner.GetY() +          0, true), 0xFF777777);
            CHECK_EQ(renderer.GetPixel(topLeftImgCorner.GetX() + 0         , topLeftImgCorner.GetY() + height / 2, true), 0xFF777777);
            CHECK_EQ(renderer.GetPixel(topLeftImgCorner.GetX() + width - 12, topLeftImgCorner.GetY() + height / 2, true), 0xFFCFCFCF);
            CHECK_EQ(renderer.GetPixel(topLeftImgCorner.GetX() + width / 2 , topLeftImgCorner.GetY() + height - 4, true), 0xFF8F8F8F);
        }
        SUBCASE("Draw memory created image")
        {
            // Arrange
            Bitmap emptyMap(height, width, PixelData::ColorDepth::RGBA);
            Point randomPoint(Random::Roll(0, width-1), Random::Roll(0, height-1));

//            MESSAGE("randomPoint: " << randomPoint);
//            MESSAGE("topLeftImgCorner: " << topLeftImgCorner);
//            MESSAGE("Combined: " << (topLeftImgCorner + randomPoint));

            // Act
            CHECK_NOTHROW(emptyMap.DrawRectangle(Rect(0, 0, width, height), col, true) ); //.SetPixel(randomPoint, col));
            CHECK_NOTHROW(canvas.DrawPixelData(topLeftImgCorner, emptyMap));
            CHECK_NOTHROW(canvas.SetPixel(topLeftImgCorner + randomPoint, Color::White));
            CHECK_NOTHROW(texture->Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Render(*texture));
            CHECK_NOTHROW(renderer.Present());

            // Assert
            CHECK_EQ(renderer.GetPixel(topLeftImgCorner, true), col.AsUint());
            CHECK_EQ(renderer.GetPixel(topLeftImgCorner + randomPoint, true), Color::White);
        }
    }

    SUBCASE("Drawing image larger than screen")
    {
        // Arrange
        Point topLeft(0, 0);
        Point randomPoint(Random::Roll(0, renderer.GetWidth()-1), Random::Roll(0, renderer.GetHeight()-1));
        std::string largeImg = "testImages/largeTestImg.bmp";
        Bitmap largeImgMap(largeImg);
        // Make sure screen is empty
        CHECK_NOTHROW(renderer.Present());

        // Act
        CHECK_NOTHROW(texture->Update(largeImgMap, Color::White));
        CHECK_NOTHROW(renderer.Render(*texture));
        CHECK_NOTHROW(renderer.Present());

        // Assert
        CHECK(largeImgMap.GetHeight() > renderer.GetHeight());
        CHECK(largeImgMap.GetWidth() > renderer.GetWidth());
        CHECK_NE(largeImgMap.GetPixel(randomPoint), 0);
        CHECK_NE(renderer.GetPixel(randomPoint, true), 0);

        SUBCASE("Spill large image into screen")
        {
            // Arrange
            topLeft = {-100, -100};

            // Act
            CHECK_NOTHROW(canvas.DrawPixelData(topLeft, largeImgMap));
            CHECK_NOTHROW(texture->Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Render(*texture));

            // Assert
            CHECK_NE(renderer.GetPixel(randomPoint, true), 0);
            CHECK_NOTHROW(renderer.Present());
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
            CHECK_NOTHROW(canvas.Fill(Color::Black));
            CHECK_NOTHROW(canvas.DrawPixelData(topLeftPoint + Point(0, static_cast<GuiUnit_t>(-i)), imgSimple));
            CHECK_NOTHROW(texture->Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Render(*texture));
            CHECK_NOTHROW(renderer.Present());
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
            CHECK_NOTHROW(canvas.Fill(Color::Black));
            CHECK_NOTHROW(canvas.DrawPixelData(topLeftPoint + Point(0, static_cast<GuiUnit_t>(-i)), imgSimple, imgSimple.GetRect(), mcl[(i / 20) % 5]));
            CHECK_NOTHROW(texture->Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Render(*texture));
            CHECK_NOTHROW(renderer.Present());
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
        bool scale = true;

        Text text("Exo 2", "Hello World");
        CHECK_NOTHROW(text.SetArea(r).GetFont().SetSize(30));

        SUBCASE("Text Attributes") {
            SUBCASE("Constructor") {
                CHECK_NOTHROW(canvas.DrawRectangle(r, Color::White));
                CHECK_NOTHROW(text.GetFont().SetColor(Color::Red));
                scale = false;
            }
            SUBCASE("SetValue") {
                CHECK_NOTHROW(canvas.DrawRectangle(r, Color::Grey));
                CHECK_NOTHROW(text.SetValue("Hello\nWorld").SetColor(Color::Green));
            }
            SUBCASE("MOON") {
                CHECK_NOTHROW(canvas.DrawRectangle(r, Color::Purple));
                CHECK_NOTHROW(text.SetValue("Hello\nWorld\nHELLO\nMOON").SetColor(Color::Grey));
            }
            SUBCASE("RED") {
                CHECK_NOTHROW(canvas.DrawRectangle(r, Color::White));
                CHECK_NOTHROW(text.SetValue("RED").SetColor(Color::Red));
            }
            SUBCASE("GREEN") {
                CHECK_NOTHROW(canvas.DrawRectangle(r, Color::White));
                CHECK_NOTHROW(text.SetValue("GREEN").SetColor(Color::Green));
            }
            SUBCASE("BLUE") {
                CHECK_NOTHROW(canvas.DrawRectangle(r, Color::White));
                CHECK_NOTHROW(text.SetValue("BLUE").SetColor(Color::Blue));
            }

            CHECK_NOTHROW(canvas.DrawPixelData(text.GetArea().GetTopLeft(), text.SetScaleToFit(scale).Reload()));
            CHECK_NOTHROW(texture->Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Render(*texture));
            CHECK_NOTHROW(renderer.Present());
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        SUBCASE("Rainbow") {
            const Color rainbow[] = { Color::White, Color::Red, Color::Yellow, Color::Green, Color::Aqua, Color::Lime, Color::Blue, Color::Silver };

            MESSAGE("FPS Test");
            CHECK_NOTHROW(text.SetScaleToFit(true).SetLineSpacing(50).GetFont().SetColor(Color::White).SetBackgroundColor(Color::Black));
    //        Rect screen(0, 0, 480, 800);
            rsp::utils::StopWatch sw;
            for (int i = 0 ; i < 500 ; i++) {
    //            fb.DrawRectangle(screen, rainbow[i & 0x07], true);
                CHECK_NOTHROW(canvas.Fill(rainbow[i & 0x07]));
//                CHECK_NOTHROW(text.GetFont().SetBackgroundColor(rainbow[i & 0x07]));
                int fps = (1000 * i) / (sw.Elapsed<std::chrono::milliseconds>() + 1);
                std::stringstream ss;
                ss << "FPS:\n" << fps;
                CHECK_NOTHROW(text.SetValue(ss.str()).Reload());
                CHECK_NOTHROW(canvas.DrawPixelData(text.GetArea().GetTopLeft(), text));
                CHECK_NOTHROW(texture->Update(canvas, Color::None));
                CHECK_NOTHROW(renderer.Render(*texture));
                CHECK_NOTHROW(renderer.Present());

                CHECK_NOTHROW(text.SetScaleToFit(false)); // Only scale first time to speed it up
            }
            MESSAGE(text.GetValue());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
//        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Text Alignment")
    {
        const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
        Font::RegisterFont(cFontFile);
//        Rect r(10, 10, 60, 80);
        Rect r(10, 10, 460, 780);

        CHECK_NOTHROW(canvas.DrawRectangle(r, Color::White));
        Text text("Exo 2", "Hello\nWorld");
        CHECK_NOTHROW(text.SetArea(r).GetFont().SetSize(50).SetColor(Color::Yellow));
        CHECK_NOTHROW(canvas.DrawPixelData(text.GetArea().GetTopLeft(), text.Reload()));
        CHECK_NOTHROW(texture->Update(canvas, Color::White));
        CHECK_NOTHROW(renderer.Render(*texture));
        CHECK_NOTHROW(renderer.Present());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        const Text::VAlign cVertical[] = { Text::VAlign::Top, Text::VAlign::Center, Text::VAlign::Bottom };
        const Text::HAlign cHorizontal[] = { Text::HAlign::Left, Text::HAlign::Center, Text::HAlign::Right };

        for (int h = 0 ; h < 3 ; h++) {
            for (int v = 0 ; v < 3 ; v++) {
                CHECK_NOTHROW(canvas.Fill(Color::Black));
                CHECK_NOTHROW(canvas.DrawRectangle(r, Color::White));
                CHECK_NOTHROW(text.SetVAlignment(cVertical[v]).SetHAlignment(cHorizontal[h]));
                CHECK_NOTHROW(canvas.DrawPixelData(text.GetArea().GetTopLeft(), text.Reload()));
                CHECK_NOTHROW(texture->Update(canvas, Color::White));
                CHECK_NOTHROW(renderer.Render(*texture));
                CHECK_NOTHROW(renderer.Present());
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    SUBCASE("Font Styles")
    {
        CHECK_NOTHROW(Font::RegisterFont("fonts/Exo2-Italic-VariableFont_wght.ttf"));
        CHECK_NOTHROW(Font::RegisterFont("fonts/Exo2-VariableFont_wght.ttf"));
        Rect r(10, 10, 460, 280);
        auto r2 = r;
        r2.AddSize(2, 2).Move(-1, -1);

        CHECK_NOTHROW(canvas.Fill(Color::Black));
        CHECK_NOTHROW(canvas.DrawRectangle(r2, Color::White));
        Text text("Exo 2", "Regular");
        CHECK_NOTHROW(text.SetArea(r).SetScaleToFit(true).GetFont().SetSize(50).SetColor(Color::Yellow).SetBackgroundColor(Color::Black));

        CHECK_NOTHROW(canvas.DrawPixelData(text.GetArea().GetTopLeft(), text.Reload()));
        CHECK_NOTHROW(texture->Update(canvas, Color::White));
        CHECK_NOTHROW(renderer.Render(*texture));
        CHECK_NOTHROW(renderer.Present());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        CHECK_NOTHROW(text.SetValue("Bold").GetFont().SetStyle(FontStyles::Bold));
        CHECK_NOTHROW(canvas.DrawPixelData(text.GetArea().GetTopLeft(), text.Reload()));
        CHECK_NOTHROW(texture->Update(canvas, Color::White));
        CHECK_NOTHROW(renderer.Render(*texture));
        CHECK_NOTHROW(renderer.Present());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        CHECK_NOTHROW(text.SetValue("Italic").GetFont().SetStyle(FontStyles::Italic));
        CHECK_NOTHROW(canvas.DrawPixelData(text.GetArea().GetTopLeft(), text.Reload()));
        CHECK_NOTHROW(texture->Update(canvas, Color::White));
        CHECK_NOTHROW(renderer.Render(*texture));
        CHECK_NOTHROW(renderer.Present());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        CHECK_NOTHROW(text.SetValue("Bold Italic").GetFont().SetStyle(FontStyles::BoldItalic));
        CHECK_NOTHROW(canvas.DrawPixelData(text.GetArea().GetTopLeft(), text.Reload()));
        CHECK_NOTHROW(texture->Update(canvas, Color::White));
        CHECK_NOTHROW(renderer.Render(*texture));
        CHECK_NOTHROW(renderer.Present());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    SUBCASE("Swapbuffer")
    {
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Draw Fullscreen") {
        std::string testImage = "testImages/Asset2WithAlpha.bmp";
        Bitmap testImgMap(testImage);
        CHECK_NOTHROW(texture->Update(testImgMap, Color::White));
        CHECK_NOTHROW(renderer.Render(*texture));
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Draw Transparent") {
        Color blue(Color::Blue);
        Color green(Color::Green);
        Color red(Color::Red);

        for (std::uint32_t a = 0; a < 200 ; a += 5) {
            blue.SetAlpha(static_cast<std::uint8_t>(a));
            green.SetAlpha(static_cast<std::uint8_t>(a));
            red.SetAlpha(static_cast<std::uint8_t>(a));
            CHECK_NOTHROW(canvas.Fill(Color::Black));
            CHECK_NOTHROW(canvas.DrawRectangle(Rect(135,170, 100, 100), red, true));
            CHECK_NOTHROW(canvas.DrawRectangle(Rect(100,100, 100, 100), blue, true));
            CHECK_NOTHROW(canvas.DrawRectangle(Rect(170,100, 100, 100), green, true));
            CHECK_NOTHROW(texture->Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Render(*texture));
            CHECK_NOTHROW(renderer.Present());
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        CHECK(renderer.GetPixel(Point(185, 185), true) == 0xFF0B612D);
    }

}

TEST_SUITE_END();
