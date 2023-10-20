/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_GRAPHICS_KEYBOARD_H
#define RSP_CORE_LIB_GRAPHICS_KEYBOARD_H

#include <array>
#include <map>
#include <string>
#include <vector>
#include "Bitmap.h"
#include "BitmapView.h"
#include "Button.h"
#include "Color.h"
#include "Control.h"
#include "Point.h"
#include "Rect.h"
#include <utils/EnumFlags.h>

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

#define K_SPACE   KEY( 98, 224)
#define K_NUMBERS KEY(  0, 224)
#define K_LETTERS KEY(  0, 224)
#define K_SPECIAL KEY(358, 224)
#define K_SHIFT   KEY(  0, 152)
#define K_ERASE   KEY(375, 152)

#define K_Q KEY(  0,   0)
#define K_W KEY( 59,   0)
#define K_E KEY(101,   0)
#define K_R KEY(143,   0)
#define K_T KEY(185,   0)
#define K_Y KEY(227,   0)
#define K_U KEY(269,   0)
#define K_I KEY(311,   0)
#define K_O KEY(353,   0)
#define K_P KEY(395,   0)
#define K_A KEY(  0,  83)
#define K_S KEY( 80,  83)
#define K_D KEY(122,  83)
#define K_F KEY(164,  83)
#define K_G KEY(206,  83)
#define K_H KEY(248,  83)
#define K_J KEY(290,  83)
#define K_K KEY(332,  83)
#define K_L KEY(374,  83)
#define K_Z KEY( 80, 152)
#define K_X KEY(122, 152)
#define K_C KEY(164, 152)
#define K_V KEY(206, 152)
#define K_B KEY(248, 152)
#define K_N KEY(290, 152)
#define K_M KEY(332, 152)

#define K_NUM_1 K_Q
#define K_NUM_2 K_W
#define K_NUM_3 K_E
#define K_NUM_4 K_R
#define K_NUM_5 K_T
#define K_NUM_6 K_Y
#define K_NUM_7 K_U
#define K_NUM_8 K_I
#define K_NUM_9 K_O
#define K_NUM_0 K_P
#define K_MINUS K_A
#define K_UNDERSCORE K_S
#define K_COLON K_D
#define K_SEMICOLON K_F
#define K_LEFT_PARANTHESES K_G
#define K_RIGHT_PARANTHESES K_H
#define K_AMPERSAND K_J
#define K_CURLY_A K_K
#define K_QUOTE K_L
#define K_DOT K_Z
#define K_COMMA K_X
#define K_QUESTION_MARK K_C
#define K_EXCLAMATION K_V
#define K_APOSTROPHE K_B
#define K_SLASH K_N
#define K_ASTERISK K_M

#define K_LEFT_BRACKET K_W
#define K_RIGHT_BRACKET K_E
#define K_LEFT_BRACE K_R
#define K_RIGHT_BRACE K_T
#define K_NUMBER_SIGN K_Y
#define K_PERCENT_SIGN K_U
#define K_PLUS_SIGN K_I
#define K_TILDE K_O
#define K_EQUAL_SIGN K_A
#define K_CURRENCY_SIGN K_S
#define K_BACKSLASH K_D
#define K_ALMOST_EQUAL K_F
#define K_LESS_THAN K_G
#define K_GREATER_THAN K_H
#define K_POUND_SIGN K_J
#define K_EURO_SIGN K_K
#define K_DOLLAR_SIGN K_L


class Key : public Button
{
public:
    Key();
    Key& Symbol(int aSymbol);
};

class KeyboardBase: public Control
{
public:
    using KeyboardCallback_t = rsp::messaging::Notifier<const std::string &>;

    [[nodiscard]] const std::string& GetInput() const { return mInput; }
    KeyboardBase& SetInput(const std::string &arText);

    KeyboardCallback_t& OnKeyClick() { return mOnKeyClick; }

protected:
    KeyboardCallback_t mOnKeyClick{};
    std::vector<Key> mKeys{};
    std::string mInput{};
    std::vector<TouchCallback_t::Listener_t> mKeyClicks{};

    void addBtn(Key &arBtn);
    void setSymbols(const std::string &arSymbols, bool aUpperCase = false);
    virtual void doKeyClick(const TouchEvent &arEvent, uint32_t aSymbol) = 0;
};

class Keyboard: public KeyboardBase
{
public:
    static constexpr int cKEY_SHIFT    = 1000000;
    static constexpr int cKEY_LETTERS  = 1000001;
    static constexpr int cKEY_NUMBERS  = 1000002;
    static constexpr int cKEY_SPECIALS = 1000003;

    const Rect cSpecialLeft = {0, 224, 98, 64};
    const Rect cSpecialRight = {356, 224, 98, 64};

    enum class LayoutType { Letters, Numbers, Special };

    enum class TextureId { BigSpecial = 1000, Erase, Key, LowerCase, SmallSpecial, Space, UpperCase };

    enum class Buttons {
        None      = 0x00,
        Shift     = 0x01,
        Letters   = 0x02,
        Numbers   = 0x04,
        Specials  = 0x08,
        Backspace = 0x10,
        Space     = 0x20,
        All       = 0x3F
    };

    Keyboard();
    ~Keyboard() override;

    Keyboard& SetLayout(LayoutType aLayout);
    [[nodiscard]] LayoutType GetLayout() const { return mLayout; }

    Keyboard& AllowedButtons(utils::EnumFlags<Buttons> aMask);
protected:
    Key mBtnShift{};
    Key mBtnLettersLeft{};
    Key mBtnLettersRight{};
    Key mBtnNumbers{};
    Key mBtnSpecials{};
    Key mBtnErase{};
    Key mBtnSpace{};
    Rect mBigSpecialRect{};
    utils::EnumFlags<Buttons> mButtonMask = Buttons::All;
    LayoutType mLayout{};

    void doKeyClick(const TouchEvent &arEvent, uint32_t aSymbol) override;
    const PixelData& getPixelData(TextureId aId);
};

} /* namespace rsp::graphics */

#endif // RSP_CORE_LIB_GRAPHICS_KEYBOARD_H
