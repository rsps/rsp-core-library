/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <algorithm>
#include <filesystem>
#include <graphics/primitives/Bitmap.h>
#include <utils/CoreException.h>
#include <logging/Logger.h>

using namespace rsp::utils;

namespace rsp::graphics
{

std::ostream& operator <<(std::ostream &os, const Bitmap &arBmp)
{
    os << arBmp.GetPixelData().GetWidth() << "x" << arBmp.GetPixelData().GetHeight();
    return os;
}

Bitmap::Bitmap(const std::string &arImgName)
    : Canvas()
{
    Load(arImgName);
}

Bitmap::Bitmap(const uint32_t *apPixels, GuiUnit_t aHeight, GuiUnit_t aWidth, unsigned int aBytesPerPixel)
    : Canvas(aHeight, aWidth, aBytesPerPixel),
      mImagePixels(aWidth, aHeight, PixelData::ColorDepth::RGB, reinterpret_cast<const std::uint8_t*>(apPixels))
{
    for (GuiUnit_t y = 0; y < mHeight; y++) {
        for (GuiUnit_t x = 0; x < mWidth; x++) {
            mImagePixels.SetPixelAt(x, y, Color(*apPixels++));
        }
    }
}

Bitmap::Bitmap(GuiUnit_t aHeight, GuiUnit_t aWidth, unsigned int aBytesPerPixel)
    : Canvas(aHeight, aWidth, aBytesPerPixel),
      mImagePixels(aWidth, aHeight, PixelData::ColorDepth::RGB)
{
}

void Bitmap::SetPixel(const Point &arPoint, const Color &arColor)
{
    if (!IsHit(arPoint)) {
        return;
    }
    mImagePixels.SetPixelAt(arPoint.GetX(), arPoint.GetY(), arColor);
}

std::uint32_t Bitmap::GetPixel(const Point &aPoint, const bool aFront) const
{
    if (!IsHit(aPoint)) {
        return 0;
    }
    return mImagePixels.GetPixelAt(aPoint.mX, aPoint.mY, Color::White);
}

Bitmap& Bitmap::Load(const std::string &arImgName)
{
    std::filesystem::path filename(arImgName);

    auto loader = ImgLoader::GetRasterLoader(filename.extension());
    // Get raw data
    loader->LoadImg(filename);
    mImagePixels = loader->GetPixelData();
    mWidth = mImagePixels.GetWidth();
    mHeight = mImagePixels.GetHeight();
    mClipRect = Rect(0, 0, mWidth, mHeight);
    return *this;
}

Bitmap& Bitmap::Assign(const uint32_t *apPixels, GuiUnit_t aHeight, GuiUnit_t aWidth, unsigned int aBytesPerPixel)
{
    mHeight = aHeight;
    mWidth = aWidth;
    mBytesPerPixel = aBytesPerPixel;

    mImagePixels.Init(aWidth, aHeight, PixelData::ColorDepth::RGB, reinterpret_cast<const std::uint8_t*>(apPixels));

    for (GuiUnit_t y = 0; y < mHeight; y++) {
        for (GuiUnit_t x = 0; x < mWidth; x++) {
            mImagePixels.SetPixelAt(x, y, Color(*apPixels++));
        }
    }

    return *this;
}

Bitmap& Bitmap::Assign(const PixelData &arPixelData)
{
    mHeight = arPixelData.GetHeight();
    mWidth = arPixelData.GetWidth();
    mBytesPerPixel = (static_cast<unsigned int>(arPixelData.GetColorDepth()) + 7u) / 8u;
    mImagePixels.Init(mWidth, mHeight, arPixelData.GetColorDepth(), arPixelData.GetData());
    return *this;
}

} // namespace rsp::graphics
