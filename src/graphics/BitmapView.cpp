/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022-2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/BitmapView.h>
#include <exceptions/CoreException.h>

namespace rsp::graphics {

BitmapView::BitmapView(const PixelData &arPixelData)
    : mpPixelData(&arPixelData)
{
    SetSection(Rect(0u, 0u, mpPixelData->GetWidth(), mpPixelData->GetHeight()));
}

BitmapView::BitmapView(const PixelData &arPixelData, const Rect &arSection)
    : mpPixelData(&arPixelData)
{
    SetSection(arSection);
}

BitmapView& BitmapView::SetPixelData(const PixelData &arPixelData)
{
    mpPixelData = &arPixelData;
    ClearSection();
    return *this;
}

const PixelData& BitmapView::GetPixelData() const
{
    if (!mpPixelData) {
        THROW_WITH_BACKTRACE1(exceptions::NotSetException, "PixelData not assigned to Image control");
    }
    return *mpPixelData;
}

BitmapView& BitmapView::ClearSection()
{
    if (mpPixelData) {
        mSection = Rect(0u, 0u, mpPixelData->GetWidth(), mpPixelData->GetHeight());
    }
    else {
        mSection = Rect();
    }
    return *this;
}

BitmapView& BitmapView::SetSection(const Rect &arSection)
{
    if (mpPixelData) {
        mSection = Rect(0u, 0u, mpPixelData->GetWidth(), mpPixelData->GetHeight()) & arSection;
    }
    else {
        mSection = Rect();
    }
    return *this;
}

BitmapView& BitmapView::SetPixelColor(const Color &arColor)
{
    mPixelColor = arColor;
    return *this;
}

GuiUnit_t BitmapView::GetWidth()
{
    return (!mpPixelData) ? 0 : mpPixelData->GetWidth();
}

GuiUnit_t BitmapView::GetHeight()
{
    return (!mpPixelData) ? 0 : mpPixelData->GetHeight();
}

} /* namespace rsp::graphics */
