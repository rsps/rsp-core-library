/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_CONTROLS_KEYBOARD_H_
#define INCLUDE_GRAPHICS_CONTROLS_KEYBOARD_H_

#include <map>
#include <array>
#include <graphics/primitives/Bitmap.h>
#include <graphics/primitives/BitmapView.h>
#include <graphics/primitives/Color.h>
#include <graphics/primitives/Point.h>
#include <graphics/primitives/Rect.h>
#include "Button.h"
#include "Control.h"

namespace rsp::graphics {

class Keyboard: public Control
{
public:
    using KeyboardCallback_t = rsp::utils::Function<void(std::string &)>;

    static constexpr int cKEY_SHIFT    = 1000000;
    static constexpr int cKEY_LETTERS  = 1000001;
    static constexpr int cKEY_NUMBERS  = 1000002;
    static constexpr int cKEY_SPECIALS = 1000003;

    const Rect cSpecialLeft = {13, 496, 96, 64};
    const Rect cSpecialRight = {371, 496, 96, 64};

    enum class LayoutType { Letters, Numbers, Special };

    Keyboard();
    ~Keyboard();

    void SetLayout(LayoutType aLayout);

    KeyboardCallback_t& OnKeyClick() { return mOnKeyClick; }
protected:
    KeyboardCallback_t mOnKeyClick;
    Bitmap mImages;
    std::array<Button, 26u> mKeys{};
    Button mBtnShift;
    Button mBtnLetters;
    Button mBtnNumbers;
    Button mBtnSpecials;
    Button mBtnErase;
    Button mBtnSpace;
    std::string mInput{};

    void setupBtn(uint32_t aBtnIndex, Rect aArea, Point aBitmapPosition);
    void setupBtn(Button &arBtn, Rect aArea, Point aBitmapPosition, BitmapView &arNormal, BitmapView &arPressed, int aSymbol);
    void setSymbols(const std::string &arSymbols);
    void doKeyClick(const Point &arPoint, int aSymbol);
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_CONTROLS_KEYBOARD_H_ */
