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
#include <graphics/controls/SceneMap.h>
#include "scenes/FirstScene.h"
#include "scenes/SecondScene.h"

using namespace rsp::graphics;
using namespace rsp::messaging;

class Scenes : public SceneMap
{
public:
    Scenes() : SceneMap() {
        MakeScene<rsp::graphics::FirstScene>();
        MakeScene<rsp::graphics::SecondScene>();
    }

    rsp::graphics::FirstScene& First() { return *reinterpret_cast<rsp::graphics::FirstScene*>(&operator[]("FirstScene")); }
    rsp::graphics::SecondScene& Second() { return *reinterpret_cast<rsp::graphics::SecondScene*>(&operator[]("SecondScene")); }
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

    Scenes scenes;
    Input anInput;
    int click_count = 0;

    scenes.Second().Whenclicked() = [&click_count]() { click_count++; };

    Rect tr = scenes.Second().GetTopArea().GetArea();
    Point insideTopPoint(tr.GetLeft() + (rand() % tr.GetWidth()),
                         tr.GetTop() + (rand() % tr.GetHeight()));

    Rect br = scenes.Second().GetBotArea().GetArea();
    Point insideBotPoint(br.GetLeft() + (rand() % br.GetWidth()),
                         br.GetTop() + (rand() % br.GetHeight()));

    SUBCASE("Scene Process Input")
    {
        // Arrange
        anInput.type = InputType::Press;
        scenes.Second().Render(fb);

        SUBCASE("Process input for Top elements")
        {
            // Arrange
            anInput.x = insideTopPoint.GetX();
            anInput.y = insideTopPoint.GetY();
            MESSAGE("Input x:" << anInput.x);
            MESSAGE("Input y:" << anInput.y);

            // Act
            scenes.Second().ProcessInput(anInput);

            // Assert
            CHECK(scenes.Second().GetTopImg().IsInvalid());
            CHECK_FALSE(scenes.Second().GetBotImg().IsInvalid());
        }
        SUBCASE("Process input for Bot elements")
        {
            // Arrange
            anInput.x = insideBotPoint.GetX();
            anInput.y = insideBotPoint.GetY();

            // Act
            scenes.Second().ProcessInput(anInput);

            // Assert
            CHECK(scenes.Second().GetBotImg().IsInvalid());
            CHECK_FALSE(scenes.Second().GetTopImg().IsInvalid());
        }
    }
    SUBCASE("Scene Render Elements")
    {
        // Arrange
        scenes.Second().GetTopImg().Invalidate();
        scenes.Second().GetBotImg().Invalidate();
        CHECK(scenes.Second().GetTopImg().IsInvalid());
        CHECK(scenes.Second().GetBotImg().IsInvalid());

        // Act
        scenes.Second().Render(fb);

        // Assert
        CHECK_FALSE(scenes.Second().GetTopImg().IsInvalid());
        CHECK_FALSE(scenes.Second().GetBotImg().IsInvalid());
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
