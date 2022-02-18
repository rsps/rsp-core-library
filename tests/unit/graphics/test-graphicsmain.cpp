/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/Framebuffer.h"
#include "graphics/GraphicsMain.h"
#include <doctest.h>
#include <functional>
#include <vector>

using namespace rsp::graphics;

class firstScene : public Scene
{
public:
    firstScene(const Rect &aRect)
        : Scene(aRect)
    {
        // Set member variables values
        Rect topRect(Point(100, 400), 200, 100);
        mTopBtn = TouchArea(topRect);
        mTopBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", topRect);

        Rect botRect(Point(100, 600), 200, 100);
        mBotBtn = TouchArea(botRect);
        mBotBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", botRect);

        // Bind onPressed
        mTopBtn.RegisterOnPressed(std::bind(&Image::HandleCallback, &mTopBtnImg, std::placeholders::_1));
        mBotBtn.RegisterOnPressed(std::bind(&Image::HandleCallback, &mBotBtnImg, std::placeholders::_1));

        //  Bind onClicked

        //  Add them to the lists?
        mTouchables.push_back(&mTopBtn);
        mTouchables.push_back(&mBotBtn);
        mChildren.push_back(&mTopBtnImg);
        mChildren.push_back(&mBotBtnImg);
    };

    ~firstScene() {}

protected:
    Color background{0x323232}; // color from old main.qml

    TouchArea mTopBtn{};
    TouchArea mBotBtn{};

    Image mTopBtnImg{};
    Image mBotBtnImg{};
};

class secondScene : public Scene
{
public:
    secondScene(const Rect &aRect)
        : Scene(aRect)
    {
        // Set member variables values
        Rect topRect(Point(100, 100), 200, 100);
        mTopBtn = TouchArea(topRect);
        mTopBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", topRect);

        Rect botRect(Point(100, 300), 200, 100);
        mBotBtn = TouchArea(botRect);
        mBotBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", botRect);

        // Bind onPressed
        mTopBtn.RegisterOnPressed(std::bind(&Image::HandleCallback, &mTopBtnImg, std::placeholders::_1));
        mBotBtn.RegisterOnPressed(std::bind(&Image::HandleCallback, &mBotBtnImg, std::placeholders::_1));

        //  Bind onClicked

        //  Add them to the lists?
        mTouchables.push_back(&mTopBtn);
        mTouchables.push_back(&mBotBtn);
        mChildren.push_back(&mTopBtnImg);
        mChildren.push_back(&mBotBtnImg);
    };

    ~secondScene() {}

protected:
    Color mBackground{0x232323}; // reverse color from old main.qml

    TouchArea mTopBtn{};
    TouchArea mBotBtn{};

    Image mTopBtnImg{};
    Image mBotBtnImg{};
};

TEST_CASE("Graphics Main Test")
{
    /*
    MESSAGE("Start");

    MESSAGE("Init Framebufer");
    // Make framebuffer
    Framebuffer fb;
    Rect screenSize(Point(0, 0), Point(fb.GetWidth(), fb.GetHeight()));

    MESSAGE("Making Scene");
    // Make scene
    firstScene myScene(screenSize);
    secondScene otherScene(screenSize);

    MESSAGE("Init InputCreator");
    // Make InputCreator
    InputCreator ic;

    MESSAGE("Init Main");
    // Make GraphicsMain
    GraphicsMain gMain(fb, ic, myScene, otherScene);

    // Bind onClick
    myScene.topBtn.RegisterOnClicked(std::bind(&GraphicsMain::ChangeScene, &gMain));
    otherScene.botBtn.RegisterOnClicked(std::bind(&GraphicsMain::ChangeScene, &gMain));

    // Run?
    gMain.Run();
    */
}
