/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <doctest.h>
#include <graphics/FramebufferCanvas.h>

#include <chrono>
#include <thread>

static Framebuffer fb;

inline void CheckPixel(const Point &aPoint, const Colour &aColour, const Framebuffer &fb)
{
    if (fb.IsInsideScreen(aPoint)) {
        CHECK(fb.GetPixel(aPoint) == aColour);
    } else {
        CHECK(fb.GetPixel(aPoint) == 0);
    }
}

TEST_CASE("Framebuffer Drawing Primitives")
{
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    srand(ms.count()); //generates random seed val
    Colour col(rand() % 200 + 56, rand() % 200 + 56, rand() % 200 + 56, 0xff);
    Pen pen(rand() % 10 + 1, col);

    SUBCASE("Clear Framebuffer")
    {
        fb.SwapBuffer(Canvas::SwapOperations::Clear);
        fb.SwapBuffer(Canvas::SwapOperations::Clear);
    }

    SUBCASE("Drawing Lines")
    {
        //Arrange
        Point pointA(rand() % fb.vinfo.xres, rand() % fb.vinfo.yres);
        Point pointB(rand() % fb.vinfo.xres, rand() % fb.vinfo.yres);

        //Act
        fb.DrawLine(pointA, pointB, pen);

        //Assert
        int i, x, y, deltaX, deltaY, absDeltaX, absDeltaY, signumX, signumY, px, py;

        deltaX = pointB.mX - pointA.mX;
        deltaY = pointB.mY - pointA.mY;
        absDeltaX = abs(deltaX);
        absDeltaY = abs(deltaY);
        signumX = (deltaX > 0) ? 1 : -1;
        signumY = (deltaY > 0) ? 1 : -1;
        x = absDeltaX >> 1;
        y = absDeltaY >> 1;
        px = pointA.mX;
        py = pointA.mY;
        if (absDeltaX >= absDeltaY) {
            for (i = 0; i < absDeltaX; i++) {
                y += absDeltaY;
                if (y >= absDeltaX) {
                    y -= absDeltaX;
                    py += signumY;
                }
                px += signumX;
                CHECK(fb.GetPixel(Point(px, py), false) == col);
            }
        } else {
            for (i = 0; i < absDeltaY; i++) {
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
        //fb.SwapBuffer();
    }

    SUBCASE("Drawing Rectangles")
    {
        //Arrange
        //Generate random values in the LEFT and TOP halves of the screen
        Point leftTop(rand() % (fb.vinfo.xres / 2),
                      rand() % (fb.vinfo.yres / 2));
        //Generate random values in the RIGHT and BOTTOM halves of the screen
        Point rightBottom(rand() % (fb.vinfo.xres + 1 - (fb.vinfo.xres / 2)) + (fb.vinfo.xres / 2),
                          rand() % (fb.vinfo.yres + 1 - (fb.vinfo.yres / 2)) + (fb.vinfo.yres / 2));
        Rect rect(leftTop, rightBottom);

        //Act
        fb.DrawRectangle(rect, pen);

        //Assert
        //Expect all four side to hold values
        for (size_t i = 0; i <= rect.GetWidth(); i++) {
            //Check top side
            CHECK(pen.mColour == fb.GetPixel(Point(leftTop.mX + i, leftTop.mY), false));
            //Check bottom side
            CHECK(pen.mColour == fb.GetPixel(Point(leftTop.mX + i, rightBottom.mY), false));
        }
        for (size_t i = 0; i <= rect.GetHeight(); i++) {
            //Check left side
            CHECK(pen.mColour == fb.GetPixel(Point(leftTop.mX, rightBottom.mY - i), false));
            //Check right side
            CHECK(pen.mColour == fb.GetPixel(Point(rightBottom.mX, rightBottom.mY - i), false));
        }
        //fb.SwapBuffer();
    }

    SUBCASE("Drawing Circles")
    {
        //Arrange
        Point centerPoint(rand() % fb.vinfo.xres, rand() % fb.vinfo.yres);
        int radius = rand() % (fb.vinfo.xres / 2);

        //Act
        fb.DrawCircle(centerPoint, radius, pen);

        //Assert
        int error = -radius;
        int y = 0;
        while (radius >= y) {
            CheckPixel(Point(centerPoint.mX + radius, centerPoint.mY + y), col, fb);
            CheckPixel(Point(centerPoint.mX - radius, centerPoint.mY + y), col, fb);
            CheckPixel(Point(centerPoint.mX + radius, centerPoint.mY - y), col, fb);
            CheckPixel(Point(centerPoint.mX - radius, centerPoint.mY - y), col, fb);
            CheckPixel(Point(centerPoint.mX + y, centerPoint.mY + radius), col, fb);
            CheckPixel(Point(centerPoint.mX - y, centerPoint.mY + radius), col, fb);
            CheckPixel(Point(centerPoint.mX + y, centerPoint.mY - radius), col, fb);
            CheckPixel(Point(centerPoint.mX - y, centerPoint.mY - radius), col, fb);
            error += y;
            y++;
            error += y;

            if (error >= 0) {
                error += -radius;
                radius--;
                error += -radius;
            }
        }
        //fb.SwapBuffer();
    }

    SUBCASE("Set/Get pixel outside screen")
    {
        //Arrange
        Point outSideXAxis(-1, 0);
        Point outSideYAxis(0, -1);

        //Act
        CHECK_NOTHROW(fb.DrawDot(outSideXAxis, pen));
        CHECK_NOTHROW(fb.DrawDot(outSideYAxis, pen));

        //Assert
        CHECK_EQ(fb.GetPixel(outSideXAxis), 0);
        CHECK_EQ(fb.GetPixel(outSideYAxis), 0);
    }

    SUBCASE("Drawing an Image")
    {
        //Arrange
        Point topLeftPoint(100, 200);

        std::string testImage = "testImages/testImageCross.bmp";
        Bitmap testImgMap(testImage);

        std::string screenImg = "testImages/Asset2NoAlpha.bmp";
        Bitmap noAlphaMap(screenImg);

        //Act
        //fb.DrawImage(topLeftPoint, testImgMap);

        //fb.SwapBuffer(Canvas::SwapOperations::Clear);

        fb.DrawImage(Point(0, 0), noAlphaMap);

        fb.SwapBuffer(Canvas::SwapOperations::Clear);

        //10s
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));

        //Assert
    }

    SUBCASE("Swapping between two images")
    {
        //Arrange
        Point topLeftPoint(100, 200);
        Bitmap imgSimple("testImages/testImage.bmp");
        int iterations = 100;

        //Act
        auto begin = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; i++) {
            fb.DrawImage(Point(topLeftPoint.mX, topLeftPoint.mY - i), imgSimple);
            fb.SwapBuffer(Canvas::SwapOperations::Clear);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        int fps = (1000 * iterations / duration);

        //Assert
        CHECK(fps > 10);
        MESSAGE("Fps: " << fps);
    }

    SUBCASE("Swapbuffer")
    {
        fb.SwapBuffer();
    }
}
