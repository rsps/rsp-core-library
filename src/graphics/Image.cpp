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

Image::Image()
{
    initTypeInfo<Image>();
}

Image::Image(const BitmapView &arBitmap)
    : mBitmap(arBitmap)
{
    initTypeInfo<Image>();
    SetArea(mBitmap.GetBoundingRect());
}

Image::Image(const BitmapView &&arBitmap)
    : mBitmap(std::move(arBitmap))
{
    initTypeInfo<Image>();
    SetArea(mBitmap.GetBoundingRect());
}

Image& Image::operator=(const BitmapView &arBitmap)
{
    mBitmap = arBitmap;
    return *this;
}

Image& Image::operator=(const BitmapView &&arBitmap)
{
    mBitmap = std::move(arBitmap);
    return *this;
}

void Image::Render(Renderer &arRenderer) const
{
    Control::Render(arRenderer);
    arRenderer.Render(*mpTexture);
}

void Image::update()
{
    if (mpTexture) {
        mpTexture->Update(mBitmap.GetPixelData(), mStyles[mState].mForegroundColor);

        auto sr = Rect(0, 0, mBitmap.GetWidth(), mBitmap.GetHeight());
        if (mpParent) {
            sr &= mpParent->GetArea();
        }
        mpTexture->SetDestination(mArea.GetTopLeft())
            .SetSourceRect(sr);
    }
}

void Image::doSetArea(const rsp::graphics::Rect &arRect)
{
    mpTexture = Texture::Create(arRect.GetWidth(), arRect.GetHeight());
}

} // namespace rsp::graphics
