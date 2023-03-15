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

namespace rsp::graphics
{

void Image::ClearSection()
{
    mBitmap.ClearSection();
}

void Image::SetSection(const Rect &arSection)
{
    mBitmap.SetSection(arSection);
}

//void Image::paint(Canvas &arCanvas, const Style &arStyle)
//{
//    Control::paint(arCanvas, arStyle);
//
//    mBitmap.Paint(GetOrigin(), arCanvas);
//}

} // namespace rsp::graphics
