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
    style.mBackground = arBackground.SetPixelColor(aBackColor);
    GetStyle(aState) = style;
    return *this;
}

//void Key::paint(Canvas &arCanvas, const Style &arStyle)
//{
////    std::cout << "Paint bitmap view on " << GetName() << " Area(" << mArea << ") TextArea(" << mText.GetArea() << ")" << std::endl;
//    arCanvas.DrawRectangle(mTouchArea, Color::Yellow);
//    arStyle.mBackground.Paint(GetOrigin(), arCanvas);
//    arStyle.mForeground.Paint(GetOrigin(), arCanvas);
//    arCanvas.DrawText(mText, arStyle.mForegroundColor);
//}

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
    : Control(rsp::utils::MakeTypeInfo<Keyboard>())
{
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

    mBtnShift.Setup(Rect(0, 152, 78, 70), small_special.GetBoundingRect({18, 167}), cKEY_SHIFT)
        .SetStyle(Control::States::normal, lowercase.SetDestination({11, 7}), small_special, Color(0x494A63), Color::White)
        .SetStyle(Control::States::pressed, lowercase, small_special, Color::White, Color(0x494A63))
        .SetStyle(Control::States::checked, uppercase.SetDestination({11, 7}), small_special, Color(0x494A63), Color::White)
        .SetStyle(Control::States::checkedPressed, uppercase, small_special, Color::White, Color(0x494A63))
        .SetCheckable(true)
        .SetName("Shift");

    mBtnLetters.Setup(cSpecialLeft, big_special.GetBoundingRect({18, 236}), cKEY_LETTERS)
        .SetStyle(Control::States::normal, empty, big_special, Color(), Color::White)
        .SetStyle(Control::States::pressed, empty, big_special, Color(), Color(0x494A63))
        .SetName("Letters");

    mBtnNumbers.Setup(cSpecialLeft, big_special.GetBoundingRect({18, 236}), cKEY_NUMBERS)
        .SetStyle(Control::States::normal, empty, big_special, Color(), Color::White)
        .SetStyle(Control::States::pressed, empty, big_special, Color(), Color(0x494A63))
        .SetName("Numbers");

    mBtnSpecials.Setup(cSpecialRight, big_special.GetBoundingRect({366, 236}), cKEY_SPECIALS)
        .SetStyle(Control::States::normal, empty, big_special, Color(), Color::White)
        .SetStyle(Control::States::pressed, empty, big_special, Color(), Color(0x494A63))
        .SetName("Special");

    mBtnErase.Setup(Rect(375, 152, 79, 70), small_special.GetBoundingRect({390, 167}), '\b')
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
            .SetStyle(Control::States::normal, empty, empty, Color(), Color::White)
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

