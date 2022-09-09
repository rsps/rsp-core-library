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
#include <graphics/TouchEvent.h>

namespace rsp::graphics {

class SecondScene : public SceneBase<SecondScene>
{
public:
    using Clicked_t = rsp::utils::Function<void(void)>;

    static std::array<TouchEvent, 13>& GetTouchEvents() {
        static std::array events {
            TouchEvent(50, TouchEvent::Types::Press, Point(100, 100)),
            TouchEvent(51, TouchEvent::Types::Lift, Point(100, 100)),  // Click outside any buttons

            TouchEvent(60, TouchEvent::Types::Press, Point(100, 150)),
            TouchEvent(61, TouchEvent::Types::Lift, Point(100, 150)),  // Click top left corner of TopBtn
            TouchEvent(62, TouchEvent::Types::Press, Point(299, 249)),
            TouchEvent(63, TouchEvent::Types::Lift, Point(299, 249)),  // Click bottom right corner of TopBtn

            TouchEvent(70, TouchEvent::Types::Press, Point(100, 300)), // Press top left corner of BotBtn
            TouchEvent(71, TouchEvent::Types::Drag, Point(150, 320)),  // Drag over BotBtn
            TouchEvent(72, TouchEvent::Types::Drag, Point(200, 340)),  // Drag over BotBtn
            TouchEvent(73, TouchEvent::Types::Drag, Point(250, 360)),  // Drag over BotBtn
            TouchEvent(74, TouchEvent::Types::Drag, Point(300, 380)),  // Drag over BotBtn
            TouchEvent(75, TouchEvent::Types::Drag, Point(310, 390)),  // Drag outside BotBtn
            TouchEvent(76, TouchEvent::Types::Lift, Point(310, 390))   // Lift outside BotBtn
        };
        return events;
    }


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
    static Rect GetBotRect() { return Rect(100, 300, 200, 100); }

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
