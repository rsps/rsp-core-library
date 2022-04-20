/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */
#ifndef SECONDSCENE_H
#define SECONDSCENE_H

#include <graphics/controls/Image.h>
#include "Scene480x800.h"

namespace rsp::graphics
{
class SecondScene : public Scene480x800
{
public:
    using Clicked_t = rsp::utils::Function<void(void)>;

    SecondScene() //, rsp::messaging::Broker<rsp::messaging::ClickTopics> &arBroker)
        : Scene480x800("SecondScene")
    {
        // myName = "Second Scene";
        //  Set member variables values
        Rect topRect(100, 150, 200, 100);
        mTopBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", topRect);
        TouchArea& top_btn = makeTouchArea(topRect);
        top_btn.GetOnPress() = [this](const Point &arPoint) { mTopBtnImg.SetState(Control::States::pressed); };
        top_btn.GetOnLift() = [this](const Point &arPoint) { mTopBtnImg.SetState(Control::States::normal); };

        Rect botRect(100, 300, 200, 100);
        mBotBtnImg = Image("testImages/Red.bmp", "testImages/Green.bmp", botRect);
        TouchArea& bot_btn = makeTouchArea(botRect);
        bot_btn.GetOnPress() = [this](const Point &arPoint) { mBotBtnImg.SetState(Control::States::pressed); };
        bot_btn.GetOnLift() = [this](const Point &arPoint) { mBotBtnImg.SetState(Control::States::normal); };
        bot_btn.GetOnClick() = std::bind(&SecondScene::doClick, this, std::placeholders::_1);

        //  Add them to the lists?
        AddChild(&mTopBtnImg);
        AddChild(&mBotBtnImg);
    };

    TouchArea& GetTopArea() { return mTouchables[0]; }
    TouchArea& GetBotArea() { return mTouchables[1]; }

    Clicked_t& Whenclicked() { return mWhenClicked; }

    Image& GetTopImg() { return mTopBtnImg;};
    Image& GetBotImg() { return mBotBtnImg;};

protected:
    Image mTopBtnImg{};
    Image mBotBtnImg{};
    Clicked_t mWhenClicked{};

    void doClick(const Point &arPoint) {
        mWhenClicked();
    }
};

} // namespace rsp::graphics
#endif // SECONDSCENE_H
