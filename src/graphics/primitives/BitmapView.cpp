/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <algorithm>
#include <graphics/primitives/BitmapView.h>

namespace rsp::graphics {

BitmapView::BitmapView(const Bitmap* apBitmap)
    : mpBitmap(apBitmap)
{
    SetSection(Rect(0u, 0u, apBitmap->GetWidth(), apBitmap->GetHeight()));
}

BitmapView::BitmapView(const Bitmap *apBitmap, const Rect &arSection)
    : mpBitmap(apBitmap)
{
    SetSection(arSection);
}

BitmapView& BitmapView::SetBitmap(const Bitmap *apBitmap)
{
    mpBitmap = apBitmap;
    ClearSection();
    return *this;
}

BitmapView& BitmapView::ClearSection()
{
    if (mpBitmap) {
        mSection = Rect(0u, 0u, mpBitmap->GetWidth(), mpBitmap->GetHeight());
    }
    else {
        mSection = Rect();
    }
    return *this;
}

BitmapView& BitmapView::SetSection(const Rect &arSection)
{
    if (mpBitmap) {
        mSection = Rect(
            std::min(0u, static_cast<unsigned int>(arSection.GetTop())),
            std::min(0u, static_cast<unsigned int>(arSection.GetLeft())),
            std::min(mpBitmap->GetWidth(), static_cast<unsigned int>(arSection.GetWidth())),
            std::min(mpBitmap->GetHeight(), static_cast<unsigned int>(arSection.GetHeight()))
        );
    }
    else {
        mSection = Rect();
    }
    return *this;
}

BitmapView& BitmapView::SetDestination(const Point &arPoint)
{
    mDestination = arPoint;
    return *this;
}

BitmapView& BitmapView::SetPixelColor(const Color &arColor)
{
    mPixelColor = arColor;
    return *this;
}

void BitmapView::Paint(Canvas &arCanvas) const
{
    if (!mpBitmap) {
        return;
    }
    arCanvas.DrawImageSection(mDestination, *mpBitmap, mSection, mPixelColor);
}

} /* namespace rsp::graphics */
