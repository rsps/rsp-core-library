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
#include <string>
#include "graphics/controls/Image.h"
#include "graphics/primitives/Rect.h"
#include <utils/CoreException.h>

using namespace rsp::utils;

namespace rsp::graphics
{

void Image::ClearSection()
{
    Style &style = mStyles.at(States::normal);
    if (style.mpBitmap) {
        mSection = Rect(0u, 0u, style.mpBitmap->GetWidth(), style.mpBitmap->GetHeight());
    }
    else {
        mSection = mArea;
    }
}

void Image::SetSection(const Rect &arSection)
{
    Style &style = mStyles.at(States::normal);
    mSection = Rect(
        std::min(0u, static_cast<unsigned int>(arSection.GetTop())),
        std::min(0u, static_cast<unsigned int>(arSection.GetLeft())),
        std::min(style.mpBitmap->GetWidth(), static_cast<unsigned int>(arSection.GetWidth())),
        std::min(style.mpBitmap->GetHeight(), static_cast<unsigned int>(arSection.GetHeight()))
    );
}

void Image::paint(Canvas &arCanvas, const Style &arStyle)
{
    Control::paint(arCanvas, arStyle);

    if (!arStyle.mpBitmap) {
        THROW_WITH_BACKTRACE1(CoreException, std::string("Bitmap is null for state " + to_string(mState)));
    }
    arCanvas.DrawImageSection(mArea.GetTopLeft(), *arStyle.mpBitmap, mSection, arStyle.mForegroundColor);
}

Image& Image::SetArea(const Rect &arRect)
{
    Control::SetArea(arRect);
    mSection = arRect;
    return *this;
}

} // namespace rsp::graphics
