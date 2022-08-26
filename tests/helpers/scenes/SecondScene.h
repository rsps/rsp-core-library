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
#include <graphics/controls/Scene.h>

namespace rsp::graphics
{
class SecondScene : public SceneBase<SecondScene>
{
public:
    using Clicked_t = rsp::utils::Function<void(void)>;

    SecondScene()
        : mRed("testImages/Red.bmp"),
          mGreen("testImages/Green.bmp")
    {
        // myName = "Second Scene";
        //  Set member variables values
        mTopBtn.SetArea(GetTopRect());
        mTopBtn.GetTouchArea() = mTopBtn.GetArea();
        mTopBtn.GetImage().GetStyle(Control::States::normal).mpBitmap = &mRed;
        mTopBtn.GetImage().GetStyle(Control::States::pressed).mpBitmap = &mGreen;
        mTopBtn.GetInfo().mName = "TopBtn";

        mBotBtn.SetArea(GetBotRect());
        mBotBtn.GetTouchArea() = mBotBtn.GetArea();
        mBotBtn.GetImage().GetStyle(Control::States::normal).mpBitmap = &mRed;
        mBotBtn.GetImage().GetStyle(Control::States::pressed).mpBitmap = &mGreen;
        mBotBtn.GetOnClick() = std::bind(&SecondScene::doClick, this, std::placeholders::_1);
        mTopBtn.GetInfo().mName = "BotBtn";

        //  Add them to the lists?
        AddChild(&mTopBtn);
        AddChild(&mBotBtn);
    };

    static Rect GetTopRect() { return Rect(100, 150, 200, 100); }
    static Rect GetBotRect() { return Rect(100, 300, 300, 100); }

//    TouchControl& GetTopArea() { return *mTouchables[0]; }
//    TouchControl& GetBotArea() { return *mTouchables[1]; }

    Clicked_t& WhenClicked() { return mWhenClicked; }

    Image& GetTopImg() { return mTopBtn.GetImage();};
    Image& GetBotImg() { return mBotBtn.GetImage();};

protected:
    Bitmap mRed;
    Bitmap mGreen;
    Button mTopBtn{};
    Button mBotBtn{};
    Clicked_t mWhenClicked{};

    void doClick(const Point &arPoint) {
        std::cout << "doClick: " << arPoint << std::endl;
        mWhenClicked();
    }
};

} // namespace rsp::graphics
#endif // SECONDSCENE_H
