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

using namespace rsp::graphics;

inline void CheckPixel(const Point &aPoint, const Color &aColour, const Framebuffer &fb)
{
    if (fb.IsInsideScreen(aPoint)) {
        CHECK(fb.GetPixel(aPoint) == aColour);
    } else {
        CHECK(fb.GetPixel(aPoint) == 0);
    }
}

TEST_CASE("Framebuffer Drawing Primitives")
{
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});

    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    srand(ms.count()); // generates random seed val
    Color col(rand() % 200 + 56, rand() % 200 + 56, rand() % 200 + 56, 0xff);

    SUBCASE("Clear Framebuffer")
    {
        fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
        fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
    }

    SUBCASE("Drawing Lines")
    {
        // Arrange
        Point pointA(rand() % fb.GetWidth(), rand() % fb.GetHeight());
        Point pointB(rand() % fb.GetWidth(), rand() % fb.GetHeight());

        // Act
        fb.DrawLine(pointA, pointB, col);

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
        // fb.SwapBuffer();
    }

    SUBCASE("Drawing Rectangles")
    {
        // Arrange
        // Generate random values in the LEFT and TOP halves of the screen
        Point leftTop(rand() % (fb.GetWidth() / 2),
                      rand() % (fb.GetHeight() / 2));
        // Generate random values in the RIGHT and BOTTOM halves of the screen
        Point rightBottom(rand() % (fb.GetWidth() + 1 - (fb.GetWidth() / 2)) + (fb.GetWidth() / 2),
                          rand() % (fb.GetHeight() + 1 - (fb.GetHeight() / 2)) + (fb.GetHeight() / 2));
        Rect rect(leftTop, rightBottom);

        // Act
        fb.DrawRectangle(rect, col);

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
        // fb.SwapBuffer();
    }

    SUBCASE("Drawing Circles")
    {
        // Arrange
        Point centerPoint(rand() % fb.GetWidth(), rand() % fb.GetHeight());
        int radius = rand() % (fb.GetWidth() / 2);

        // Act
        fb.DrawCircle(centerPoint, radius, col);

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
        // fb.SwapBuffer();
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
        uint32_t height = testImgMap.GetHeight();
        uint32_t width = testImgMap.GetWidth();
        Point topLeft(0, 0);
        Point topRight(width - 1, 0);
        Point botLeft(0, height - 1);
        Point botRight(width - 1, height - 1);

        // Assert
        CHECK(testImgMap.IsInsideScreen(topLeft));
        CHECK(testImgMap.IsInsideScreen(topRight));
        CHECK(testImgMap.IsInsideScreen(botLeft));
        CHECK(testImgMap.IsInsideScreen(botRight));

        SUBCASE("Draw image from file")
        {
            // Act
            fb.DrawImage(topLeftImgCorner, testImgMap);

            // Assert
            CHECK(testImgMap.GetHeight() == height);
            CHECK(testImgMap.GetWidth() == width);
            CHECK(testImgMap.GetPixels().size() == (width * height));

            fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
        }
        SUBCASE("Draw edited image file")
        {
            // Arrange
            Color red(0xFF0000);
            Color green(0x00FF00);
            Color blue(0x0000FF);
            Color thisColor(0x777777);

            // Act
            testImgMap.DrawLine(topLeft, topRight, red);
            testImgMap.DrawLine(topLeft, botLeft, green);
            testImgMap.DrawLine(topRight, botRight, blue);
            testImgMap.DrawLine(botLeft, botRight, thisColor);

            fb.DrawImage(topLeftImgCorner, testImgMap);
            fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);

            // Assert
            CHECK(testImgMap.GetPixel(Point(width / 2, 0)) == red);
            CHECK(testImgMap.GetPixel(Point(0, height / 2)) == green);
            CHECK(testImgMap.GetPixel(Point(width - 1, height / 2)) == blue);
            CHECK(testImgMap.GetPixel(Point(width / 2, height - 1)) == thisColor);
        }
        SUBCASE("Draw memory created image")
        {
            // Arrange
            Bitmap emptyMap(height, width, 4);
            Point randomPoint(rand() % emptyMap.GetWidth(), rand() % emptyMap.GetHeight());

            // Act
            emptyMap.SetPixel(randomPoint, col);
            fb.DrawImage(topLeftImgCorner, emptyMap);
            fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);

            // Assert
            CHECK(emptyMap.GetPixel(randomPoint) == col);
        }
    }

    SUBCASE("Swapping between two images")
    {
        // Arrange
        Point topLeftPoint(100, 200);
        Bitmap imgSimple("testImages/testImage.bmp");
        int iterations = 100;

        // Act
        auto begin = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; i++) {
            fb.DrawImage(Point(topLeftPoint.GetX(), topLeftPoint.GetY() - i), imgSimple);
            fb.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        int fps = (1000 * iterations / duration);

        // Assert
        CHECK(fps > 10);
        MESSAGE("Fps: " << fps);
    }

    SUBCASE("Draw text")
    {
        const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
        Font font(cFontFile);
        Rect r(100, 200, 280, 200);
//        font.SetSize(82);

        fb.DrawRectangle(r, Color(0xFFFFFFFF));
        fb.DrawText1(r, font, "Hello World", true);
        fb.SwapBuffer();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    SUBCASE("Swapbuffer")
    {
        fb.SwapBuffer();
    }
}
