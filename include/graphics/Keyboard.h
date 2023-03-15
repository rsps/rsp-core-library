/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_KEYBOARD_H_
#define INCLUDE_GRAPHICS_KEYBOARD_H_

#include <map>
#include <array>
#include <string>
#include "Bitmap.h"
#include "BitmapView.h"
#include "Button.h"
#include "Color.h"
#include "Control.h"
#include "Point.h"
#include "Rect.h"

namespace rsp::graphics {

/**
 * \brief Macro definitions for key placement and touch
 *        Set KB_X/Y to the offset on the screen where the macros
 *        are to be used.
 *        E.g. See InputScene unit test.
 */
#ifndef KB_X
#define KB_X 0
#define KB_Y 0
#endif

#define KEY(x,y) {KB_X + x, KB_Y + y}

#define _SPACE   KEY( 98, 224)
#define _NUMBERS KEY(  0, 224)
#define _LETTERS KEY(  0, 224)
#define _SPECIAL KEY(358, 224)
#define _SHIFT   KEY(  0, 152)
#define _ERASE   KEY(375, 152)

#define _Q KEY(  0,   0)
#define _W KEY( 59,   0)
#define _E KEY(101,   0)
#define _R KEY(143,   0)
#define _T KEY(185,   0)
#define _Y KEY(227,   0)
#define _U KEY(269,   0)
#define _I KEY(311,   0)
#define _O KEY(353,   0)
#define _P KEY(395,   0)
#define _A KEY(  0,  83)
#define _S KEY( 80,  83)
#define _D KEY(122,  83)
#define _F KEY(164,  83)
#define _G KEY(206,  83)
#define _H KEY(248,  83)
#define _J KEY(290,  83)
#define _K KEY(332,  83)
#define _L KEY(374,  83)
#define _Z KEY( 80, 152)
#define _X KEY(122, 152)
#define _C KEY(164, 152)
#define _V KEY(206, 152)
#define _B KEY(248, 152)
#define _N KEY(290, 152)
#define _M KEY(332, 152)

#define _1 _Q
#define _2 _W
#define _3 _E
#define _4 _R
#define _5 _T
#define _6 _Y
#define _7 _U
#define _8 _I
#define _9 _O
#define _0 _P
#define _MINUS _A
#define _UNDERSCORE _S
#define _COLON _D
#define _SEMICOLON _F
#define _LEFT_PARANTHESES _G
#define _RIGHT_PARANTHESES _H
#define _AMPERSAND _J
#define _CURLY_A _K
#define _QUOTE _L
#define _DOT _Z
#define _COMMA _X
#define _QUESTION_MARK _C
#define _EXCLAMATION _V
#define _APOSTROPHE _B
#define _SLASH _N
#define _ASTERISK _M

#define _LEFT_BRACKET _W
#define _RIGHT_BRACKET _E
#define _LEFT_BRACE _R
#define _RIGHT_BRACE _T
#define _NUMBER_SIGN _Y
#define _PERCENT_SIGN _U
#define _PLUS_SIGN _I
#define _TILDE _O
#define _EQUAL_SIGN _A
#define _CURRENCY_SIGN _S
#define _BACKSLASH _D
#define _ALMOST_EQUAL _F
#define _LESS_THAN _G
#define _GREATER_THAN _H
#define _POUND_SIGN _J
#define _EURO_SIGN _K
#define _DOLLAR_SIGN _L


class Key : public Button
{
public:
    Key();
    Key& Setup(Rect aTouchArea, Rect aArea, Point aPosition, int aSymbol = 0);
    Key& SetStyle(Control::States aState, TexturePtr_t *apForeground, TexturePtr_t *apBackground, Color aFrontColor, Color aBackColor);
};

class Keyboard: public Control
{
public:
    using KeyboardCallback_t = rsp::utils::Function<void(const std::string &)>;

    static constexpr int cKEY_SHIFT    = 1000000;
    static constexpr int cKEY_LETTERS  = 1000001;
    static constexpr int cKEY_NUMBERS  = 1000002;
    static constexpr int cKEY_SPECIALS = 1000003;

    const Rect cSpecialLeft = {0, 224, 98, 64};
    const Rect cSpecialRight = {356, 224, 98, 64};

    enum class LayoutType { Letters, Numbers, Special };

    Keyboard();
    ~Keyboard();

    void SetLayout(LayoutType aLayout);

    const std::string GetInput() { return mInput; }
    Keyboard& SetInput(const std::string &arText);

    KeyboardCallback_t& OnKeyClick() { return mOnKeyClick; }
protected:
    KeyboardCallback_t mOnKeyClick{};
    std::array<Key, 26u> mKeys{};
    Key mBtnShift{};
    Key mBtnLetters{};
    Key mBtnNumbers{};
    Key mBtnSpecials{};
    Key mBtnErase{};
    Key mBtnSpace{};
    std::string mInput{};

    void addBtn(Key &arBtn);
    void setSymbols(const std::string &arSymbols);
    void doKeyClick(const Point &arPoint, uint32_t aSymbol);
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_KEYBOARD_H_ */
