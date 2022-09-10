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

class Key : public Button
{
public:
    TYPEINFO(Key)
    Key();
    Key& Setup(Rect aTouchArea, Rect aArea, int aSymbol = 0);
    Key& SetStyle(Control::States aState, BitmapView &arForeground, BitmapView &arBackground, Color aFrontColor, Color aBackColor);
};

class Keyboard: public Control
{
public:
    TYPEINFO(Keyboard)

    using KeyboardCallback_t = rsp::utils::Function<void(std::string &)>;

    static constexpr int cKEY_SHIFT    = 1000000;
    static constexpr int cKEY_LETTERS  = 1000001;
    static constexpr int cKEY_NUMBERS  = 1000002;
    static constexpr int cKEY_SPECIALS = 1000003;

    const Rect cSpecialLeft = {0, 224, 96, 64};
    const Rect cSpecialRight = {358, 224, 96, 64};

    enum class LayoutType { Letters, Numbers, Special };
    enum class ImageIds { SmallSpecial, BigSpecial, Space, Key, LowerCase, UpperCase, Erase };

    Keyboard();
    ~Keyboard();

    void SetLayout(LayoutType aLayout);

    KeyboardCallback_t& OnKeyClick() { return mOnKeyClick; }
protected:
    KeyboardCallback_t mOnKeyClick;
    std::map<ImageIds, Bitmap> mImages{};
    std::array<Key, 26u> mKeys{};
    Key mBtnShift;
    Key mBtnLetters;
    Key mBtnNumbers;
    Key mBtnSpecials;
    Key mBtnErase;
    Key mBtnSpace;
    std::string mInput{};

    void addBtn(Key &arBtn);
    void setSymbols(const std::string &arSymbols);
    void doKeyClick(const Point &arPoint, int aSymbol);
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_CONTROLS_KEYBOARD_H_ */
