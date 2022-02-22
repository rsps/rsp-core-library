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

/*class firstScene : public Scene
{
  public:
    firstScene(Rect &aRect)
        : Scene(aRect)
    {
        // Set member variables values
        Rect topRect(Point(100, 400), 200, 100);
        topBtn = TouchArea(topRect);
        topBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", topRect);

        Rect botRect(Point(100, 600), 200, 100);
        botBtn = TouchArea(botRect);
        botBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", botRect);

        // Bind onPressed
        topBtn.RegisterOnPressed(std::bind(&Image::HandleCallback, &topBtnImg, std::placeholders::_1));
        botBtn.RegisterOnPressed(std::bind(&Image::HandleCallback, &botBtnImg, std::placeholders::_1));

        //  Bind onClicked

        //  Add them to the lists?
        mTouchables.push_back(&topBtn);
        mTouchables.push_back(&botBtn);
        mChildren.push_back(&topBtnImg);
        mChildren.push_back(&botBtnImg);
    };

    ~firstScene() {}

    Color background{0x323232}; // color from old main.qml

    TouchArea topBtn;
    TouchArea botBtn;

    Image topBtnImg;
    Image botBtnImg;
};
class secondScene : public Scene
{
  public:
    secondScene(Rect &aRect)
        : Scene(aRect)
    {
        // Set member variables values
        Rect topRect(Point(100, 100), 200, 100);
        topBtn = TouchArea(topRect);
        topBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", topRect);

        Rect botRect(Point(100, 300), 200, 100);
        botBtn = TouchArea(botRect);
        botBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", botRect);

        // Bind onPressed
        topBtn.RegisterOnPressed(std::bind(&Image::HandleCallback, &topBtnImg, std::placeholders::_1));
        botBtn.RegisterOnPressed(std::bind(&Image::HandleCallback, &botBtnImg, std::placeholders::_1));

        //  Bind onClicked

        //  Add them to the lists?
        mTouchables.push_back(&topBtn);
        mTouchables.push_back(&botBtn);
        mChildren.push_back(&topBtnImg);
        mChildren.push_back(&botBtnImg);
    };

    ~secondScene() {}

    Color background{0x232323}; // reverse color from old main.qml

    TouchArea topBtn;
    TouchArea botBtn;

    Image topBtnImg;
    Image botBtnImg;
};*/

TEST_CASE("Graphics Main Test")
{

    MESSAGE("Start");

    MESSAGE("Init Framebufer");
    // Make framebuffer
    Framebuffer fb;
    Rect screenSize(Point(0, 0), Point(fb.GetWidth(), fb.GetHeight()));

    MESSAGE("Making Broker");
    // Make Broker
    Broker broker;

    MESSAGE("Making SceneLoader");
    // Make sceneLoader
    SceneLoader scenes(broker);
    // firstScene myScene(screenSize);
    // secondScene otherScene(screenSize);

    MESSAGE("Init InputCreator");
    // Make InputCreator
    InputCreator ic;

    MESSAGE("Init Main");
    // Make GraphicsMain
    GraphicsMain gMain(fb, ic, scenes);

    // Bind onClick
    gMain.subscribeToBroker(&broker, Topic::SceneChange);
    // scenes.GetScene("first").
    // myScene.topBtn.RegisterOnClicked(std::bind(&GraphicsMain::ChangeScene, &gMain));
    // otherScene.botBtn.RegisterOnClicked(std::bind(&GraphicsMain::ChangeScene, &gMain));

    // Run?
    gMain.Run();
}