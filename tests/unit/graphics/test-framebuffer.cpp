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

#include "FramebufferCanvas.h"

TEST_CASE("Framebuffer Drawing Primitives") {
    srand(time(NULL));  //generates random seed val
    Framebuffer fb;
    fb.Clear();
    SUBCASE("Drawing Lines") {
        for (size_t i = 0; i < 10; i++) {
            Point pointA(rand() % fb.vinfo.xres, rand() % fb.vinfo.yres);
            Point pointB(rand() % fb.vinfo.xres, rand() % fb.vinfo.yres);
            Colour col(0, rand() % 256, rand() % 256, rand() % 256);
            Pen pen(rand() % 10 + 1, col);

            fb.DrawLine(pointA, pointB, pen);
            //CHECK(col == fb.GetPixel(pointA, false));
            //CHECK(col == fb.GetPixel(pointB, false));
        }
        //fb.SwapBuffer();
    }
    SUBCASE("Drawing Rectangles") {
        SUBCASE("Giving Points correctly") {
            //Generate random values in the LEFT and TOP halves of the screen
            Point leftTop(rand() % (fb.vinfo.xres / 2),
                          rand() % (fb.vinfo.yres / 2));
            //Generate random values in the RIGHT and BOTTOM halves of the screen
            Point rightBottom(rand() % (fb.vinfo.xres + 1 - (fb.vinfo.xres / 2)) + (fb.vinfo.xres / 2),
                              rand() % (fb.vinfo.yres + 1 - (fb.vinfo.yres / 2)) + (fb.vinfo.yres / 2));

            Colour col(0, rand() % 256, rand() % 256, rand() % 256);
            Rect rect(leftTop, rightBottom);
            Pen pen(rand() % 10 + 1, col);

            //Act
            fb.DrawRectangle(rect, pen);

            //Expect all four side to hold values
            for (size_t i = 0; i <= rect.GetWidth(); i++) {
                //Check top side
                CHECK(col == fb.GetPixel(Point(leftTop.x + i, leftTop.y), false));
                //Check bottom side
                CHECK(col == fb.GetPixel(Point(leftTop.x + i, rightBottom.y), false));
            }

            for (size_t i = 0; i <= rect.GetHeight(); i++) {
                //Check left side
                CHECK(col == fb.GetPixel(Point(leftTop.x, rightBottom.y - i), false));
                //Check right side
                CHECK(col == fb.GetPixel(Point(rightBottom.x, rightBottom.y - i), false));
            }
            //fb.SwapBuffer();
        }
        /*SUBCASE("Points flipped on x-axis") {
            //Generate random values in the RIGHT and TOP halves of the screen
            Point rightTop(rand() % (fb.vinfo.xres + 1 - (fb.vinfo.xres / 2)) + (fb.vinfo.xres / 2),
                           rand() % (fb.vinfo.yres / 2));
            //Generate random values in the LEFT and BOTTOM halves of the screen
            Point leftBottom(rand() % (fb.vinfo.xres / 2),
                             rand() % (fb.vinfo.yres + 1 - (fb.vinfo.yres / 2)) + (fb.vinfo.yres / 2));

            Colour col(0, rand() % 256, rand() % 256, rand() % 256);
            Rect rect(rightTop, leftBottom);
            Pen pen(rand() % 10 + 1, col);

            //Act
            fb.DrawRectangle(rect, pen);

            std::cout << "Width " << rect.GetWidth() << std::endl;
            //Expect only left and right sides to hold values
            for (size_t i = -1; i < rect.GetWidth(); i--) {
                //Not inclusive, since corners are drawn as part of the left and right sides
                //Check top side
                CHECK(col != fb.GetPixel(Point(leftBottom.x + i, rightTop.y), false));
                //Check bottom side
                CHECK(col != fb.GetPixel(Point(leftBottom.x + i, leftBottom.y), false));
            }

            for (size_t i = 0; i <= rect.GetHeight(); i++) {
                //Check left side
                CHECK(col == fb.GetPixel(Point(leftBottom.x, leftBottom.y - i), false));
                //Check right side
                CHECK(col == fb.GetPixel(Point(rightTop.x, leftBottom.y - i), false));
            }
            //Expect rightTop and leftBottom corners to still hold values
            /*CHECK(col == fb.GetPixel(rightTop, false));
            CHECK(col == fb.GetPixel(leftBottom, false));
            //Expect topLeft and bottomRight to not hold the correct values
            CHECK(col != fb.GetPixel(Point(rightTop.x - rect.GetWidth(), rightTop.y), false));
            CHECK(col != fb.GetPixel(Point(leftBottom.x + rect.GetWidth(), leftBottom.y), false));*/
        //fb.SwapBuffer();
        //}
        /*SUBCASE("Points flipped on y-axis") {
            for (size_t i = 0; i < 10; i++) {
                //Generate random values in the LEFT and BOTTOM halves of the screen
                Point leftBottom(rand() % (fb.vinfo.xres / 2),
                                 rand() % (fb.vinfo.yres + 1 - (fb.vinfo.yres / 2)) + (fb.vinfo.yres / 2));
                //Generate random values in the RIGHT and TOP halves of the screen
                Point rightTop(rand() % (fb.vinfo.xres + 1 - (fb.vinfo.xres / 2)) + (fb.vinfo.xres / 2),
                               rand() % (fb.vinfo.yres / 2));

                Colour col(0, rand() % 256, rand() % 256, rand() % 256);
                Rect rect(leftBottom, rightTop);
                Pen pen(rand() % 10 + 1, col);

                fb.DrawRectangle(rect, pen);

                //Expect leftBottom and rightTop corners to not hold the correct values
                CHECK(col == fb.GetPixel(leftBottom, false));
                CHECK(col == fb.GetPixel(rightTop, false));
                //Expect topLeft and bottomRight to hold values
                CHECK(col == fb.GetPixel(Point(leftBottom.x + rect.GetWidth(), leftBottom.y), false));
                CHECK(col == fb.GetPixel(Point(rightTop.x - rect.GetWidth(), rightTop.y), false));
            }
            fb.SwapBuffer();
        }*/
        /*SUBCASE("Negative width") {
        }
        SUBCASE("Negative height") {
        }*/
    }
}