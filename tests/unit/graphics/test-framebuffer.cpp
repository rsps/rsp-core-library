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
#include <graphics/Bitmap.h>
#include <graphics/Font.h>
#include <graphics/Text.h>
#include <graphics/Renderer.h>
#include <graphics/SW/Framebuffer.h>
#include <thread>
#include <filesystem>
#include <posix/FileSystem.h>
#include <utils/StopWatch.h>
#include <TestHelpers.h>
#include <utils/Random.h>
#include <TestHelpers.h>
#include <magic_enum.hpp>

using namespace rsp::graphics;
using namespace rsp::utils;

static void CheckPixel(GuiUnit_t aX, GuiUnit_t aY, Color aColor, const Renderer &fb)
{
    if (Rect(0, 0, fb.GetWidth(), fb.GetHeight()).IsHit(aX, aY)) {
        CHECK_HEX(fb.GetPixel(aX, aY).AsUint(), aColor.AsUint());
    } else {
        CHECK_HEX(fb.GetPixel(aX, aY).AsUint(), 0);
    }
}

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Framebuffer")
{
    using namespace std::chrono;
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    Random::Seed(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

#ifdef USE_GFX_SW
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    sw::Framebuffer::mpDevicePath = p.c_str();
#endif

    auto &renderer = Renderer::Init(480, 800);

    Canvas canvas(renderer.GetWidth(), renderer.GetHeight());
    CHECK_NOTHROW(Texture::Create(renderer.GetWidth(), renderer.GetHeight()));
    auto __tx = Texture::Create(renderer.GetWidth(), renderer.GetHeight());
    auto &texture = *__tx;

    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    srand(ms.count()); // generates random seed val
    Color col( Random::Roll(56u, 200u), Random::Roll(56u, 200u), Random::Roll(56u, 200u), 0xff);
    MESSAGE("Color: " << col.AsUint());

    SUBCASE("Fill")
    {
        Color colors[] { Color::Red, Color::Blue, Color::Green };

        for (Color color : colors) {
            renderer.Fill(color);
            renderer.Present();
            renderer.Fill(color);
            renderer.Present();

            uint32_t fb_value = renderer.GetPixel(0, 0).AsUint();
            CHECK_HEX(fb_value, color.AsUint());
            fb_value = renderer.GetPixel(479, 799).AsUint();
            CHECK_HEX(fb_value, color.AsUint());
            std::this_thread::sleep_for(300ms);
        }
    }

    SUBCASE("Clear")
    {
        renderer.Fill(Color::None);
        renderer.Present();
        renderer.Fill(Color::None);
        renderer.Present();
        canvas.Fill(Color::None);
        texture.Fill(Color::None);
    }

    SUBCASE("Drawing Lines")
    {
        // Arrange
        Point pointA(Random::Roll(0, renderer.GetWidth()-1), Random::Roll(0, renderer.GetHeight()-1));
        Point pointB(Random::Roll(0, renderer.GetWidth()-1), Random::Roll(0, renderer.GetHeight()-1));

        // Act
        CHECK_NOTHROW(canvas.DrawLine(pointA, pointB, col));
        CHECK_NOTHROW(texture.Update(canvas, col));
        CHECK_NOTHROW(renderer.Blit(texture));

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
                CHECK_HEX(canvas.GetPixelAt(px, py).AsUint(), col.AsUint());
                CHECK_HEX(renderer.GetPixel(px, py, false).AsUint(), col.AsUint());
            }
        } else {
            for (int i = 0; i < absDeltaY; i++) {
                x += absDeltaX;
                if (x >= absDeltaY) {
                    x -= absDeltaY;
                    px += signumX;
                }
                py += signumY;
                CHECK_HEX(canvas.GetPixelAt(px, py).AsUint(), col.AsUint());
                CHECK_HEX(renderer.GetPixel(px, py, false).AsUint(), col.AsUint());
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
        CHECK_NOTHROW(texture.Update(canvas, col));
        CHECK_NOTHROW(renderer.Blit(texture));

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
        CHECK_NOTHROW(texture.Update(canvas, white));
        CHECK_NOTHROW(renderer.Blit(texture));
        CHECK_EQ(renderer.GetPixel(10, 10), white);
        CHECK_NE(renderer.GetPixel(11, 10), white);
        CHECK_NE(renderer.GetPixel(10, 11), white);
        CHECK_NE(renderer.GetPixel(11, 11), white);
        CHECK_NOTHROW(renderer.Present());

        Rect medium(20, 20, 10, 10);
        CHECK_NOTHROW(canvas.DrawRectangle(medium, white));
        CHECK_NOTHROW(texture.Update(canvas, white));
        CHECK_NOTHROW(renderer.Blit(texture));
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
        CHECK_NOTHROW(texture.Update(canvas, col));
        CHECK_NOTHROW(renderer.Blit(texture));

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
        CHECK_HEX(testImgMap.GetPixelAt(50, 50).AsUint(), 0xFF031b95);

        SUBCASE("Draw image from file")
        {
            // Act
            auto raster = Texture::Create(testImgMap, Color::White);
            CHECK_NOTHROW(raster->SetDestination(topLeftImgCorner));
            CHECK_NOTHROW(renderer.Blit(*raster));

            // Assert
            CHECK_EQ(renderer.GetPixel(topLeftImgCorner.GetX() + 4, topLeftImgCorner.GetY() + 4, false).AsUint(), 0xFF020F92);

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

            auto raster = Texture::Create(testImgMap, Color::White);
            CHECK_NOTHROW(raster->SetDestination(topLeftImgCorner));
            CHECK_NOTHROW(renderer.Blit(*raster));
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
            Bitmap emptyMap(width, height, ColorDepth::RGBA);
            Point randomPoint(Random::Roll(0, width-1), Random::Roll(0, height-1));

            MESSAGE("randomPoint: " << randomPoint);
//            MESSAGE("topLeftImgCorner: " << topLeftImgCorner);
            MESSAGE("Combined: " << (topLeftImgCorner + randomPoint));

            // Act
            CHECK_NOTHROW(emptyMap.DrawRectangle(Rect(0, 0, width, height), col, true) ); //.SetPixel(randomPoint, col));
            CHECK_NOTHROW(canvas.DrawPixelData(topLeftImgCorner, emptyMap));
            CHECK_NOTHROW(canvas.SetPixel(topLeftImgCorner + randomPoint, Color::White));
            CHECK_NOTHROW(texture.Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Blit(texture));
            CHECK_NOTHROW(renderer.Present());

            // Assert
            CHECK_EQ(renderer.GetPixel(topLeftImgCorner, true), col.AsUint());
            CHECK_EQ(renderer.GetPixel(topLeftImgCorner + Point(width-1, height-1), true), col.AsUint());
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
        CHECK_NOTHROW(texture.Update(largeImgMap, Color::White));
        CHECK_NOTHROW(renderer.Blit(texture));
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
            CHECK_NOTHROW(texture.Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Blit(texture));

            // Assert
            CHECK_NE(renderer.GetPixel(randomPoint, true), 0);
            CHECK_NOTHROW(renderer.Present());
        }
    }

    SUBCASE("Moving image")
    {
        // Arrange
        Bitmap imgSimple("testImages/testImage.bmp");
        int iterations = 100;

        auto sprite = Texture::Create(imgSimple, Color::Black);
        Point pos(100, 200);

        // Act
        rsp::utils::StopWatch sw;
        for (int i = 0; i < iterations; i++) {
            CHECK_NOTHROW(renderer.Fill(Color::Black));
            CHECK_NOTHROW(sprite->SetDestination(pos));
            CHECK_NOTHROW(renderer.Blit(*sprite));
            CHECK_NOTHROW(renderer.Present());
            pos.SetY(200 - i);
        }
        int fps = (1000 * iterations) / (sw.Elapsed<std::chrono::milliseconds>() + 1);

        // Assert
        CHECK(fps > 10);
        MESSAGE("Fps: " << fps);
    }

    SUBCASE("Moving image2")
    {
        CHECK_NOTHROW(renderer.Fill(Color::Black));
        CHECK_NOTHROW(renderer.Present());
        CHECK_NOTHROW(renderer.Fill(Color::Black));
        // Arrange
        Bitmap imgSimple("testImages/testImage.bmp");
        int iterations = 100;

        auto sprite = Texture::Create(imgSimple.GetWidth(), imgSimple.GetHeight()+2);
        sprite->Fill(Color::Black);
        sprite->Update(imgSimple, Color::Black);

        Point pos(100, 200);

        // Act
        rsp::utils::StopWatch sw;
        for (int i = 0; i < iterations; i++) {
            CHECK_NOTHROW(sprite->SetDestination(pos));
            CHECK_NOTHROW(renderer.Blit(*sprite));
            CHECK_NOTHROW(renderer.Present());
            pos.SetY(200 - i);
        }
        int fps = (1000 * iterations) / (sw.Elapsed<std::chrono::milliseconds>() + 1);

        // Assert
        CHECK(fps > 10);
        MESSAGE("Fps: " << fps);
    }

    SUBCASE("Moving monochrome image")
    {
        CHECK_NOTHROW(renderer.Fill(Color::Black));
        CHECK_NOTHROW(renderer.Present());
        CHECK_NOTHROW(renderer.Fill(Color::Black));

        // Arrange
        Bitmap imgSimple("testImages/monochrome/Monochrome.bmp");
        int iterations = 100;
        Color mcl[5] = {
            Color::White,
            Color::Blue,
            Color::Red,
            Color::Green,
            Color::Yellow
        };


        auto sprite = Texture::Create(imgSimple.GetWidth(), imgSimple.GetHeight()+5);
        CHECK_NOTHROW(sprite->Fill(Color::Black));
        CHECK_NOTHROW(sprite->SetBlendOperation(Texture::BlendOperation::SourceAlpha));
        Point pos(100, 200);

        // Act
        rsp::utils::StopWatch sw;
        for (int i = 0; i < iterations; i++) {
            if ((i % 20) == 0) {
                CHECK_NOTHROW(sprite->Update(imgSimple, mcl[(i / 20) % 5]));
            }
            CHECK_NOTHROW(sprite->SetDestination(pos));
            CHECK_NOTHROW(renderer.Blit(*sprite));
            CHECK_NOTHROW(renderer.Present());
            pos.SetY(200 - i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        int fps = (1000 * iterations) / (sw.Elapsed<std::chrono::milliseconds>() + 1);

        // Assert
        CHECK(fps > 10);
        MESSAGE("Fps: " << fps);
    }

    SUBCASE("Draw Text")
    {
        CHECK_NOTHROW(renderer.Fill(Color::Black));
        CHECK_NOTHROW(renderer.Present());
        CHECK_NOTHROW(renderer.Fill(Color::Black));

        const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
        Font::RegisterFont(cFontFile);
        Rect r(100, 200, 280, 200);

        Text text("Exo 2", "Hello World");
        CHECK_NOTHROW(text.GetFont().SetSize(30));

        auto panel = Texture::Create(280, 200);
        CHECK_NOTHROW(panel->SetBlendOperation(Texture::BlendOperation::ColorKey));

        SUBCASE("Text Attributes") {
            CHECK_NOTHROW(panel->Fill(Color::None));
            Color text_color;

            SUBCASE("Constructor") {
                CHECK_NOTHROW(renderer.DrawRect(Color::White, r));
                text_color = Color::Red;
            }
            SUBCASE("SetValue") {
                CHECK_NOTHROW(renderer.DrawRect(Color::Grey, r));
                text_color = Color::Green;
                CHECK_NOTHROW(text.SetValue("Hello\nWorld\nHello"));
            }
            SUBCASE("MOON") {
                CHECK_NOTHROW(renderer.DrawRect(Color::Purple, r));
                text_color = Color::Grey;
                CHECK_NOTHROW(text.SetValue("Hello\nWorld\nHELLO\nMOON"));
            }
            SUBCASE("RED") {
                CHECK_NOTHROW(renderer.DrawRect(Color::White, r));
                text_color = Color::Red;
                CHECK_NOTHROW(text.SetValue("RED"));
            }
            SUBCASE("GREEN") {
                CHECK_NOTHROW(renderer.DrawRect(Color::White, r));
                text_color = Color::Green;
                CHECK_NOTHROW(text.SetValue("GREEN"));
            }
            SUBCASE("BLUE") {
                CHECK_NOTHROW(renderer.DrawRect(Color::White, r));
                text_color = Color::Blue;
                CHECK_NOTHROW(text.SetValue("BLUE"));
            }

            CHECK_NOTHROW(text.Reload(r));
            CHECK_NOTHROW(panel->Update(text, text_color));
            CHECK_NOTHROW(panel->SetDestination(text.GetPosition(r)));
            CHECK_NOTHROW(renderer.Blit(*panel));
            CHECK_NOTHROW(renderer.Present());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        SUBCASE("Rainbow") {
            const Color rainbow[] = { Color::White, Color::Red, Color::Yellow, Color::Green, Color::Aqua, Color::Lime, Color::Blue, Color::Silver };

            MESSAGE("FPS Test");
            CHECK_NOTHROW(text.SetLineSpacing(50).SetValue("FPS:\n0").Reload(r));
            panel = Texture::Create(text, Color::Black);
            CHECK_NOTHROW(panel->SetBlendOperation(Texture::BlendOperation::ColorKey).SetDestination(Point(100, 200)));

            rsp::utils::StopWatch sw;
            for (int i = 0 ; i < 200 ; i++) {
                CHECK_NOTHROW(renderer.Fill(rainbow[i & 0x07]));
                int fps = (1000 * i) / (sw.Elapsed<std::chrono::milliseconds>() + 1);
                std::stringstream ss;
                ss << "FPS:\n" << fps;
                CHECK_NOTHROW(text.SetValue(ss.str()).Reload());
                CHECK_NOTHROW(panel->Fill(Color::None));
                CHECK_NOTHROW(panel->Update(text, Color::Black));
                CHECK_NOTHROW(renderer.Blit(*panel));
                CHECK_NOTHROW(renderer.Present());
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
        Rect r(10, 10, 460, 780);

        Text text("Exo 2", "Hello\nWorld");
        CHECK_NOTHROW(text.GetFont().SetSize(50));
        CHECK_NOTHROW(text.Reload());

        auto panel = Texture::Create(text.GetWidth(), text.GetHeight());

        CHECK_NOTHROW(panel->SetBlendOperation(Texture::BlendOperation::ColorKey, Color::None));
        CHECK_NOTHROW(text.Reload());
        CHECK_NOTHROW(panel->Fill(Color::None).Update(text, Color::Yellow));

        const Text::VAlign cVertical[] = { Text::VAlign::Top, Text::VAlign::Center, Text::VAlign::Bottom };
        const Text::HAlign cHorizontal[] = { Text::HAlign::Left, Text::HAlign::Center, Text::HAlign::Right };

        for (int h = 0 ; h < 3 ; h++) {
            for (int v = 0 ; v < 3 ; v++) {
                CHECK_NOTHROW(text.SetVAlignment(cVertical[v]).SetHAlignment(cHorizontal[h]));
                CHECK_NOTHROW(panel->SetDestination(text.GetPosition(r)));

                CHECK_NOTHROW(renderer.Fill(Color::Black));
                CHECK_NOTHROW(renderer.Blit(*panel));
                CHECK_NOTHROW(renderer.DrawRect(Color::White, r));
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

        auto panel = Texture::Create(r.GetWidth(), r.GetHeight());
        CHECK_NOTHROW(panel->SetBlendOperation(Texture::BlendOperation::ColorKey, Color::None));

        Text text("Exo 2", "Regular");
        CHECK_NOTHROW(text.GetFont().SetSize(50));

        SUBCASE("Normal") {
        }
        SUBCASE("Bold") {
            CHECK_NOTHROW(text.SetValue("Bold").GetFont().SetStyle(FontStyles::Bold));
        }
        SUBCASE("Italic") {
            CHECK_NOTHROW(text.SetValue("Italic").GetFont().SetStyle(FontStyles::Italic));
        }
        SUBCASE("BoldItalic") {
            CHECK_NOTHROW(text.SetValue("Bold Italic").GetFont().SetStyle(FontStyles::BoldItalic));
        }

        CHECK_NOTHROW(text.Reload(r));
        CHECK_NOTHROW(panel->Fill(Color::None).Update(text, Color::Yellow));
        CHECK_NOTHROW(panel->SetDestination(text.GetPosition(r)));
        CHECK_NOTHROW(renderer.Fill(Color::Black));
        CHECK_NOTHROW(renderer.DrawRect(Color::Green, r2));
        CHECK_NOTHROW(renderer.Blit(*panel));
        CHECK_NOTHROW(renderer.Present());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    SUBCASE("Swapbuffer")
    {
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Draw Fullscreen") {
        std::string testImage = "testImages/rgba/Asset2WithAlpha.bmp";
        Bitmap testImgMap(testImage);

        texture.SetBlendOperation(Texture::BlendOperation::Copy);

        CHECK_NOTHROW(texture.Update(testImgMap, Color::White));
        CHECK_NOTHROW(renderer.Blit(texture));
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Draw Transparent") {
        Color blue(Color::Blue);
        Color green(Color::Green);
        Color red(Color::Red);

        for (std::uint8_t a = 0; a < 200 ; a += 5) {
            blue.SetAlpha(a);
            green.SetAlpha(a);
            red.SetAlpha(a);
            CHECK_NOTHROW(canvas.Fill(Color::Black));
            CHECK_NOTHROW(canvas.DrawRectangle(Rect(135,170, 100, 100), red, true));
            CHECK_NOTHROW(canvas.DrawRectangle(Rect(100,100, 100, 100), blue, true));
            CHECK_NOTHROW(canvas.DrawRectangle(Rect(170,100, 100, 100), green, true));
            CHECK_NOTHROW(texture.Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Blit(texture));
            CHECK_NOTHROW(renderer.Present());
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        CHECK_HEX(renderer.GetPixel(Point(180, 180), true).AsUint(), 0xFF386100);
    }

    std::this_thread::sleep_for(1000ms);
}

TEST_SUITE_END();
