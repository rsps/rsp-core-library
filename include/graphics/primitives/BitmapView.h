/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_PRIMITIVES_BITMAPVIEW_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_BITMAPVIEW_H_

#include "Bitmap.h"
#include "Color.h"

namespace rsp::graphics {

class BitmapView
{
public:
    BitmapView() {};
    BitmapView(const Bitmap* apBitmap);
    BitmapView(const Bitmap* apBitmap, const Rect& arSection);
    BitmapView(const BitmapView &) = default;
    BitmapView(BitmapView &&) = default;

    BitmapView& operator=(const BitmapView&) = default;
    BitmapView& operator=(BitmapView&&) = default;

    BitmapView& SetBitmap(const Bitmap *apBitmap);
    const Bitmap* GetBitmap() const { return mpBitmap; }

    BitmapView& ClearSection();
    BitmapView& SetSection(const Rect &arSection);
    const Rect& GetSection() const { return mSection; }

    BitmapView& SetDestination(const Point &arPoint);
    const Point& GetDestination() const { return mDestination; }

    BitmapView& SetPixelColor(const Color &arColor);
    const Color& GetPixelColor() const { return mPixelColor; }

    GuiUnit_t GetWidth();
    GuiUnit_t GetHeight();

    Rect GetBoundingRect(Point aPoint = {0,0}) { return Rect(aPoint, GetWidth(), GetHeight()); }

    void Paint(const Point &arOffset, Canvas &arCanvas) const;

protected:
    Point mDestination{};
    Color mPixelColor = Color::White;
    Rect mSection{};
    const Bitmap* mpBitmap = nullptr;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_BITMAPVIEW_H_ */
