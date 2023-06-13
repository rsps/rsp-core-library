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
    SetArea(mBitmap.GetBoundingRect());
    return *this;
}

Image& Image::operator=(const BitmapView &&arBitmap)
{
    mBitmap = std::move(arBitmap);
    SetArea(mBitmap.GetBoundingRect());
    return *this;
}

void Image::render(Renderer &arRenderer) const
{
    arRenderer.Blit(*mpTexture);
}

void Image::update()
{
    if (mpTexture) {
        if (mBitmap.GetPixelData().GetColorDepth() != ColorDepth::RGB) {
            mpTexture->Fill(Color::None);
            mpTexture->SetBlendOperation(Texture::BlendOperation::SourceAlpha);
        }
        else {
            mpTexture->SetBlendOperation(Texture::BlendOperation::Copy);
        }
        mpTexture->Update(mBitmap.GetPixelData(), mStyles[mState].mForegroundColor);

        auto sr = Rect(0, 0, mBitmap.GetWidth(), mBitmap.GetHeight());
        if (mpParent) {
            Rect a = mpParent->GetArea();
            sr &= a.MoveTo({0, 0});
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
