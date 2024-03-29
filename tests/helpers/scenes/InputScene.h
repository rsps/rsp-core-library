/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef TESTS_HELPERS_SCENES_INPUTSCENE_H_
#define TESTS_HELPERS_SCENES_INPUTSCENE_H_

#define KB_X 12
#define KB_Y 450

#include <array>
#include <graphics/controls/Button.h>
#include <graphics/controls/Keyboard.h>
#include <graphics/controls/Label.h>
#include <graphics/controls/Scene.h>
#include <graphics/TouchEvent.h>
#include <graphics/primitives/Bitmap.h>

namespace rsp::graphics {

#define CLICK(_time, _key) \
    TouchEvent(_time, TouchEvent::Types::Press, _key), \
    TouchEvent(_time+20, TouchEvent::Types::Lift, _key)


class InputScene : public SceneBase<InputScene>
{
public:
    static std::array<TouchEvent, 44>& GetTouchEvents() {
        static std::array events {
            CLICK(300, _SHIFT),
            CLICK(400, _H),
            CLICK(500, _SHIFT),
            CLICK(600, _E),
            CLICK(700, _L),
            CLICK(800, _L),
            CLICK(900, _O),
            CLICK(1000, _SPACE),
            CLICK(1100, _NUMBERS),
            CLICK(1200, _1),
            CLICK(1300, _2),
            CLICK(1400, _8),
            CLICK(1500, _0),
            CLICK(1700, _ERASE),
            CLICK(1800, _SPECIAL),
            CLICK(1900, _EURO_SIGN),
            CLICK(2000, _QUESTION_MARK),
            CLICK(2100, _SPECIAL),
            CLICK(2200, _S),
            CLICK(2250, _D),
            CLICK(2300, _G),
            CLICK(2350, _P),
        };
        return events;
    }

    InputScene()
        : mBackground("testImages/rgb/Background.bmp")
    {
        SetTransparent(true); // No need to paint background color, we have full screen image.

        GetStyle(Control::States::normal).mBackground.SetPixelData(mBackground);

        mLabel.GetText().GetFont().SetSize(34);
        mLabel.SetArea(Rect(80, 150, 320, 160));
        mLabel.GetStyle(States::normal).mForegroundColor = Color::Black;
        mLabel.GetStyle(States::normal).mBackgroundColor = Color::White;

        mKeyboard.SetArea(Rect(KB_X, KB_Y, 460, 350));
        mKeyboard.GetInfo().mName = "Keyboard";
        mKeyboard.OnKeyClick() = rsp::utils::Method(this, &InputScene::onInputChange);

        AddChild(&mLabel);
        AddChild(&mKeyboard);
    };

    Keyboard& GetKeyboard() { return mKeyboard; }
    Label& GetLabel() { return mLabel; }

protected:
    Label mLabel{};
    Keyboard mKeyboard{};
    Bitmap mBackground{};

    void onInputChange(const std::string &arInput)
    {
        mLabel.SetCaption(arInput);
    }
};

} // namespace rsp::graphics

#endif /* TESTS_HELPERS_SCENES_INPUTSCENE_H_ */
