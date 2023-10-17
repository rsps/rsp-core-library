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
#define KB_Y 400

#include <array>
#include <graphics/Button.h>
#include <graphics/Keyboard.h>
#include <graphics/Label.h>
#include <graphics/Scene.h>
#include <graphics/GfxInputEvents.h>
#include <graphics/Bitmap.h>
#include <TestTouchParser.h>
#include <utils/Function.h>

namespace rsp::graphics {

#define CLICK(_time, _key) \
    MakeEventItem<TouchEvent>(_time, _time, TouchTypes::Press, Point(_key)), \
    MakeEventItem<TouchEvent>(_time+20, _time+20, TouchTypes::Lift, Point(_key))


class InputScene : public SceneBase<InputScene>
{
public:
    static std::array<TestEventItem_t, 44>& GetTouchEvents() {
        static std::array events {
            CLICK(300, K_SHIFT),
            CLICK(400, K_H),
            CLICK(500, K_SHIFT),
            CLICK(600, K_E),
            CLICK(700, K_L),
            CLICK(800, K_L),
            CLICK(900, K_O),
            CLICK(1000, K_SPACE),
            CLICK(1100, K_NUMBERS),
            CLICK(1200, K_NUM_1),
            CLICK(1300, K_NUM_2),
            CLICK(1400, K_NUM_8),
            CLICK(1500, K_NUM_0),
            CLICK(1700, K_ERASE),
            CLICK(1800, K_SPECIAL),
            CLICK(1900, K_EURO_SIGN),
            CLICK(2000, K_QUESTION_MARK),
            CLICK(2100, K_SPECIAL),
            CLICK(2200, K_S),
            CLICK(2250, K_D),
            CLICK(2300, K_G),
            CLICK(2350, K_P)
        };
        return events;
    }

    InputScene()
        : mBackground("testImages/rgb/Background.bmp")
    {
        SetTransparent(true); // No need to paint background color, we have full screen image.

        GetStyle(Control::States::Normal).mTextures.push_back(Texture::Create(mBackground));

        mLabel.SetFontSize(34).SetArea(Rect(80, 150, 320, 160));
        mLabel.GetStyle(States::Normal).mForegroundColor = Color::Black;
        mLabel.GetStyle(States::Normal).mBackgroundColor = Color::White;

//        mKeyboard.SetArea(Rect(KB_X, KB_Y, 460, 350));
        mKeyboard.SetOrigin(Point(KB_X, KB_Y));
        mKeyboard.SetName("Keyboard");
        mListener = mKeyboard.OnKeyClick().Listen(rsp::utils::Method(this, &InputScene::onInputChange));
//        mKeyboard.SetInput("qwertyuiopzxcvbnm");

        AddChild(&mLabel);
        AddChild(&mKeyboard);
    };

    Keyboard& GetKeyboard() { return mKeyboard; }
    Label& GetLabel() { return mLabel; }

protected:
    Label mLabel{};
    Keyboard mKeyboard{};
    Bitmap mBackground;
    TouchCallback_t::Listener_t mListener{};

    void onInputChange(const std::string &arInput)
    {
        mLabel.SetCaption(arInput);
    }
};

} // namespace rsp::graphics

#endif /* TESTS_HELPERS_SCENES_INPUTSCENE_H_ */
