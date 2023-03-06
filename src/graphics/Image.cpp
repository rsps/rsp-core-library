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
#include <graphics/Image.h>
#include <graphics/Rect.h>

namespace rsp::graphics
{

void Image::ClearSection()
{
    for (auto &style : mStyles) {
        style.mBackground.ClearSection();
    }
}

void Image::SetSection(const Rect &arSection)
{
    for (auto &style : mStyles) {
        style.mBackground.SetSection(arSection);
    }
}

void Image::paint(Canvas &arCanvas, const Style &arStyle)
{
    Control::paint(arCanvas, arStyle);

    arStyle.mBackground.Paint(GetOrigin(), arCanvas);
}

} // namespace rsp::graphics
