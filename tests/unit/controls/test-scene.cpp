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
#include "graphics/controls/Scene.h"
#include "messaging/Subscriber.h"
#include "posix/FileSystem.h"
#include <doctest.h>

using namespace rsp::graphics;
using namespace rsp::messaging;

class TestScene : public Scene
{
  public:
    TestScene(Rect &aRect, rsp::messaging::Broker<rsp::messaging::ClickTopics> &arBroker)
        : Scene(aRect, arBroker)
    {
        // Set member variables values
        Rect topRect(Point(100, 400), 200, 100);
        topArea = TouchArea(topRect, rsp::messaging::ClickTopics::NullTopic, "");
        topImg = Image("testImages/Red.bmp", "testImages/Green.bmp", topRect);

        Rect botRect(Point(100, 600), 200, 100);
        botArea = TouchArea(botRect);
        botImg = Image("testImages/Red.bmp", "testImages/Green.bmp", botRect);

        // Bind onPressed
        topArea.RegisterOnPressed(std::bind(&Image::HandleCallback, &topImg, std::placeholders::_1));
        botArea.RegisterOnPressed(std::bind(&Image::HandleCallback, &botImg, std::placeholders::_1));

        // Add them to the lists
        mTouchables.push_back(&topArea);
        mTouchables.push_back(&botArea);
        mChildren.push_back(&topImg);
        mChildren.push_back(&botImg);
    };

    ~TestScene() {}

    std::vector<TouchArea *> &GetTouchables()
    {
        return mTouchables;
    }
    std::vector<Control *> &GetImages()
    {
        return mChildren;
    }

    Color background{0x323232}; // color from old main.qml

    TouchArea topArea{};
    TouchArea botArea{};

    Image topImg{};
    Image botImg{};
};

class TestSub : public Subscriber<ClickTopics>
{
  public:
    TestSub(Broker<ClickTopics>& arBroker)
        : Subscriber<ClickTopics>(arBroker)
    {
    }

    void HandleEvent(Event &arNewEvent) override
    {
        calledCount++;
    }
    int calledCount = 0;
};
TEST_CASE("Scene Test")
{
    // Arrange
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());
    Rect screenSize = Rect(0, 0, 480, 800);
    Broker<ClickTopics> testBroker;
    TestScene testScene(screenSize, testBroker);
    Input anInput;
    Point insideTopPoint(testScene.topArea.GetArea().GetLeft() + (rand() % testScene.topArea.GetArea().GetWidth()),
                         testScene.topArea.GetArea().GetTop() + (rand() % testScene.topArea.GetArea().GetHeight()));
    Point insideBotPoint(testScene.botArea.GetArea().GetLeft() + (rand() % testScene.botArea.GetArea().GetWidth()),
                         testScene.botArea.GetArea().GetTop() + (rand() % testScene.botArea.GetArea().GetHeight()));

    SUBCASE("Scene Process Input")
    {
        // Arrange
        anInput.type = InputType::Press;
        testScene.Render(fb);

        SUBCASE("Process input for Top elements")
        {
            // Arrange
            anInput.x = insideTopPoint.GetX();
            anInput.y = insideTopPoint.GetY();
            MESSAGE("Input x:" << anInput.x);
            MESSAGE("Input y:" << anInput.y);

            // Act
            testScene.ProcessInput(anInput);

            // Assert
            CHECK(testScene.topImg.IsInvalid());
            CHECK_FALSE(testScene.botImg.IsInvalid());
        }
        SUBCASE("Process input for Bot elements")
        {
            // Arrange
            anInput.x = insideBotPoint.GetX();
            anInput.y = insideBotPoint.GetY();

            // Act
            testScene.ProcessInput(anInput);

            // Assert
            CHECK(testScene.botImg.IsInvalid());
            CHECK_FALSE(testScene.topImg.IsInvalid());
        }
    }
    SUBCASE("Scene Render Elements")
    {
        // Arrange

        for (auto image : testScene.GetImages()) {
            image->Invalidate();
        }

        // Act
        testScene.Render(fb);

        // Assert
        CHECK_FALSE(testScene.topImg.IsInvalid());
        CHECK_FALSE(testScene.botImg.IsInvalid());
    }

    SUBCASE("Scene Bind click Callbacks")
    {
        // Arrange
        TestSub sub(testBroker);
        sub.Subscribe(ClickTopics::NullTopic);
        anInput.x = insideTopPoint.GetX();
        anInput.y = insideTopPoint.GetY();

        // Act
        testScene.BindElementsToBroker();
        anInput.type = InputType::Press;
        testScene.ProcessInput(anInput);
        anInput.type = InputType::Lift;
        testScene.ProcessInput(anInput);

        // Assert
        CHECK(sub.calledCount == 1);
    }
}
