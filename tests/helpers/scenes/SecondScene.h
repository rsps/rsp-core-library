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

#include <graphics/controls/Button.h>
#include "Scene480x800.h"

namespace rsp::graphics
{
class SecondScene : public Scene480x800
{
public:
    using Clicked_t = rsp::utils::Function<void(void)>;

    SecondScene()
        : Scene480x800("SecondScene")
    {
        // myName = "Second Scene";
        //  Set member variables values
        Bitmap red("testImages/Red.bmp");
        Bitmap green("testImages/Green.bmp");

        Rect topRect(100, 150, 200, 100);
        mTopBtnImg.SetArea(topRect);
        mTopBtnImg.GetTouchArea() = topRect;
        mTopBtnImg.GetStyle(Control::States::normal).mpBitmap = &red;
        mTopBtnImg.GetStyle(Control::States::pressed).mpBitmap = &green;

        Rect botRect(100, 300, 300, 100);
        mBotBtnImg.SetArea(botRect);
        mBotBtnImg.GetTouchArea() = botRect;
        mBotBtnImg.GetStyle(Control::States::normal).mpBitmap = &red;
        mBotBtnImg.GetStyle(Control::States::pressed).mpBitmap = &green;
        mBotBtnImg.GetOnClick() = std::bind(&SecondScene::doClick, this, std::placeholders::_1);

        //  Add them to the lists?
        AddChild(&mTopBtnImg);
        AddChild(&mBotBtnImg);
    };

    TouchControl& GetTopArea() { return *mTouchables[0]; }
    TouchControl& GetBotArea() { return *mTouchables[1]; }

    Clicked_t& Whenclicked() { return mWhenClicked; }

    Image& GetTopImg() { return mTopBtnImg.GetImage();};
    Image& GetBotImg() { return mBotBtnImg.GetImage();};

protected:
    Button mTopBtnImg{};
    Button mBotBtnImg{};
    Clicked_t mWhenClicked{};

    void doClick(const Point &arPoint) {
        mWhenClicked();
    }
};

} // namespace rsp::graphics
#endif // SECONDSCENE_H
