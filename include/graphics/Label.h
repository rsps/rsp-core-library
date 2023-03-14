/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_LABEL_H
#define INCLUDE_GRAPHICS_LABEL_H

#include <string>
#include <graphics/Control.h>
#include <graphics/Text.h>
#include <graphics/Texture.h>

namespace rsp::graphics {

class Label : public Control
{
public:
    Label() { initTypeInfo<Label>(); }

    Label& SetCaption(const std::string &arCaption);
    Label& SetTextPosition(const Point &arPoint);
    Label& SetTextArea(const Rect &arArea);
    Label& SetFontSize(int aSizePx);
    Label& SetVAlignment(Text::VAlign aVAlign);
    Label& SetHAlignment(Text::HAlign aHAlign);

    Text& GetText() { return mText; }

    Control& SetOrigin(const Point &arPoint) override;

protected:
    Text mText{};
    Point mTextOrigin{};

    void paint(Canvas &arCanvas, const Style &arStyle) override;
    void refresh() override;
};

} /* namespace rsp::graphics */

#endif // INCLUDE_GRAPHICS_LABEL_H
