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
    for (auto &tuple : mStyles) {
        tuple.second.mBitmapView.ClearSection();
    }
}

void Image::SetSection(const Rect &arSection)
{
    for (auto &tuple : mStyles) {
        tuple.second.mBitmapView.SetSection(arSection);
    }
}

void Image::paint(Canvas &arCanvas, const Style &arStyle)
{
    Control::paint(arCanvas, arStyle);

    arStyle.mBitmapView.Paint(arCanvas);
}

Image& Image::SetArea(const Rect &arRect)
{
    Control::SetArea(arRect);
    for (auto &tuple : mStyles) {
        tuple.second.mBitmapView.SetSection(arRect).SetDestination(arRect.GetTopLeft());
    }
    return *this;
}

} // namespace rsp::graphics
