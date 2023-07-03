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
#include <string_view>
#include <graphics/Control.h>
#include <graphics/Text.h>
#include <graphics/Texture.h>

namespace rsp::graphics {

class Label : public Control
{
public:
    Label() { initTypeInfo<Label>(); }

    Label(const Label &arOther);
    Label(Label &&arOther) = default;

    Label& operator=(const Label &arOther);
    Label& operator=(Label &&arOther) = default;

    Label& operator<<(const std::string &arCaption) { return SetCaption(arCaption); }
    Label& operator<<(const std::string_view aCaption) { return SetCaption(std::string(aCaption)); }
    Label& operator<<(const char *apCaption) { return SetCaption(std::string(apCaption)); }
    Label& operator<<(Text::VAlign aVAlign) { return SetVAlignment(aVAlign); }
    Label& operator<<(Text::HAlign aHAlign) { return SetHAlignment(aHAlign); }

    Label& SetCaption(const std::string &arCaption);
    Label& SetVAlignment(Text::VAlign aVAlign);
    Label& SetHAlignment(Text::HAlign aHAlign);
    Label& SetFontSize(int aSizePx);
    Label& ScaleToFit(bool aValue);

    Text& GetText() { return mText; }

protected:
    Text mText{};
    TexturePtr_t mpTexture = nullptr;
    bool mScaleToFit = false;

    void update() override;
    void refresh() override;
    bool render(Renderer &arRenderer) const override;
};

} /* namespace rsp::graphics */

#endif // INCLUDE_GRAPHICS_LABEL_H
