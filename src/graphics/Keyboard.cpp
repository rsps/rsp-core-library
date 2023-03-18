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
#include <graphics/Keyboard.h>
#include <utils/Function.h>
#include "pixmaps/BigSpecial.h"
#include "pixmaps/Erase.h"
#include "pixmaps/Key.h"
#include "pixmaps/LowerCase.h"
#include "pixmaps/SmallSpecial.h"
#include "pixmaps/Space.h"
#include "pixmaps/UpperCase.h"

using namespace rsp::utils;

namespace rsp::graphics {


Key::Key()
{
    initTypeInfo<Key>();
    mText.GetFont().SetSize(34);
    SetTransparent(true);
}

Key& Key::Setup(Rect aTouchArea, Rect aArea, Point aPosition, int aSymbol)
{
    SetArea(aArea.MoveTo(aPosition));
    SetTouchArea(aTouchArea);
    SetTexturePosition(aPosition);

    if (aSymbol) {
        if ((aSymbol < Keyboard::cKEY_SHIFT) && (aSymbol > ' ')) {
            std::cout << "Set key symbol: " << static_cast<char>(aSymbol) << " on TextArea(" << mText.GetBoundingRect() << ")" << std::endl;
            SetCaption(std::string(1, aSymbol));
        }
        TypeInfo::SetId(aSymbol);
    }
    return *this;
}

Key& Key::Background(Control::States aState, Color aColor, const PixelData *apPixels, Point aOffset)
{
    Style& style = GetStyle(aState);
    if (apPixels) {
        style.mTextures.push_back(Texture::Create(*apPixels, aColor));
        style.mTextures.back()->SetOffset(aOffset);
    }

    style.mBackgroundColor = aColor;
    return *this;
}

Key& Key::Foreground(Control::States aState, Color aColor, const PixelData *apPixels, Point aOffset)
{
    Style& style = GetStyle(aState);
    if (apPixels) {
        style.mTextures.push_back(Texture::Create(*apPixels, aColor));
        style.mTextures.back()->SetOffset(aOffset);
    }
    style.mForegroundColor = aColor;
    return *this;
}

static Rect cKeyTouchAreas[26] = {
    {_Q, 59, 83}, // Q
    {_W, 42, 83}, // W
    {_E, 42, 83}, // E
    {_R, 42, 83}, // R
    {_T, 42, 83}, // T
    {_Y, 42, 83}, // Y
    {_U, 42, 83}, // U
    {_I, 42, 83}, // I
    {_O, 42, 83}, // O
    {_P, 59, 83}, // P
    {_A, 80, 69}, // A
    {_S, 42, 69}, // S
    {_D, 42, 69}, // D
    {_F, 42, 69}, // F
    {_G, 42, 69}, // G
    {_H, 42, 69}, // H
    {_J, 42, 69}, // J
    {_K, 42, 69}, // K
    {_L, 80, 69}, // L
    {_Z, 42, 71}, // Z
    {_X, 42, 71}, // X
    {_C, 42, 71}, // C
    {_V, 42, 71}, // V
    {_B, 42, 71}, // B
    {_N, 42, 71}, // N
    {_M, 42, 71}  // M
};

static Point cKeyPositions[26] = {
    { 18,  14},  // Q
    { 60,  14},  // W
    {102,  14},  // E
    {144,  14},  // R
    {186,  14},  // T
    {228,  14},  // Y
    {270,  14},  // U
    {312,  14},  // I
    {354,  14},  // O
    {396,  14},  // P
    { 39,  84},  // A
    { 81,  84},  // S
    {123,  84},  // D
    {165,  84},  // F
    {207,  84},  // G
    {249,  84},  // H
    {291,  84},  // J
    {333,  84},  // K
    {375,  84},  // L
    { 81, 154},  // Z
    {123, 154},  // X
    {165, 154},  // C
    {207, 154},  // V
    {249, 154},  // B
    {291, 154},  // N
    {333, 154}   // M
};

Keyboard::Keyboard()
{
    initTypeInfo<Keyboard>();
    SetTransparent(true);

    mBtnShift
        .Background(Control::States::Normal, Color::White, &cSmallSpecial)
        .Foreground(Control::States::Normal, Color(0x494A63), &cLowerCase, {11, 7})
        .Background(Control::States::Pressed, Color(0x494A63), &cSmallSpecial)
        .Foreground(Control::States::Pressed, Color::White, &cLowerCase, {11, 7})
        .Background(Control::States::Checked, Color::White, &cSmallSpecial)
        .Foreground(Control::States::Checked, Color(0x494A63), &cUpperCase, {11, 3})
        .Background(Control::States::CheckedPressed, Color(0x494A63), &cSmallSpecial)
        .Foreground(Control::States::CheckedPressed, Color::White, &cUpperCase, {11, 3})
        .Setup(Rect(0, 152, 80, 71), cSmallSpecial.GetRect(), {18, 167}, cKEY_SHIFT)
        .SetCheckable(true)
        .SetName("Shift");

    mBtnLetters
        .Background(Control::States::Normal, Color::White, &cBigSpecial)
        .Foreground(Control::States::Normal, Color::Black)
        .Background(Control::States::Pressed, Color(0x494A63), &cBigSpecial)
        .Foreground(Control::States::Pressed, Color::White)
        .Setup(cSpecialLeft, cBigSpecial.GetRect(), {18, 236}, cKEY_LETTERS)
        .SetName("Letters");
    mBtnLetters.SetCaption("ABC").SetFontSize(22);

    mBtnNumbers
        .Background(Control::States::Normal, Color::White, &cBigSpecial)
        .Foreground(Control::States::Normal, Color::Black)
        .Background(Control::States::Pressed, Color(0x494A63), &cBigSpecial)
        .Foreground(Control::States::Pressed, Color::White)
        .Setup(cSpecialLeft, cBigSpecial.GetRect(), {18, 236}, cKEY_NUMBERS)
        .SetName("Numbers");
    mBtnNumbers.SetCaption("123").SetFontSize(22);

    mBtnSpecials
        .Background(Control::States::Normal, Color::White, &cBigSpecial)
        .Foreground(Control::States::Normal, Color::Black)
        .Background(Control::States::Pressed, Color(0x494A63), &cBigSpecial)
        .Foreground(Control::States::Pressed, Color::White)
        .Setup(cSpecialRight, cBigSpecial.GetRect(), {366, 236}, cKEY_SPECIALS)
        .SetName("Special");
    mBtnSpecials.SetCaption("+*#").SetFontSize(22);

    mBtnErase
        .Background(Control::States::Normal, Color::White, &cSmallSpecial)
        .Foreground(Control::States::Normal, Color(0x494A63), &cErase, {9, 9})
        .Background(Control::States::Pressed, Color(0x494A63), &cSmallSpecial)
        .Foreground(Control::States::Pressed, Color::White, &cErase, {9, 9})
        .Setup(Rect(374, 152, 80, 71), cSmallSpecial.GetRect(), {390, 167}, '\b')
        .SetName("Erase");

    mBtnSpace
        .Background(Control::States::Normal, Color::White, &cSpace)
        .Foreground(Control::States::Normal, Color::None)
        .Background(Control::States::Pressed, Color(0x494A63), &cSpace)
        .Foreground(Control::States::Pressed, Color::None)
        .Setup(Rect(98, 224, 258, 64), cSpace.GetRect(), {106, 236}, ' ')
        .SetName("Space");

    addBtn(mBtnShift);
    addBtn(mBtnLetters);
    addBtn(mBtnNumbers);
    addBtn(mBtnSpecials);
    addBtn(mBtnErase);
    addBtn(mBtnSpace);

    int index = 0;
    for(Key& arBtn : mKeys) {
        arBtn.Setup(cKeyTouchAreas[index], cKey.GetRect(), cKeyPositions[index])
            .Background(Control::States::Normal, Color::None)
            .Foreground(Control::States::Normal, Color::White)
            .Background(Control::States::Pressed, Color(0x494A63), &cKey, cKeyPositions[index])
            .Foreground(Control::States::Pressed, Color::White);
        addBtn(arBtn);
        index++;
    }

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
        btn.SetId(uint32_t(symbol));
        std::wstring ws(1, wchar_t(symbol));
        std::string utf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(ws);
        btn.SetCaption(utf8).SetName(utf8);
    }
}

Keyboard::~Keyboard()
{
}

void Keyboard::doKeyClick(const Point &arPoint, uint32_t aSymbol)
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
            std::wstring ws(1, wchar_t(aSymbol));
            std::string utf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(ws);
            mInput += utf8;
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
            mBtnLetters.Setup(cSpecialLeft, cBigSpecial.GetRect(), Point(18, 236) + GetOrigin()).Show();
            mBtnNumbers.Hide();
            mBtnSpecials.Show();
            break;

        case LayoutType::Special:
            setSymbols(std::string(cSpecials));
            mBtnShift.Hide();
            mBtnLetters.Setup(cSpecialRight, cBigSpecial.GetRect(), Point(366, 236) + GetOrigin()).Show();
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

