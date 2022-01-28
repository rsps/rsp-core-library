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
    std::cout << "Constructor of Image" << std::endl;
}
Image::~Image()
{
}

void Image::Render(Canvas &aCanvas)
{
    std::cout << "Rendering Img" << std::endl;
    std::cout << this << " " << (int)mState << std::endl;
    if (!mIsInvalid) {
        std::cout << "Img NOT invalid" << std::endl;
        return;
    }
    switch (mState) {
    case States::pressed:
        std::cout << "Rendering Pressed" << std::endl;
        aCanvas.DrawImage(mArea.GetTopLeft(), mPressed);
        break;
    case States::normal:
        std::cout << "Rendering Normal" << std::endl;
        aCanvas.DrawImage(mArea.GetTopLeft(), mNormal);
        break;
    default:
        break;
    }
    mIsInvalid = false;
}
} // namespace rsp::graphics