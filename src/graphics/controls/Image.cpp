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
    : Control(aRect)
{
    mStateMap.insert(std::pair<States, Bitmap>(States::pressed, Bitmap(aPressed)));
    mStateMap.insert(std::pair<States, Bitmap>(States::normal, Bitmap(aNormal)));
}
Image::~Image()
{
}

void Image::Render(Canvas &aCanvas)
{
    if (!mIsInvalid) {
        return;
    }
    std::cout << "Rendering Img" << std::endl;
    aCanvas.DrawImage(mArea.GetTopLeft(), mStateMap.at(mState));
    mIsInvalid = false;
}

void Image::HandleCallback(States aState)
{
    std::cout << "Image handling Pressed Callback" << std::endl;
    if (aState != mState) {
        mState = aState;
        std::cout << "Invalidating Image object" << std::endl;
        this->Invalidate();
    }
}
} // namespace rsp::graphics