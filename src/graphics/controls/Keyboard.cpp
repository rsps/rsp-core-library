/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/controls/Keyboard.h>
#include <cctype>
#include <functional>
#include <utils/Function.h>

using namespace rsp::utils;

namespace rsp::graphics {


Keyboard::Keyboard()
    : mImages("testImages/Keyboard.bmp")
{
    BitmapView lower_normal(&mImages, Rect({0, 0}, {63, 48}));
    BitmapView lower_pressed(&mImages, Rect({64, 0}, {127, 48}));
    BitmapView upper_normal(&mImages, Rect({128, 0}, {191, 48}));
    BitmapView upper_pressed(&mImages, Rect({192, 0}, {255, 48}));
    BitmapView erase_normal(&mImages, Rect({256, 0}, {319, 48}));
    BitmapView erase_pressed(&mImages, Rect({320, 0}, {383, 48}));
    BitmapView space_normal(&mImages, Rect({0, 49}, {335, 103}));
    BitmapView space_pressed(&mImages, Rect({0, 104}, {335, 159}));
    BitmapView numbers_normal(&mImages, Rect({337, 49}, {433, 103}));
    BitmapView numbers_pressed(&mImages, Rect({434, 49}, {530, 103}));
    BitmapView letters_normal(&mImages, Rect({337, 104}, {433, 159}));
    BitmapView letters_pressed(&mImages, Rect({434, 104}, {530, 159}));
    BitmapView special_normal(&mImages, Rect({337, 160}, {433, 215}));
    BitmapView special_pressed(&mImages, Rect({434, 160}, {530, 215}));

    // Special checked state for shift button
    Style style;
    style.mBitmapView = upper_normal;
    mBtnShift.GetStyle(Control::States::checked) = style;
    style.mBitmapView = upper_pressed;
    mBtnShift.GetStyle(Control::States::checkedPressed) = style;
    mBtnShift.SetCheckable(true);

    setupBtn(mBtnShift, Rect(14, 426, 78, 68), {31, 439}, lower_normal, lower_pressed, cKEY_SHIFT);
    setupBtn(mBtnLetters, cSpecialLeft, {31, 508}, letters_normal, letters_pressed, cKEY_LETTERS);
    setupBtn(mBtnNumbers, cSpecialLeft, {31, 508}, numbers_normal, numbers_pressed, cKEY_NUMBERS);
    setupBtn(mBtnSpecials, cSpecialRight, {379, 508}, special_normal, special_pressed, cKEY_SPECIALS);
    setupBtn(mBtnErase, Rect(388, 426, 78, 68), {403, 439}, erase_normal, erase_pressed, '\b');
    setupBtn(mBtnSpace, Rect(111, 496, 258, 64), {119, 508}, space_normal, space_pressed, ' ');
}

void Keyboard::setupBtn(uint32_t aBtnIndex, Rect aArea, Point aBitmapPosition)
{
    Button& arBtn = mKeys[aBtnIndex];

    BitmapView normal(&mImages, Rect({194, 160}, {249, 243}));
    BitmapView pressed(&mImages, Rect({249, 160}, {305, 243}));

    Style style;
    style.mBitmapView = normal;
    arBtn.GetStyle(Control::States::normal) = style;
    style.mBitmapView = pressed;
    arBtn.GetStyle(Control::States::pressed) = style;

    arBtn.SetArea(aArea)
        .SetBitmapPosition(aBitmapPosition)
        .SetTransparent(true);
    arBtn.SetId(aBtnIndex);

    AddChild(&arBtn);
    arBtn.OnClick() = Method(this, &Keyboard::doKeyClick);
}

void Keyboard::setupBtn(Button &arBtn, Rect aArea, Point aBitmapPosition, BitmapView &arNormal, BitmapView &arPressed, int aSymbol)
{
    Style style;
    style.mBitmapView = arNormal;
    arBtn.GetStyle(Control::States::normal) = style;
    style.mBitmapView = arPressed;
    arBtn.GetStyle(Control::States::pressed) = style;

    arBtn.SetArea(aArea)
        .SetBitmapPosition(aBitmapPosition)
        .SetTransparent(true);
    arBtn.SetCaption(std::string(1, aSymbol));
    arBtn.SetId(aSymbol);

    AddChild(&arBtn);
    arBtn.OnClick() = Method(this, &Keyboard::doKeyClick);
}

void Keyboard::setSymbols(const std::string &arSymbols)
{
    bool checked = mBtnShift.IsChecked();
    unsigned int index = 0;
    for (char symbol : arSymbols) {
        Button &btn = mKeys[index];
        symbol = checked ? std::toupper(symbol) : std::tolower(symbol);
        btn.SetId(symbol);
        btn.SetCaption(std::string(1, symbol));
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
            break;

        case cKEY_SPECIALS:
            break;

        case '\b':
            if (mInput.length() > 1) {
                mInput.pop_back();
                mOnKeyClick(mInput);
            }
            break;

        default:
            mInput.push_back(aSymbol);
            mOnKeyClick(mInput);
            break;
    }
}

void Keyboard::SetLayout(LayoutType aLayout)
{
    const char* cLetters  = "qwertyuiopasdfghjklzxcvbnm";
    const char* cNumbers  = "1234567890-_:;()&@\".,?!'/*";
    const char* cSpecials = " []{}#%+~ =¤\\=<>£€$.,?!'/*";

    switch (aLayout) {
        case LayoutType::Letters:
            setSymbols(std::string(cLetters));
            mBtnShift.Show();
            break;

        case LayoutType::Numbers:
            setSymbols(std::string(cNumbers));
            mBtnShift.Hide();
            break;

        case LayoutType::Special:
            setSymbols(std::string(cSpecials));
            mBtnShift.Hide();
            break;
    }
}

} /* namespace rsp::graphics */

