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

#include <graphics/controls/TouchControl.h>
#include <map>
#include <graphics/primitives/Bitmap.h>
#include <graphics/primitives/Text.h>
#include "Image.h"

namespace rsp::graphics {

class Button: public TouchControl
{
public:
    Button();
    Button(const Rect &arArea);

    Text& GetText() { return mText; }
    Image& GetImage() { return mImage; }
    Button& SetBitmapPosition(const Point &arPoint);
    Button& SetTextPosition(const Point &arPoint);

protected:
    Text mText{};
    Image mImage{};
    Point mBitmapPosition{};

    void paint(Canvas &arCanvas, const Style &arStyle) override;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_CONTROLS_BUTTON_H_ */
