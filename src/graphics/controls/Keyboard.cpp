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


void Key::Setup(Rect aTouchArea, Point aBitmapPosition, BitmapView &arNormal, BitmapView &arPressed, int aSymbol)
{
    SetArea(Rect(aBitmapPosition.GetX(), aBitmapPosition.GetY(), static_cast<int>(arNormal.GetBitmap()->GetWidth()), static_cast<int>(arNormal.GetBitmap()->GetHeight())));
    SetTouchArea(aTouchArea);

    Style style;
    style.mBitmapView = arNormal;
    GetStyle(Control::States::normal) = style;
    style.mBitmapView = arPressed;
    GetStyle(Control::States::pressed) = style;
    if (aSymbol) {
        SetCaption(std::string(1, aSymbol));
        SetId(aSymbol);
    }
}

void Key::paint(Canvas &arCanvas, const Style &arStyle)
{
    std::cout << "Paint bitmap view on " << GetName() << " (" << arStyle.mBitmapView.GetSection() << ") at " << arStyle.mBitmapView.GetDestination() << std::endl;
    arStyle.mBitmapView.Paint(GetOrigin(), arCanvas);
    mForeground.Paint(GetOrigin(), arCanvas);
    arCanvas.DrawText(mText, arStyle.mForegroundColor);
}



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

    mBtnShift.SetName("Shift");
    mBtnLetters.SetName("Letters");
    mBtnNumbers.SetName("Numbers");
    mBtnSpecials.SetName("Special");
    mBtnErase.SetName("Erase");
    mBtnSpace.SetName("Space");

    setupBtn(mBtnShift, Rect(0, 154, 78, 68), {18, 167}, lower_normal, lower_pressed, cKEY_SHIFT);
    setupBtn(mBtnLetters, cSpecialLeft, {18, 236}, letters_normal, letters_pressed, cKEY_LETTERS);
    setupBtn(mBtnNumbers, cSpecialLeft, {18, 236}, numbers_normal, numbers_pressed, cKEY_NUMBERS);
    setupBtn(mBtnSpecials, cSpecialRight, {366, 236}, special_normal, special_pressed, cKEY_SPECIALS);
    setupBtn(mBtnErase, Rect(375, 154, 78, 68), {390, 167}, erase_normal, erase_pressed, '\b');
    setupBtn(mBtnSpace, Rect(98, 224, 258, 64), {106, 236}, space_normal, space_pressed, ' ');

    SetLayout(LayoutType::Letters);
}

void Keyboard::setupBtn(uint32_t aBtnIndex, Rect aTouchArea, Point aBitmapPosition)
{
    Key& arBtn = mKeys[aBtnIndex];

    BitmapView normal(&mImages, Rect({194, 160}, {249, 243}));
    BitmapView pressed(&mImages, Rect({249, 160}, {305, 243}));

    arBtn.Setup(aTouchArea, aBitmapPosition, normal, pressed);

    AddChild(&arBtn);
    arBtn.OnClick() = Method(this, &Keyboard::doKeyClick);
}

void Keyboard::setupBtn(Key &arBtn, Rect aTouchArea, Point aBitmapPosition, BitmapView &arNormal, BitmapView &arPressed, int aSymbol)
{

    arBtn.Setup(aTouchArea, aBitmapPosition, arNormal, arPressed, aSymbol);

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
            mBtnLetters.Hide();
            mBtnNumbers.Show();
            mBtnSpecials.Show();
            break;

        case LayoutType::Numbers:
            setSymbols(std::string(cNumbers));
            mBtnShift.Hide();
            mBtnLetters.Show();
            mBtnNumbers.Hide();
            mBtnSpecials.Show();
            break;

        case LayoutType::Special:
            setSymbols(std::string(cSpecials));
            mBtnShift.Hide();
            mBtnLetters.Show();
            mBtnNumbers.Hide();
            mBtnSpecials.Hide();
            break;
    }

}

} /* namespace rsp::graphics */

