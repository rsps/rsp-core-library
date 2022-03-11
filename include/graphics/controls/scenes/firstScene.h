#ifndef FIRSTSCENE_H
#define FIRSTSCENE_H

/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/controls/Scene.h"
#include <functional>
#include <vector>

namespace rsp::graphics
{
class FirstScene : public Scene
{
  public:
    FirstScene(Rect &aRect, rsp::messaging::BrokerBase &arBroker)
        : Scene(aRect, arBroker)
    {
        // myName = "First Scene";
        // Set member variables values
        Rect topRect(Point(100, 400), 200, 100);
        topBtn = TouchArea(topRect, rsp::messaging::ClickTopic::SceneChange, "second");
        topBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", topRect);

        Rect botRect(Point(100, 600), 200, 100);
        botBtn = TouchArea(botRect);
        botBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", botRect);

        // Bind onPressed
        topBtn.RegisterOnPressed(std::bind(&Image::HandleCallback, &topBtnImg, std::placeholders::_1));
        botBtn.RegisterOnPressed(std::bind(&Image::HandleCallback, &botBtnImg, std::placeholders::_1));

        // Bind onClicked or set clicked variables || Should be set from Constructor

        // Add them to the lists?
        mTouchables.push_back(&topBtn);
        mTouchables.push_back(&botBtn);
        mChildren.push_back(&topBtnImg);
        mChildren.push_back(&botBtnImg);
    };

    ~FirstScene() {}

    Color background{0x323232}; // color from old main.qml

    TouchArea topBtn{};
    TouchArea botBtn{};

    Image topBtnImg{};
    Image botBtnImg{};
};
} // namespace rsp::graphics
#endif // FIRSTSCENE_H
