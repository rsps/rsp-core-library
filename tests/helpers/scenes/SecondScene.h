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
#include <graphics/primitives/Bitmap.h>

namespace rsp::graphics
{
class SecondScene : public SceneBase<SecondScene>
{
public:
    using Clicked_t = rsp::utils::Function<void(void)>;

    SecondScene()
        : mNormal("testImages/Red.bmp"),
          mPressed("testImages/Green.bmp")
    {
        // myName = "Second Scene";
        //  Set member variables values
        mTopBtn.SetArea(GetTopRect());
        mTopBtn.GetTouchArea() = mTopBtn.GetArea();
        mTopBtn.GetImage().GetStyle(Control::States::normal).mpBitmap = &mNormal;
        mTopBtn.GetImage().GetStyle(Control::States::pressed).mpBitmap = &mPressed;
        mTopBtn.GetInfo().mName = "TopBtn";

        mBotBtn.SetArea(GetBotRect());
        mBotBtn.GetTouchArea() = mBotBtn.GetArea();
        mBotBtn.GetImage().GetStyle(Control::States::normal).mpBitmap = &mNormal;
        mBotBtn.GetImage().GetStyle(Control::States::pressed).mpBitmap = &mPressed;
        mTopBtn.GetInfo().mName = "BotBtn";

        //  Add them to the lists?
        AddChild(&mTopBtn);
        AddChild(&mBotBtn);
    };

    static Rect GetTopRect() { return Rect(100, 150, 200, 100); }
    static Rect GetBotRect() { return Rect(100, 300, 300, 100); }

    Button& GetBottomBtn() { return mBotBtn; }

    Image& GetTopImg() { return mTopBtn.GetImage();};
    Image& GetBotImg() { return mBotBtn.GetImage();};

protected:
    Bitmap mNormal;
    Bitmap mPressed;
    Button mTopBtn{};
    Button mBotBtn{};
};

} // namespace rsp::graphics
#endif // SECONDSCENE_H
