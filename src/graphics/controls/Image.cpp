/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/controls/Image.h"

namespace rsp::graphics
{
Image::Image(std::string aPressed, std::string aNormal, Rect &arRect)
    : Control(arRect)
{
    mStateMap.insert(std::pair<States, Bitmap>(States::pressed, Bitmap(aPressed)));
    mStateMap.insert(std::pair<States, Bitmap>(States::normal, Bitmap(aNormal)));
}
Image::~Image()
{
}

void Image::Render(Canvas &arCanvas)
{
    if (!mIsInvalid) {
        return;
    }
    arCanvas.DrawImage(mArea.GetTopLeft(), mStateMap.at(mState));
    mIsInvalid = false;
}

} // namespace rsp::graphics
