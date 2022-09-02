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


Key::Key()
{
    mText.GetFont().SetSize(34);
}

void Key::Setup(Rect aTouchArea, Point aPosition, BitmapView &arBackground, BitmapView &arForeground, int aSymbol)
{
    SetArea(Rect(aPosition.GetX(), aPosition.GetY(), static_cast<int>(arBackground.GetBitmap()->GetWidth()), static_cast<int>(arBackground.GetBitmap()->GetHeight())));
    SetTouchArea(aTouchArea);

    Style style;
    style.mBackground = arBackground.SetPixelColor(Color::White);
    style.mForeground = arForeground.SetPixelColor(Color(0x494A63));
    GetStyle(Control::States::normal) = style;
    style.mBackground = arBackground.SetPixelColor(Color(0x494A63));
    style.mForeground = arForeground.SetPixelColor(Color::White);
    GetStyle(Control::States::pressed) = style;
    if (aSymbol) {
        if ((aSymbol < Keyboard::cKEY_SHIFT) && (aSymbol > ' ')) {
            std::cout << "Set key symbol: " << static_cast<char>(aSymbol) << " on TextArea(" << mText.GetArea() << ")" << std::endl;
            SetCaption(std::string(1, aSymbol));
        }
        SetId(aSymbol);
    }
}

void Key::paint(Canvas &arCanvas, const Style &arStyle)
{
    std::cout << "Paint bitmap view on " << GetName() << "Area(" << mArea << ") TextArea(" << mText.GetArea() << ")" << std::endl;
    arCanvas.DrawRectangle(mTouchArea, Color::Yellow);
    arStyle.mBackground.Paint(GetOrigin(), arCanvas);
    arStyle.mForeground.Paint(GetOrigin(), arCanvas);
    arCanvas.DrawText(mText, arStyle.mForegroundColor);
}

static Rect cKeyTouchAreas[26] = {
    {0, 0, 58, 82},
    {59, 0, 41, 82},
    {101, 0, 41, 82},
    {143, 0, 41, 82},
    {185, 0, 41, 82},
    {227, 0, 41, 82},
    {269, 0, 41, 82},
    {311, 0, 41, 82},
    {353, 0, 41, 82},
    {395, 0, 59, 82},
    {0,   83, 79, 68},
    {80,  83, 41, 68},
    {122, 83, 41, 68},
    {164, 83, 41, 68},
    {206, 83, 41, 68},
    {248, 83, 41, 68},
    {290, 83, 41, 68},
    {332, 83, 41, 68},
    {374, 83, 80, 68},
    {80,  152, 41, 70},
    {122, 152, 41, 70},
    {164, 152, 41, 70},
    {206, 152, 41, 70},
    {248, 152, 41, 70},
    {290, 152, 41, 70},
    {332, 152, 41, 70}
};

static Point cKeyPositions[26] = {
    {18, 14},
    {60, 14},
    {102, 14},
    {144, 14},
    {186, 14},
    {228, 14},
    {270, 14},
    {312, 14},
    {354, 14},
    {396, 14},
    {39, 84},
    {81, 84},
    {123, 84},
    {165, 84},
    {207, 84},
    {249, 84},
    {291, 84},
    {333, 84},
    {375, 84},
    {81, 154},
    {123, 154},
    {165, 154},
    {207, 154},
    {249, 154},
    {291, 154},
    {333, 154}
};

Keyboard::Keyboard()
{
    mImages[ImageIds::BigSpecial].Load("testImages/alpha/BigSpecial.bmp");
    mImages[ImageIds::SmallSpecial].Load("testImages/alpha/SmallSpecial.bmp");
    mImages[ImageIds::Space].Load("testImages/alpha/Space.bmp");
    mImages[ImageIds::Erase].Load("testImages/alpha/Erase.bmp");
    mImages[ImageIds::Key].Load("testImages/alpha/Key.bmp");
    mImages[ImageIds::LowerCase].Load("testImages/alpha/LowerCase.bmp");
    mImages[ImageIds::UpperCase].Load("testImages/alpha/UpperCase.bmp");

    BitmapView big_special(&mImages[ImageIds::BigSpecial]);
    BitmapView small_special(&mImages[ImageIds::SmallSpecial]);
    BitmapView space(&mImages[ImageIds::Space]);
    BitmapView erase(&mImages[ImageIds::Erase]);
    BitmapView key(&mImages[ImageIds::Key]);
    BitmapView lowercase(&mImages[ImageIds::LowerCase]);
    BitmapView uppercase(&mImages[ImageIds::UpperCase]);
    BitmapView empty;

    // Special checked state for shift button
    Style style;
    style.mBackground = small_special.SetPixelColor(Color::White);
    style.mForeground = uppercase.SetPixelColor(Color(0x494A63));
    mBtnShift.GetStyle(Control::States::checked) = style;
    style.mBackground = small_special.SetPixelColor(Color(0x494A63));
    style.mForeground = uppercase.SetPixelColor(Color::White);
    mBtnShift.GetStyle(Control::States::checkedPressed) = style;
    mBtnShift.SetCheckable(true);

    mBtnShift.SetName("Shift");
    mBtnLetters.SetName("Letters");
    mBtnNumbers.SetName("Numbers");
    mBtnSpecials.SetName("Special");
    mBtnErase.SetName("Erase");
    mBtnSpace.SetName("Space");

    setupBtn(mBtnShift, Rect(0, 153, 78, 69), {18, 167}, small_special, lowercase, cKEY_SHIFT);
    setupBtn(mBtnLetters, cSpecialLeft, {18, 236}, big_special, empty, cKEY_LETTERS);
    setupBtn(mBtnNumbers, cSpecialLeft, {18, 236}, big_special, empty, cKEY_NUMBERS);
    setupBtn(mBtnSpecials, cSpecialRight, {366, 236}, big_special, empty, cKEY_SPECIALS);
    setupBtn(mBtnErase, Rect(375, 153, 78, 69), {390, 167}, small_special, erase, '\b');
    setupBtn(mBtnSpace, Rect(98, 224, 258, 64), {106, 236}, space, empty, ' ');


    int index = 0;
    for(Key& arBtn : mKeys) {
        setupBtn(arBtn, cKeyTouchAreas[index], cKeyPositions[index], key, empty);
        index++;
    }

//    mBtnShift.SetState(States::checked);
    SetLayout(LayoutType::Letters);
}

void Keyboard::setupBtn(Key &arBtn, Rect aTouchArea, Point aBitmapPosition, BitmapView &arNormal, BitmapView &arPressed, int aSymbol)
{
    arBtn.Setup(aTouchArea, aBitmapPosition, arNormal, arPressed, aSymbol);

    AddChild(&arBtn);
    arBtn.OnClick() = Method(this, &Keyboard::doKeyClick);
}

void Keyboard::setSymbols(const std::string &arSymbols)
{
    ASSERT(arSymbols.length() == 26);
    bool checked = mBtnShift.IsChecked();
    unsigned int index = 0;
    for (char symbol : arSymbols) {
        Button &btn = mKeys[index++];
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

