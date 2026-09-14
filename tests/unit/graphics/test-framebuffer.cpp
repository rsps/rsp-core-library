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
#include <rsp/config.h>
#include <rsp/exceptions/SignalHandler.h>
#include <rsp/graphics/Bitmap.h>
#include <rsp/graphics/Font.h>
#include <rsp/graphics/Text.h>
#include <rsp/graphics/Renderer.h>
#include <random>
#include <thread>
#include <filesystem>
#include <rsp/utils/StopWatch.h>
#include <TestHelpers.h>
#include <rsp/utils/Random.h>
#include <rsp/utils/EnumReflection.h>
#ifdef RSP_CORE_LIB_USE_GFX_SW
    #include <rsp/posix/FileSystem.h>
#endif

using namespace rsp::graphics;
using namespace rsp::utils;
using namespace rsp::exceptions;

namespace {
void CheckPixel(GuiUnit_t aX, GuiUnit_t aY, Color aColor, const Renderer& arFb)
{
    if (Rect(0, 0, arFb.GetWidth(), arFb.GetHeight()).IsHit(aX, aY)) {
        CHECK_HEX(arFb.GetPixel(aX, aY).ToARGB(), aColor.ToARGB());
    }
    else {
        CHECK_HEX(arFb.GetPixel(aX, aY).ToARGB(), 0);
    }
}
}

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Framebuffer")
{
    using namespace std::chrono;
    TestLogger logger;

    SignalHandler sig_handler;

    Random::Seed(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

#ifdef RSP_CORE_LIB_USE_GFX_SW
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName(
        "vfb2", std::filesystem::path{"/dev/fb?"});
    Renderer::SetDevicePath(p.string());
#endif

    auto& renderer = Renderer::Init(480, 800);

    Canvas canvas(renderer.GetWidth(), renderer.GetHeight());
    CHECK_NOTHROW(Texture::Create(renderer.GetWidth(), renderer.GetHeight()));
    auto tx = Texture::Create(renderer.GetWidth(), renderer.GetHeight());
    auto& texture = *tx;

    auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    Random::Seed(static_cast<Random::Engine::result_type>(ms.count())); // generates random seed val
    Color col(Random::Roll<uint8_t>(56u, 200u), Random::Roll<uint8_t>(56u, 200u), Random::Roll<uint8_t>(56u, 200u),
              0xff);
    MESSAGE("Color: " << TestHelpers::ToHex(col.ToARGB()));

    SUBCASE("Fill") {
        std::array<Color, 3> colors{Color::Red, Color::Blue, Color::Green};

        for (Color color : colors) {
            CHECK_NOTHROW(renderer.Fill(color));
            CHECK_NOTHROW(renderer.Flush());

            uint32_t fb_value = renderer.GetPixel(0, 0).ToARGB();
            CHECK_HEX(fb_value, color.ToARGB());
            fb_value = renderer.GetPixel(479, 799).ToARGB();
            CHECK_HEX(fb_value, color.ToARGB());
            renderer.Present();
            std::this_thread::sleep_for(500ms);
        }
    }

    SUBCASE("Clear") {
        renderer.Fill(Color::None);
        renderer.Present();
        renderer.Fill(Color::None);
        renderer.Present();
        canvas.Fill(Color::None);
        texture.Fill(Color::None);
    }

    SUBCASE("Drawing Lines") {
        // Arrange
        Point pointA(Random::Roll(0, renderer.GetWidth() - 1), Random::Roll(0, renderer.GetHeight() - 1));
        Point pointB(Random::Roll(0, renderer.GetWidth() - 1), Random::Roll(0, renderer.GetHeight() - 1));

        // Act
        CHECK_NOTHROW(canvas.DrawLine(pointA, pointB, col));
        CHECK_NOTHROW(texture.Update(canvas, col));
        CHECK_NOTHROW(renderer.Blit(texture));
        CHECK_NOTHROW(renderer.Flush());

        // Assert
        auto deltaX = static_cast<int>(pointB.GetX() - pointA.GetX());
        auto deltaY = static_cast<int>(pointB.GetY() - pointA.GetY());
        int absDeltaX = abs(deltaX);
        int absDeltaY = abs(deltaY);
        int signumX = (deltaX > 0) ? 1 : -1;
        int signumY = (deltaY > 0) ? 1 : -1;
        int x = absDeltaX >> 1;
        int y = absDeltaY >> 1;
        auto px = static_cast<int>(pointA.GetX());
        auto py = static_cast<int>(pointA.GetY());
        if (absDeltaX >= absDeltaY) {
            for (int i = 0 ; i < absDeltaX ; i++) {
                y += absDeltaY;
                if (y >= absDeltaX) {
                    y -= absDeltaX;
                    py += signumY;
                }
                px += signumX;
                CHECK_HEX(canvas.GetPixelAt(px, py).ToARGB(), col.ToARGB());
                CHECK_HEX(renderer.GetPixel(px, py).ToARGB(), col.ToARGB());
            }
        }
        else {
            for (int i = 0 ; i < absDeltaY ; i++) {
                x += absDeltaX;
                if (x >= absDeltaY) {
                    x -= absDeltaY;
                    px += signumX;
                }
                py += signumY;
                CHECK_HEX(canvas.GetPixelAt(px, py).ToARGB(), col.ToARGB());
                CHECK_HEX(renderer.GetPixel(px, py).ToARGB(), col.ToARGB());
            }
        }

        SUBCASE("Lines are Inclusive") {
            CHECK_HEX(renderer.GetPixel(pointA.GetX(), pointA.GetY()).ToARGB(), col.ToARGB());
            CHECK_HEX(renderer.GetPixel(pointB.GetX(), pointB.GetY()).ToARGB(), col.ToARGB());
        }

        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Drawing Rectangles") {
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
        CHECK_NOTHROW(renderer.Flush());

        // Assert
        // Expect all four side to hold values
        for (GuiUnit_t i = 0 ; i < rect.GetWidth() ; i++) {
            // Check top side
            CHECK_HEX(canvas.GetPixelAt(rect.GetLeft() + i, rect.GetTop()).ToARGB(), col.ToARGB());
            CHECK_HEX(renderer.GetPixel(leftTop.GetX() + i, leftTop.GetY()).ToARGB(), col.ToARGB());
            // Check bottom side
            CHECK_HEX(canvas.GetPixelAt(rect.GetLeft() + i, rect.GetBottom() - 1).ToARGB(), col.ToARGB());
            CHECK_HEX(renderer.GetPixel(leftTop.GetX() + i, rightBottom.GetY()-1).ToARGB(), col.ToARGB());
        }
        for (GuiUnit_t i = 0 ; i < rect.GetHeight() ; i++) {
            // Check left side
            CHECK_HEX(renderer.GetPixel(leftTop.GetX(), rightBottom.GetY()-1 - i).ToARGB(), col.ToARGB());
            // Check right side
            CHECK_HEX(renderer.GetPixel(rightBottom.GetX()-1, rightBottom.GetY()-1 - i).ToARGB(), col.ToARGB());
        }
        CHECK_NOTHROW(renderer.Present());

        Rect small(10, 10, 1, 1);
        Color white(Color::White);
        CHECK_NOTHROW(canvas.DrawRectangle(small, white));
        CHECK_NOTHROW(texture.Update(canvas, white));
        CHECK_NOTHROW(renderer.Blit(texture));
        CHECK_NOTHROW(renderer.Flush());
        CHECK_EQ(renderer.GetPixel(10, 10).ToARGB(), white.ToARGB());
        CHECK_NE(renderer.GetPixel(11, 10).ToARGB(), white.ToARGB());
        CHECK_NE(renderer.GetPixel(10, 11).ToARGB(), white.ToARGB());
        CHECK_NE(renderer.GetPixel(11, 11).ToARGB(), white.ToARGB());
        CHECK_NOTHROW(renderer.Present());

        Rect medium(20, 20, 10, 10);
        CHECK_NOTHROW(canvas.DrawRectangle(medium, white));
        CHECK_NOTHROW(texture.Update(canvas, white));
        CHECK_NOTHROW(renderer.Blit(texture));
        CHECK_NOTHROW(renderer.Flush());
        CHECK_EQ(renderer.GetPixel(20, 20).ToARGB(), white.ToARGB());
        CHECK_EQ(renderer.GetPixel(20, 29).ToARGB(), white.ToARGB());
        CHECK_NE(renderer.GetPixel(20, 30).ToARGB(), white.ToARGB());
        CHECK_EQ(renderer.GetPixel(29, 20).ToARGB(), white.ToARGB());
        CHECK_NE(renderer.GetPixel(30, 20).ToARGB(), white.ToARGB());
        CHECK_EQ(renderer.GetPixel(29, 29).ToARGB(), white.ToARGB());
        CHECK_NE(renderer.GetPixel(30, 30).ToARGB(), white.ToARGB());
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Drawing Circles") {
        // Arrange
        Point centerPoint(Random::Roll(0, renderer.GetWidth()), Random::Roll(0, renderer.GetHeight()));
        GuiUnit_t radius = Random::Roll(0, renderer.GetWidth() / 2);

        // Act
        CHECK_NOTHROW(canvas.DrawCircle(centerPoint, radius, col));
        CHECK_NOTHROW(texture.Update(canvas, col));
        CHECK_NOTHROW(renderer.Blit(texture));
        CHECK_NOTHROW(renderer.Flush());

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

    SUBCASE("Set/Get pixel outside screen") {
        // Arrange
        Point outSideXAxis(-1, 0);
        Point outSideYAxis(0, -1);

        // Act
        CHECK_NOTHROW(renderer.SetPixel(outSideXAxis.GetX(), outSideXAxis.GetY(), col));
        CHECK_NOTHROW(renderer.SetPixel(outSideYAxis.GetX(), outSideYAxis.GetY(), col));
        CHECK_NOTHROW(renderer.Flush());

        // Assert
        CHECK_HEX(renderer.GetPixel(outSideXAxis.GetX(), outSideXAxis.GetY()).ToARGB(), 0);
        CHECK_HEX(renderer.GetPixel(outSideYAxis.GetX(), outSideYAxis.GetY()).ToARGB(), 0);
    }

    SUBCASE("Drawing Images") {
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
        CHECK_EQ(testImgMap.GetPixelData().GetDataSize(), static_cast<size_t>(width * height * 3));
        CHECK_HEX(testImgMap.GetPixelAt(50, 50).ToARGB(), 0xFF031b95);

        SUBCASE("Draw image from file") {
            // Act
            auto raster = Texture::Create(testImgMap, Color::White);
            CHECK_NOTHROW(raster->SetDestination(topLeftImgCorner));
            CHECK_NOTHROW(renderer.Blit(*raster));
            CHECK_NOTHROW(renderer.Flush());

            // Assert
            CHECK_HEX(renderer.GetPixel(topLeftImgCorner.GetX() + 4, topLeftImgCorner.GetY() + 4).ToARGB(), 0xFF020F92);

            CHECK_NOTHROW(renderer.Present());
        }
        SUBCASE("Draw edited image file") {
            // Arrange
            auto offset = [](const Point& arPoint, const int aVal) { return (arPoint + Point(aVal, aVal)); };

            // Act
            for (uint8_t i = 0 ; i < 16 ; ++i) {
                uint8_t val = 0x77 + (i * 8);
                Color grey(val, val, val, 0xFF);
                CHECK_NOTHROW(testImgMap.DrawRectangle(Rect(offset(topLeft, i), width-(i*2), height-(i*2)), grey));
            }

            CHECK_NOTHROW(testImgMap.DrawLine(topLeft, botRight, Color::Red));
            CHECK_NOTHROW(testImgMap.DrawLine(botLeft, topRight, Color::Red));

            auto raster = Texture::Create(testImgMap, Color::White);
            CHECK_NOTHROW(raster->SetDestination(topLeftImgCorner));
            CHECK_NOTHROW(renderer.Blit(*raster));
            CHECK_NOTHROW(renderer.Flush());

            // Assert
            CHECK_HEX(testImgMap.GetPixelAt(width / 2, 0).ToARGB(), 0xFF777777);
            CHECK_HEX(testImgMap.GetPixelAt(0, height / 2).ToARGB(), 0xFF777777);
            CHECK_HEX(testImgMap.GetPixelAt(width - 12, height / 2).ToARGB(), 0xFFCFCFCF);
            CHECK_HEX(testImgMap.GetPixelAt(width / 2, height - 4).ToARGB(), 0xFF8F8F8F);

            CHECK_HEX(renderer.GetPixel(topLeftImgCorner.GetX() + width / 2 , topLeftImgCorner.GetY() + 0).ToARGB(),
                      0xFF777777);
            CHECK_HEX(renderer.GetPixel(topLeftImgCorner.GetX() + 0 , topLeftImgCorner.GetY() + height / 2).ToARGB(),
                      0xFF777777);
            CHECK_HEX(
                renderer.GetPixel(topLeftImgCorner.GetX() + width - 12, topLeftImgCorner.GetY() + height / 2).ToARGB(),
                0xFFCFCFCF);
            CHECK_HEX(
                renderer.GetPixel(topLeftImgCorner.GetX() + width / 2 , topLeftImgCorner.GetY() + height - 4).ToARGB(),
                0xFF8F8F8F);
            CHECK_NOTHROW(renderer.Present());
        }
        SUBCASE("Draw memory created image") {
            // Arrange
            Bitmap emptyMap(width, height, ColorDepth::RGBA);
            Point randomPoint(Random::Roll(0, width - 1), Random::Roll(0, height - 1));

            MESSAGE("randomPoint: " << randomPoint);
            MESSAGE("Combined: " << (topLeftImgCorner + randomPoint));

            // Act
            CHECK_NOTHROW(emptyMap.DrawRectangle(Rect(0, 0, width, height), col, true));
            CHECK_NOTHROW(canvas.DrawPixelData(topLeftImgCorner, emptyMap));
            CHECK_NOTHROW(canvas.SetPixel(topLeftImgCorner + randomPoint, Color::White));
            CHECK_NOTHROW(texture.Update(canvas, Color::White));
            CHECK_NOTHROW(renderer.Blit(texture));
            CHECK_NOTHROW(renderer.Flush());

            // Assert
            CHECK_HEX(emptyMap.GetPixel({0,0}).ToARGB(), col.ToARGB());
            CHECK_HEX(emptyMap.GetPixel({1,1}).ToARGB(), col.ToARGB());

            CHECK_HEX(canvas.GetPixel(topLeftImgCorner).ToARGB(), col.ToARGB());
            CHECK_HEX(canvas.GetPixel(topLeftImgCorner + Point(width-1, height-1)).ToARGB(), col.ToARGB());
            CHECK_HEX(canvas.GetPixel(topLeftImgCorner + randomPoint).ToARGB(), Color::White);

            CHECK_HEX(renderer.GetPixel(topLeftImgCorner).ToARGB(), col.ToARGB());
            CHECK_HEX(renderer.GetPixel(topLeftImgCorner + Point(width-1, height-1)).ToARGB(), col.ToARGB());
            CHECK_HEX(renderer.GetPixel(topLeftImgCorner + randomPoint).ToARGB(), Color::White);
            CHECK_NOTHROW(renderer.Present());
        }
    }

    SUBCASE("Drawing image larger than screen") {
        // Arrange
        Point topLeft(0, 0);
        Point randomPoint(Random::Roll(0, renderer.GetWidth() - 1), Random::Roll(0, renderer.GetHeight() - 1));
        std::string largeImg = "testImages/largeTestImg.bmp";
        Bitmap largeImgMap(largeImg);
        // Make sure screen is empty
        CHECK_NOTHROW(renderer.Present());

        // Act
        CHECK_NOTHROW(texture.Update(largeImgMap, Color::White));
        CHECK_NOTHROW(renderer.Blit(texture));
        CHECK_NOTHROW(renderer.Flush());

        // Assert
        CHECK_GT(largeImgMap.GetHeight(), renderer.GetHeight());
        CHECK_GT(largeImgMap.GetWidth(), renderer.GetWidth());
        CHECK_NE(largeImgMap.GetPixel(randomPoint).ToARGB(), 0u);
        CHECK_NE(renderer.GetPixel(randomPoint).ToARGB(), 0u);
        CHECK_NOTHROW(renderer.Present());

        SUBCASE("Pan screen over large image") {
            TexturePtr_t large_texture = Texture::Create(largeImgMap);

            int x = 0;
            int y = 0;
            int y_delta = 4;
            int x_delta = 4;

            for (int i = 0 ; i < 500 ; ++i) {
                large_texture->SetSourceRect({x, y, 480, 800});

                CHECK_NOTHROW(renderer.Fill(Color::Black));
                CHECK_NOTHROW(renderer.Blit(*large_texture));
                CHECK_NOTHROW(renderer.Present());
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

                x += x_delta;
                y += y_delta;
                if (x >= (large_texture->GetWidth() - 280)) {
                    x_delta = -4;
                }
                else if (x <= -200) {
                    x_delta = 4;
                }
                if (y >= large_texture->GetHeight() - 200) {
                    y_delta = -4;
                }
                else if (y <= -200) {
                    y_delta = 4;
                }
            }

            // Update back buffer with visible content, so we can check result
            CHECK_NOTHROW(renderer.Fill(Color::Black));
            CHECK_NOTHROW(renderer.Blit(*large_texture));
            CHECK_NOTHROW(renderer.Flush());

            // Assert
            CHECK_HEX(renderer.GetPixel(43, 171).ToARGB(), 0xFF000000);
            CHECK_HEX(renderer.GetPixel(44, 172).ToARGB(), 0xFF121C2D);
            CHECK_HEX(renderer.GetPixel(79, 201).ToARGB(), 0xFF3583C5);
        }
    }

    SUBCASE("Moving image") {
        // Arrange
        Bitmap imgSimple("testImages/testImage.bmp");
        int iterations = 700;

        auto sprite = Texture::Create(imgSimple, Color::Black);
        Point pos(460, 810);

        // Act
        rsp::utils::StopWatch sw;
        for (int i = 0 ; i < iterations ; i += 4) {
            CHECK_NOTHROW(sprite->SetDestination(pos));
            CHECK_NOTHROW(renderer.Fill(Color::Black));
            CHECK_NOTHROW(renderer.Blit(*sprite));
            CHECK_NOTHROW(renderer.Present());
            pos.SetY(800 - int(i * 1.5));
            pos.SetX(500 - i);
        }

        for (int i = 0 ; i < iterations ; i += 4) {
            pos.SetY(int((i - 200) * 1.5));
            pos.SetX(470 - i);
            CHECK_NOTHROW(sprite->SetDestination(pos));
            CHECK_NOTHROW(renderer.Fill(Color::Black));
            CHECK_NOTHROW(renderer.Blit(*sprite));
            CHECK_NOTHROW(renderer.Present());
        }

        auto fps = static_cast<int>((1000 * iterations) / (sw.Elapsed<std::chrono::milliseconds>() + 1));

        // Assert
        CHECK_GT(fps, 10);
        MESSAGE("Fps: " << fps);
    }

    SUBCASE("Moving monochrome image") {
        // Arrange
        Bitmap imgSimple("testImages/monochrome/Monochrome.bmp");
        size_t iterations = 100;
        std::array<Color, 5> mcl{
            Color::White,
            Color::Blue,
            Color::Red,
            Color::Green,
            Color::Yellow
        };

        auto sprite = Texture::Create(imgSimple.GetWidth(), imgSimple.GetHeight() + 5);
        CHECK_NOTHROW(sprite->Fill(Color::Black));
        CHECK_NOTHROW(sprite->SetBlendOperation(Texture::BlendOperation::SourceAlpha));
        Point pos(100, 200);

        // Act
        rsp::utils::StopWatch sw;
        for (size_t i = 0 ; i < iterations ; i++) {
            if ((i % 20) == 0) {
                CHECK_NOTHROW(sprite->Update(imgSimple, mcl[(i / 20) % 5]));
            }
            CHECK_NOTHROW(sprite->SetDestination(pos));
            CHECK_NOTHROW(renderer.Fill(Color::Black));
            CHECK_NOTHROW(renderer.Blit(*sprite));
            CHECK_NOTHROW(renderer.Present());
            pos.SetY(static_cast<GuiUnit_t>(200u - i));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        auto fps = static_cast<int>(1000 * iterations) / (sw.Elapsed<std::chrono::milliseconds>() + 1);

        // Assert
        CHECK_GT(fps, 10);
        MESSAGE("Fps: " << fps);
    }

    SUBCASE("Draw Text") {
        const char* cFontFile = "fonts/Exo 2/Exo2-VariableFont_wght.ttf";
        Font::RegisterFont(cFontFile);
        Rect r(100, 200, 280, 200);

        Text text("Exo 2", "Hello World");
        CHECK_NOTHROW(text.GetFont().SetSize(30));

        auto panel = Texture::Create(280, 200);
        CHECK_NOTHROW(panel->SetBlendOperation(Texture::BlendOperation::ColorKey));

        CHECK_NOTHROW(renderer.Fill(Color::Black));

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
            const std::array<Color, 8> rainbow{
                Color::White, Color::Red, Color::Yellow, Color::Green, Color::Aqua, Color::Lime, Color::Blue,
                Color::Silver
            };

            MESSAGE("FPS Test");
            CHECK_NOTHROW(text.SetLineSpacing(50).SetValue("FPS:\n0000").Reload(r));
            panel = Texture::Create(text, Color::Black);
            CHECK_NOTHROW(
                panel->SetBlendOperation(Texture::BlendOperation::ColorKey, Color::None).SetDestination(Point(100, 200)
                ));

            rsp::utils::StopWatch sw;
            for (int i = 0 ; i < 300 ; i++) {
                auto fps = static_cast<int>((1000 * i) / (sw.Elapsed<std::chrono::milliseconds>() + 1));
                std::stringstream ss;
                ss << "FPS:\n" << fps;
                CHECK_NOTHROW(text.SetValue(ss.str()).Reload());
                CHECK_NOTHROW(panel->Fill(Color::None));
                CHECK_NOTHROW(panel->Update(text, Color::Black));
                CHECK_NOTHROW(renderer.Fill(rainbow[i & 0x07]));
                CHECK_NOTHROW(renderer.Blit(*panel));
                CHECK_NOTHROW(renderer.Present());
            }
            MESSAGE(text.GetValue());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    SUBCASE("Text Alignment") {
        auto cFontFile = "fonts/Exo 2/Exo2-VariableFont_wght.ttf";
        Font::RegisterFont(cFontFile);
        Rect r(10, 10, 460, 780);

        Text text("Exo 2", "Hello\nUniverse");
        CHECK_NOTHROW(text.GetFont().SetSize(50));
        CHECK_NOTHROW(text.Reload());

        constexpr std::array<Text::VAlign, 3> cVertical{Text::VAlign::Top, Text::VAlign::Center, Text::VAlign::Bottom};
        constexpr std::array<Text::HAlign, 3> cHorizontal{Text::HAlign::Left, Text::HAlign::Center, Text::HAlign::Right};

        for (auto& h : cHorizontal) {
            CHECK_NOTHROW(text.SetHAlignment(h));
            CHECK_NOTHROW(text.Reload());
            auto panel = Texture::Create(text.GetWidth(), text.GetHeight());
            CHECK_NOTHROW(panel->SetBlendOperation(Texture::BlendOperation::ColorKey, Color::None));
            CHECK_NOTHROW(panel->Fill(Color::None).Update(text, Color::Yellow));
            for (auto& v : cVertical) {
                CHECK_NOTHROW(text.SetVAlignment(v));
                CHECK_NOTHROW(panel->SetDestination(text.GetPosition(r)));

                CHECK_NOTHROW(renderer.Fill(Color::Black));
                CHECK_NOTHROW(renderer.Blit(*panel));
                CHECK_NOTHROW(renderer.DrawRect(Color::White, r));
                CHECK_NOTHROW(renderer.Present());
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    }

    SUBCASE("Font Styles") {
        CHECK_NOTHROW(Font::RegisterFont("fonts/Exo 2/Exo2-Italic-VariableFont_wght.ttf"));
        CHECK_NOTHROW(Font::RegisterFont("fonts/Exo 2/Exo2-VariableFont_wght.ttf"));
        Rect r(10, 10, 460, 280);
        auto r2 = r;
        r2.AddSize(2, 2).Move(-1, -1);

        auto panel = Texture::Create(r.GetWidth(), r.GetHeight());
        CHECK_NOTHROW(panel->SetBlendOperation(Texture::BlendOperation::ColorKey, Color::None));

        Text text("Exo 2", "Regular");
        CHECK_NOTHROW(text.GetFont().SetSize(50));

        SUBCASE("Normal") {
            // No extra styles to set here
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
        CHECK_NOTHROW(panel->Fill(Color::None));
        CHECK_NOTHROW(panel->Update(text, Color::Yellow));
        CHECK_NOTHROW(panel->SetDestination(text.GetPosition(r)));
        CHECK_NOTHROW(renderer.Fill(Color::Black));
        CHECK_NOTHROW(renderer.DrawRect(Color::Green, r2));
        CHECK_NOTHROW(renderer.Blit(*panel));
        CHECK_NOTHROW(renderer.Present());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    SUBCASE("Swap Buffer") {
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Draw Fullscreen") {
        std::string testImage = "testImages/rgba/Asset2WithAlpha.bmp";
        Bitmap testImgMap(testImage);

        texture.SetBlendOperation(Texture::BlendOperation::Copy);

        CHECK_NOTHROW(texture.Update(testImgMap, Color::White));
        CHECK_NOTHROW(renderer.Blit(texture));
        CHECK_NOTHROW(renderer.Present());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    SUBCASE("Draw Transparent") {
        Color blue(Color::Blue);
        Color green(Color::Green);
        Color red(Color::Red);

        for (uint8_t a = 0 ; a < 250 ; a += 5) {
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
        CHECK_NOTHROW(renderer.Blit(texture));
        CHECK_NOTHROW(renderer.Flush());
        CHECK_HEX(renderer.GetPixel(Point(180, 180)).ToARGB(), 0xFF007A0A);
    }
}

TEST_SUITE_END();
