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

namespace rsp::graphics {

class Button: public Control
{
public:
    Button();
    Button(const rsp::utils::TypeInfo &arInfo);

    Button& SetArea(const Rect &arRect) override;

    Text& GetText() { return mText; }
    Button& SetBitmapPosition(const Point &arPoint);
    Button& SetTextPosition(const Point &arPoint);

protected:
    Text mText{};

    void paint(Canvas &arCanvas, const Style &arStyle) override;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_CONTROLS_BUTTON_H_ */
