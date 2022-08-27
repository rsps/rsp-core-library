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

    BitmapView& ClearSection();
    BitmapView& SetSection(const Rect &arSection);

    BitmapView& SetDestination(const Point &arPoint);
    BitmapView& SetPixelColor(const Color &arColor);

    void Paint(Canvas &arCanvas) const;

protected:
    Point mDestination{};
    Color mPixelColor = Color::White;
    Rect mSection{};
    const Bitmap* mpBitmap = nullptr;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_BITMAPVIEW_H_ */
