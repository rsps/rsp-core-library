/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_BUTTON_H_
#define INCLUDE_GRAPHICS_BUTTON_H_

#include <map>
#include "Bitmap.h"
#include "Control.h"
#include "Label.h"
#include "Text.h"
#include <utils/OptionalPtr.h>

namespace rsp::graphics {

class Button: public Label
{
public:
    using Optional_t = utils::OptionalPtr<PixelData>;

    Button() { initTypeInfo<Button>(); }

    Button& Setup(Rect aTouchArea, Rect aArea, Point aPosition);
    Button& Background(Control::States aState, Color aColor, Optional_t apPixelData = nullptr, Point aOffset = {0,0});
    Button& Foreground(Control::States aState, Color aColor, Optional_t apPixelData = nullptr, Point aOffset = {0,0});
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_BUTTON_H_ */
