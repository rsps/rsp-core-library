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
#include <TestTouchParser.h>

namespace rsp::graphics {

class SecondScene : public SceneBase<SecondScene>
{
public:
    using Clicked_t = std::function<void(void)>;

    static std::array<TestEventItem_t, 15>& GetTouchEvents() {
        static std::array events {
            MAKE_TOUCH_ITEM(100, TouchTypes::Press, Point(100, 100)),
            MAKE_TOUCH_ITEM(110, TouchTypes::Lift, Point(100, 100)),  // Click outside any buttons

            MAKE_TOUCH_ITEM(200, TouchTypes::Press, Point(100, 150)),
            MAKE_TOUCH_ITEM(210, TouchTypes::Lift, Point(100, 150)),  // Click top left corner of TopBtn
            MAKE_TOUCH_ITEM(220, TouchTypes::Press, Point(299, 249)),
            MAKE_TOUCH_ITEM(230, TouchTypes::Lift, Point(299, 249)),  // Click bottom right corner of TopBtn

            MAKE_TOUCH_ITEM(300, TouchTypes::Press, Point(100, 300)), // Press top left corner of BotBtn
            MAKE_TOUCH_ITEM(310, TouchTypes::Drag, Point(150, 320)),  // Drag over BotBtn
            MAKE_TOUCH_ITEM(320, TouchTypes::Drag, Point(200, 340)),  // Drag over BotBtn
            MAKE_TOUCH_ITEM(330, TouchTypes::Drag, Point(250, 360)),  // Drag over BotBtn
            MAKE_TOUCH_ITEM(340, TouchTypes::Drag, Point(300, 380)),  // Drag over BotBtn
            MAKE_TOUCH_ITEM(350, TouchTypes::Drag, Point(310, 390)),  // Drag outside BotBtn
            MAKE_TOUCH_ITEM(360, TouchTypes::Lift, Point(310, 390)),  // Lift outside BotBtn

            MakeEventItem<RefreshEvent>(400),   // Refresh display
            MakeEventItem<QuitEvent>(410)       // Terminate
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
