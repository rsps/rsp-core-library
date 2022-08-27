/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_CONTROLS_STYLE_H_
#define INCLUDE_GRAPHICS_CONTROLS_STYLE_H_

#include <graphics/primitives/BitmapView.h>
#include <graphics/primitives/Color.h>

namespace rsp::graphics {

class Style
{
public:
    Style() {};
    Style(const Style &arOther) = default;
    Style(Style &&) = default;
    Style& operator=(const Style &) = default;
    Style& operator=(Style &&) = default;

    Color mForegroundColor = Color::White;
    Color mBackgroundColor = Color::Black;
    BitmapView mBitmapView{};
};


} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_CONTROLS_STYLE_H_ */
