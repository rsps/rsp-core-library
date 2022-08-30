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
#include <vector>
#include <graphics/primitives/BitmapView.h>
#include <graphics/primitives/Rect.h>
#include <utils/Function.h>
#include "Button.h"
#include "Control.h"

namespace rsp::graphics {

class Keyboard: public Control
{
public:
    class Layout: public Control
    {
    protected:
        std::vector<Button> mKeys{};
    };

    enum class LayoutType { None, LowerCase, UpperCase, Numbers, Special };

    Keyboard();
    ~Keyboard();

    std::map<LayoutType, Layout*>& GetLayouts() { return mLayouts; }

protected:
    std::map<LayoutType, Layout*> mLayouts{};
    LayoutType mCurrentLayout = LayoutType::None;
    rsp::utils::Function<void(int aSymbol)> mOnKeyClick;

    friend Keyboard::Layout;
    void doKeyClick(int aSymbol);
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_CONTROLS_KEYBOARD_H_ */
