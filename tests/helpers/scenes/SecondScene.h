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

#include <graphics/Button.h>
#include <graphics/GfxInputEvents.h>
#include <graphics/Scene.h>

namespace rsp::graphics {

class SecondScene : public SceneBase<SecondScene>
{
public:
    using Clicked_t = rsp::utils::Function<void(void)>;

    static std::array<GfxEvent, 15>& GetTouchEvents() {
        static std::array events {
            GfxEvent(TouchEvent(50, TouchTypes::Press, Point(100, 100))),
            GfxEvent(TouchEvent(51, TouchTypes::Lift, Point(100, 100))),  // Click outside any buttons

            GfxEvent(TouchEvent(60, TouchTypes::Press, Point(100, 150))),
            GfxEvent(TouchEvent(61, TouchTypes::Lift, Point(100, 150))),  // Click top left corner of TopBtn
            GfxEvent(TouchEvent(62, TouchTypes::Press, Point(299, 249))),
            GfxEvent(TouchEvent(63, TouchTypes::Lift, Point(299, 249))),  // Click bottom right corner of TopBtn

            GfxEvent(TouchEvent(70, TouchTypes::Press, Point(100, 300))), // Press top left corner of BotBtn
            GfxEvent(TouchEvent(71, TouchTypes::Drag, Point(150, 320))),  // Drag over BotBtn
            GfxEvent(TouchEvent(72, TouchTypes::Drag, Point(200, 340))),  // Drag over BotBtn
            GfxEvent(TouchEvent(73, TouchTypes::Drag, Point(250, 360))),  // Drag over BotBtn
            GfxEvent(TouchEvent(74, TouchTypes::Drag, Point(300, 380))),  // Drag over BotBtn
            GfxEvent(TouchEvent(75, TouchTypes::Drag, Point(310, 390))),  // Drag outside BotBtn
            GfxEvent(TouchEvent(76, TouchTypes::Lift, Point(310, 390))),  // Lift outside BotBtn

            GfxEvent(RefreshEvent()),   // Refresh display
            GfxEvent(QuitEvent())       // Terminate
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
        mTopBtn.GetStyle(Control::States::Normal).mTextures.push_back(Texture::Create(mNormal, Color::White, {}, GetTopRect().GetTopLeft()));
        mTopBtn.GetStyle(Control::States::Pressed).mTextures.push_back(Texture::Create(mPressed, Color::White, {}, GetTopRect().GetTopLeft()));
        mTopBtn.SetDraggable(true);
        mTopBtn.SetName("TopBtn");

        mBotBtn.SetArea(GetBotRect());
        mBotBtn.SetTouchArea(GetBotRect());
        mBotBtn.GetStyle(Control::States::Normal).mTextures.push_back(Texture::Create(mNormal, Color::White, {}, GetBotRect().GetTopLeft()));
        mBotBtn.GetStyle(Control::States::Pressed).mTextures.push_back(Texture::Create(mPressed, Color::White, {}, GetBotRect().GetTopLeft()));
        mBotBtn.GetStyle(Control::States::Dragged).mTextures.push_back(Texture::Create(mPressed, Color::White, {}, GetBotRect().GetTopLeft()));
        mBotBtn.SetDraggable(true);
        mBotBtn.SetName("BotBtn");

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
