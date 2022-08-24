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

#include <graphics/controls/TouchControl.h>
#include "Control.h"
#include <utils/EnumFlags.h>

namespace rsp::graphics {

class Key
{
public:
    TouchControl mTouchArea;
    int mSymbol;
    bool checked;
    Bitmap *mpNormalBmp;
    Bitmap *mpPressedBmp;
    Bitmap *mpCheckedBmp;
};

class Keyboard: public Control
{
public:
    enum class Flags : std::uint32_t { Shift = 1 };

    Keyboard() : Control() {}
    Keyboard(const Rect &arRect) : Control(arRect) {}

protected:
    std::array<Key, 10> mKeys;
    rsp::utils::EnumFlags<Flags> mFlags;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_CONTROLS_KEYBOARD_H_ */
