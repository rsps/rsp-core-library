/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_BITMAPVIEW_H_
#define INCLUDE_GRAPHICS_BITMAPVIEW_H_

#include <graphics/Bitmap.h>
#include <graphics/Color.h>
#include <graphics/PixelData.h>

namespace rsp::graphics {

class BitmapView
{
public:
    BitmapView() = default;
    explicit BitmapView(const PixelData &arPixelData);
    BitmapView(const PixelData &arPixelData, const Rect& arSection);

    BitmapView(const BitmapView &) = default;
    BitmapView(BitmapView &&) noexcept = default;

    BitmapView& operator=(const BitmapView&) = default;
    BitmapView& operator=(BitmapView&&) = default;

    BitmapView& SetPixelData(const PixelData &arPixelData);
    [[nodiscard]] const PixelData& GetPixelData() const;

    BitmapView& ClearSection();
    BitmapView& SetSection(const Rect &arSection);
    [[nodiscard]] const Rect& GetSection() const { return mSection; }

    BitmapView& SetPixelColor(const Color &arColor);
    [[nodiscard]] const Color& GetPixelColor() const { return mPixelColor; }

    GuiUnit_t GetWidth();
    GuiUnit_t GetHeight();

    Rect GetBoundingRect(Point aPoint = {0,0}) { return {aPoint, GetWidth(), GetHeight()}; }

protected:
    Color mPixelColor = Color::White;
    Rect mSection{};
    const PixelData* mpPixelData = nullptr;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_BITMAPVIEW_H_ */
