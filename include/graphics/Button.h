/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_CONTROLS_BUTTON_H_
#define INCLUDE_GRAPHICS_CONTROLS_BUTTON_H_

#include <map>
#include <graphics/primitives/Bitmap.h>
#include <graphics/primitives/Text.h>
#include "Control.h"
#include "Label.h"

namespace rsp::graphics {

class Button: public Label
{
public:
    TYPEINFO(Button)
    Button() : Label(rsp::utils::MakeTypeInfo<Button>()) {}
    Button(const rsp::utils::TypeInfo &arInfo) : Label(arInfo) {};
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_CONTROLS_BUTTON_H_ */
