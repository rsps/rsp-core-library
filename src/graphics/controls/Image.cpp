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
Image::Image(std::string aPressed, std::string aNormal, Rect &aRect)
    : mPressed(aPressed), mNormal(aNormal), Control(aRect)
{
}
Image::~Image()
{
}

void Image::Render(Canvas &aCanvas)
{
    if (!mIsInvalid) {
        return;
    }
    switch (mState) {
    case States::pressed:
        aCanvas.DrawImage(mArea.GetTopLeft(), mPressed);
        mIsInvalid = false;
        break;
    case States::normal:
        aCanvas.DrawImage(mArea.GetTopLeft(), mNormal);
        mIsInvalid = false;
        break;
    default:
        break;
    }
}
} // namespace rsp::graphics