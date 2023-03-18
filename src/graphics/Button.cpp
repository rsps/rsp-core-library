/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/Button.h>

namespace rsp::graphics {


Button& Button::Setup(Rect aTouchArea, Rect aArea, Point aPosition)
{
    SetArea(aArea.MoveTo(aPosition));
    SetTouchArea(aTouchArea);
    SetTexturePosition(aPosition);
    return *this;
}

Button& Button::Background(Control::States aState, Color aColor, OptionalPixels_t aPixels, Point aOffset)
{
    Style& style = GetStyle(aState);
    if (aPixels) {
        style.mTextures.push_back(Texture::Create(*aPixels, aColor));
        style.mTextures.back()->SetOffset(aOffset);
    }
    style.mBackgroundColor = aColor;
    return *this;
}

Button& Button::Foreground(Control::States aState, Color aColor, OptionalPixels_t aPixels, Point aOffset)
{
    Style& style = GetStyle(aState);
    if (aPixels) {
        style.mTextures.push_back(Texture::Create(*aPixels, aColor));
        style.mTextures.back()->SetOffset(aOffset);
    }
    style.mForegroundColor = aColor;
    return *this;
}

} /* namespace rsp::graphics */
