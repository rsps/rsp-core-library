/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cctype>
#include <functional>
#include <string>
#include <locale>
#include <codecvt>
#include <utils/Function.h>
#include <graphics/controls/Keyboard.h>

#include "pixmaps/BigSpecial.h"
#include "pixmaps/SmallSpecial.h"
#include "pixmaps/Erase.h"
#include "pixmaps/Key.h"
#include "pixmaps/LowerCase.h"
#include "pixmaps/UpperCase.h"
#include "pixmaps/Space.h"

using namespace rsp::utils;

namespace rsp::graphics {


Key::Key()
{
    mText.GetFont().SetSize(34);
    SetTransparent(true);
}

Key& Key::Setup(Rect aTouchArea, Rect aArea, int aSymbol)
{
    SetArea(aArea);
//    SetArea(Rect(aPosition.GetX(), aPosition.GetY(),
//        static_cast<int>(std::max(arBackground.GetWidth(), arForeground.GetWidth())),
//        static_cast<int>(std::max(arBackground.GetHeight(), arForeground.GetHeight()))));
    SetTouchArea(aTouchArea);

    if (aSymbol) {
        if ((aSymbol < Keyboard::cKEY_SHIFT) && (aSymbol > ' ')) {
            std::cout << "Set key symbol: " << static_cast<char>(aSymbol) << " on TextArea(" << mText.GetArea() << ")" << std::endl;
            SetCaption(std::string(1, aSymbol));
        }
        SetId(aSymbol);
    }
    return *this;
}

Key& Key::SetStyle(Control::States aState, BitmapView &arForeground, BitmapView &arBackground, Color aFrontColor, Color aBackColor)
{
    Style style;
    style.mForeground = arForeground.SetPixelColor(aFrontColor);
    style.mForegroundColor = aFrontColor;
    style.mBackground = arBackground.SetPixelColor(aBackColor);
    style.mBackgroundColor = aBackColor;
    GetStyle(aState) = style;
    return *this;
}

static Rect cKeyTouchAreas[26] = {
    {  0,   0, 59, 83}, // Q
    { 59,   0, 42, 83}, // W
    {101,   0, 42, 83}, // E
    {143,   0, 42, 83}, // R
    {185,   0, 42, 83}, // T
    {227,   0, 42, 83}, // Y
    {269,   0, 42, 83}, // U
    {311,   0, 42, 83}, // I
    {353,   0, 42, 83}, // O
    {395,   0, 59, 83}, // P
    {  0,  83, 80, 69}, // A
    { 80,  83, 42, 69}, // S
    {122,  83, 42, 69}, // D
    {164,  83, 42, 69}, // F
    {206,  83, 42, 69}, // G
    {248,  83, 42, 69}, // H
    {290,  83, 42, 69}, // J
    {332,  83, 42, 69}, // K
    {374,  83, 80, 69}, // L
    { 80, 152, 42, 71}, // Z
    {122, 152, 42, 71}, // X
    {164, 152, 42, 71}, // C
    {206, 152, 42, 71}, // V
    {248, 152, 42, 71}, // B
    {290, 152, 42, 71}, // N
    {332, 152, 42, 71}  // M
};

static Point cKeyPositions[26] = {
    {18, 14},   // Q
    {60, 14},   // W
    {102, 14},  // E
    {144, 14},  // R
    {186, 14},  // T
    {228, 14},  // Y
    {270, 14},  // U
    {312, 14},  // I
    {354, 14},  // O
    {396, 14},  // P
    {39, 84},   // A
    {81, 84},   // S
    {123, 84},  // D
    {165, 84},  // F
    {207, 84},  // G
    {249, 84},  // H
    {291, 84},  // J
    {333, 84},  // K
    {375, 84},  // L
    {81, 154},  // Z
    {123, 154}, // X
    {165, 154}, // C
    {207, 154}, // V
    {249, 154}, // B
    {291, 154}, // N
    {333, 154}  // M
};

Keyboard::Keyboard()
    : Control(rsp::utils::MakeTypeInfo<Keyboard>())
{
    SetTransparent(true);

    mImages[ImageIds::BigSpecial].Assign(cBigSpecial);// Load("testImages/alpha/BigSpecial.bmp");
    mImages[ImageIds::SmallSpecial].Assign(cSmallSpecial); // Load("testImages/alpha/SmallSpecial.bmp");
    mImages[ImageIds::Space].Assign(cSpace); // Load("testImages/alpha/Space.bmp");
    mImages[ImageIds::Erase].Assign(cErase); // Load("testImages/alpha/Erase.bmp");
    mImages[ImageIds::Key].Assign(cKey); // Load("testImages/alpha/Key.bmp");
    mImages[ImageIds::LowerCase].Assign(cLowerCase); // Load("testImages/alpha/LowerCase.bmp");
    mImages[ImageIds::UpperCase].Assign(cUpperCase); // Load("testImages/alpha/UpperCase.bmp");

    BitmapView big_special(&mImages[ImageIds::BigSpecial]);
    BitmapView small_special(&mImages[ImageIds::SmallSpecial]);
    BitmapView space(&mImages[ImageIds::Space]);
    BitmapView erase(&mImages[ImageIds::Erase]);
    BitmapView key(&mImages[ImageIds::Key]);
    BitmapView lowercase(&mImages[ImageIds::LowerCase]);
    BitmapView uppercase(&mImages[ImageIds::UpperCase]);
    BitmapView empty;

    mBtnShift.Setup(Rect(0, 152, 80, 71), small_special.GetBoundingRect({18, 167}), cKEY_SHIFT)
        .SetStyle(Control::States::normal, lowercase.SetDestination({11, 7}), small_special, Color(0x494A63), Color::White)
        .SetStyle(Control::States::pressed, lowercase, small_special, Color::White, Color(0x494A63))
        .SetStyle(Control::States::checked, uppercase.SetDestination({11, 3}), small_special, Color(0x494A63), Color::White)
        .SetStyle(Control::States::checkedPressed, uppercase, small_special, Color::White, Color(0x494A63))
        .SetCheckable(true)
        .SetName("Shift");

    mBtnLetters.Setup(cSpecialLeft, big_special.GetBoundingRect({18, 236}), cKEY_LETTERS)
        .SetStyle(Control::States::normal, empty, big_special, Color::Black, Color::White)
        .SetStyle(Control::States::pressed, empty, big_special, Color::White, Color(0x494A63))
        .SetName("Letters");
    mBtnLetters.SetCaption("ABC").SetFontSize(22);

    mBtnNumbers.Setup(cSpecialLeft, big_special.GetBoundingRect({18, 236}), cKEY_NUMBERS)
        .SetStyle(Control::States::normal, empty, big_special, Color::Black, Color::White)
        .SetStyle(Control::States::pressed, empty, big_special, Color::White, Color(0x494A63))
        .SetName("Numbers");
    mBtnNumbers.SetCaption("123").SetFontSize(22);

    mBtnSpecials.Setup(cSpecialRight, big_special.GetBoundingRect({366, 236}), cKEY_SPECIALS)
        .SetStyle(Control::States::normal, empty, big_special, Color::Black, Color::White)
        .SetStyle(Control::States::pressed, empty, big_special, Color::White, Color(0x494A63))
        .SetName("Special");
    mBtnSpecials.SetCaption("+-/").SetFontSize(22);

    mBtnErase.Setup(Rect(374, 152, 80, 71), small_special.GetBoundingRect({390, 167}), '\b')
        .SetStyle(Control::States::normal, erase.SetDestination({9, 9}), small_special, Color(0x494A63), Color::White)
        .SetStyle(Control::States::pressed, erase, small_special, Color::White, Color(0x494A63))
        .SetName("Erase");

    mBtnSpace.Setup(Rect(98, 224, 258, 64), space.GetBoundingRect({106, 236}), ' ')
        .SetStyle(Control::States::normal, empty, space, Color(), Color::White)
        .SetStyle(Control::States::pressed, empty, space, Color(), Color(0x494A63))
        .SetName("Space");

    addBtn(mBtnShift);
    addBtn(mBtnLetters);
    addBtn(mBtnNumbers);
    addBtn(mBtnSpecials);
    addBtn(mBtnErase);
    addBtn(mBtnSpace);

    int index = 0;
    for(Key& arBtn : mKeys) {
        arBtn.Setup(cKeyTouchAreas[index], key.GetBoundingRect(cKeyPositions[index]))
            .SetStyle(Control::States::normal, empty, empty, Color::White, Color::Black)
            .SetStyle(Control::States::pressed, empty, key, Color(), Color(0x494A63));
        addBtn(arBtn);
        index++;
    }

//    mBtnShift.SetState(States::checked);
    SetLayout(LayoutType::Letters);
}

void Keyboard::addBtn(Key &arBtn)
{
    AddChild(&arBtn);
    arBtn.OnClick() = Method(this, &Keyboard::doKeyClick);
}

void Keyboard::setSymbols(const std::string &arSymbols)
{
    ASSERT(arSymbols.length() >= 26);
    bool checked = mBtnShift.IsChecked();
    std::u32string utf32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(arSymbols);
    unsigned int index = 0;
    for (char32_t symbol : utf32) {
        Button &btn = mKeys[index++];
        symbol = checked ? std::towupper(symbol) : std::towlower(symbol);
        btn.SetId(symbol);
        std::wstring ws(1, symbol);
        std::string utf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(ws);
        btn.SetCaption(utf8);
    }
}

Keyboard::~Keyboard()
{
}

void Keyboard::doKeyClick(const Point &arPoint, int aSymbol)
{
    switch(aSymbol) {
        case cKEY_SHIFT:
        case cKEY_LETTERS:
            SetLayout(LayoutType::Letters);
            break;

        case cKEY_NUMBERS:
            SetLayout(LayoutType::Numbers);
            break;

        case cKEY_SPECIALS:
            SetLayout(LayoutType::Special);
            break;

        case '\b':
            if (mInput.length() > 1) {
                mInput.pop_back();
                mOnKeyClick(mInput);
            }
            break;

        default:
            std::wstring ws(1, aSymbol);
            mInput += std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(ws);
            mOnKeyClick(mInput);
            break;
    }
}

void Keyboard::SetLayout(LayoutType aLayout)
{
    const char* cLetters = "qwertyuiopasdfghjklzxcvbnm";
    const char* cNumbers  = "1234567890-_:;()&@\".,?!'/*";
    const char* cSpecials = " []{}#%+~ =¤\\≈<>£€$.,?!'/*";

    switch (aLayout) {
        case LayoutType::Letters:
            setSymbols(std::string(cLetters));
            mBtnShift.Show();
            mBtnLetters.Hide();
            mBtnNumbers.Show();
            mBtnSpecials.Show();
            break;

        case LayoutType::Numbers:
            setSymbols(std::string(cNumbers));
            mBtnShift.Hide();
            mBtnLetters.Setup(cSpecialLeft, mBtnLetters.GetArea().MoveTo(Point(18, 236))).Show();
            mBtnNumbers.Hide();
            mBtnSpecials.Show();
            break;

        case LayoutType::Special:
            setSymbols(std::string(cSpecials));
            mBtnShift.Hide();
            mBtnLetters.Setup(cSpecialRight, mBtnLetters.GetArea().MoveTo(Point(366, 236))).Show();
            mBtnNumbers.Show();
            mBtnSpecials.Hide();
            break;
    }

}

Keyboard& Keyboard::SetInput(const std::string &arText)
{
    mInput = arText;
    Invalidate();
    mOnKeyClick(mInput);
    return *this;
}

} /* namespace rsp::graphics */

