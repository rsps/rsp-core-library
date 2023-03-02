/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_CONTROLS_LABEL_H
#define INCLUDE_GRAPHICS_CONTROLS_LABEL_H

#include <string>
#include <graphics/Control.h>
#include <graphics/Text.h>

namespace rsp::graphics {

class Label : public Control
{
public:
    TYPEINFO(Label)

    Label() : Control(rsp::utils::MakeTypeInfo<Label>()) {}
    Label(const rsp::utils::TypeInfo &arInfo) : Control(arInfo) {};

    Label& SetCaption(const std::string &arCaption);
    Label& SetTextPosition(const Point &arPoint);
    Label& SetFontSize(int aSizePx);
    Label& SetVAlignment(Text::VAlign aVAlign);
    Label& SetHAlignment(Text::HAlign aHAlign);

    Text& GetText() { return mText; }

    Control& SetOrigin(const Point &arPoint) override;

protected:
    Text mText{};

    void paint(Canvas &arCanvas, const Style &arStyle) override;
    void refresh() override;
    void doSetArea(const Rect &arRect) override;
};

} // namespace rsp::graphics

#endif // INCLUDE_GRAPHICS_CONTROLS_LABEL_H
