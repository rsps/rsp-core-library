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

namespace rsp::graphics {

class SecondScene : public SceneBase<SecondScene>
{
public:
    using Clicked_t = rsp::utils::Function<void(void)>;

    SecondScene()
        : mNormal("testImages/Green.bmp"),
          mPressed("testImages/Red.bmp")
    {
        // myName = "Second Scene";
        //  Set member variables values
        mTopBtn.SetArea(GetTopRect());
        mTopBtn.SetTouchArea(GetTopRect());
        mTopBtn.GetStyle(Control::States::normal).mBackground.SetBitmap(&mNormal);
        mTopBtn.GetStyle(Control::States::pressed).mBackground.SetBitmap(&mPressed);
        mTopBtn.SetDraggable(true);
        mTopBtn.GetInfo().mName = "TopBtn";

        mBotBtn.SetArea(GetBotRect());
        mBotBtn.SetTouchArea(GetBotRect());
        mBotBtn.GetStyle(Control::States::normal).mBackground.SetBitmap(&mNormal);
        mBotBtn.GetStyle(Control::States::pressed).mBackground.SetBitmap(&mPressed);
        mBotBtn.GetStyle(Control::States::dragged).mBackground.SetBitmap(&mPressed);
        mBotBtn.SetDraggable(true);
        mBotBtn.GetInfo().mName = "BotBtn";

        //  Add them to the lists?
        AddChild(&mTopBtn);
        AddChild(&mBotBtn);
    };

    static Rect GetTopRect() { return Rect(100, 150, 200, 100); }
    static Rect GetBotRect() { return Rect(200, 300, 200, 100); }

    Button& GetTopBtn() { return mTopBtn; }
    Button& GetBottomBtn() { return mBotBtn; }

protected:
    Bitmap mNormal;
    Bitmap mPressed;
    Button mTopBtn{};
    Button mBotBtn{};
};

} // namespace rsp::graphics
#endif // SECONDSCENE_H
