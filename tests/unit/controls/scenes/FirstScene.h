/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef FIRSTSCENE_H
#define FIRSTSCENE_H

#include "Scene480x800.h"

namespace rsp::graphics {

class FirstScene : public Scene480x800
{
public:
    FirstScene()
        : Scene480x800("FirstScene")
    {
        std::cout << "FirstScene" << std::endl;
        // myName = "First Scene";
        // Set member variables values
        Rect topRect(100, 400, 200, 100);
        mTopBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", topRect);
        TouchArea& top_btn = makeTouchArea(topRect);
        top_btn.GetOnPress() = [this](const Point &arPoint) { mTopBtnImg.SetState(Control::States::pressed); };
        top_btn.GetOnLift() = [this](const Point &arPoint) { mTopBtnImg.SetState(Control::States::normal); };

        std::cout << "FirstScene 1" << std::endl;
        Rect botRect(100, 600, 200, 100);
        mBotBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", botRect);
        TouchArea& bot_btn = makeTouchArea(botRect);
        std::cout << "FirstScene 2" << std::endl;
        bot_btn.GetOnPress() = [this](const Point &arPoint) { mBotBtnImg.SetState(Control::States::pressed); };
        bot_btn.GetOnLift() = [this](const Point &arPoint) { mBotBtnImg.SetState(Control::States::normal); };


        // Add them to the lists?
        AddChild(&mTopBtnImg);
        AddChild(&mBotBtnImg);
        std::cout << "FirstScene exit" << std::endl;
    };

protected:
    Image mTopBtnImg{};
    Image mBotBtnImg{};
};

} // namespace rsp::graphics

#endif // FIRSTSCENE_H
