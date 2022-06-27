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

Bitmap::Bitmap(const std::string &arImgName)
    : Canvas()
{
    std::filesystem::path filename(arImgName);

    auto loader = ImgLoader::GetRasterLoader(filename.extension());
    // Get raw data
    loader->LoadImg(filename);
    mImagePixels = loader->GetPixelData();
    mWidth = mImagePixels.GetWidth();
    mHeight = mImagePixels.GetHeight();
}

Bitmap::Bitmap(const uint32_t *apPixels, unsigned int aHeight, unsigned int aWidth, unsigned int aBytesPerPixel)
    : Canvas(aHeight, aWidth, aBytesPerPixel),
      mImagePixels(aWidth, aHeight, PixelData::ColorDepth::RGB, reinterpret_cast<const std::uint8_t*>(apPixels))
{
    for (unsigned int y = 0; y < mHeight; y++) {
        for (unsigned int x = 0; x < mWidth; x++) {
            mImagePixels.SetPixelAt(x, y, Color(*apPixels++));
        }
    }
}

Bitmap::Bitmap(unsigned int aHeight, unsigned int aWidth, unsigned int aBytesPerPixel)
    : Canvas(aHeight, aWidth, aBytesPerPixel),
      mImagePixels(aWidth, aHeight, PixelData::ColorDepth::RGB)
{
}

std::uint32_t Bitmap::GetPixel(const Point &aPoint, const bool aFront) const
{
    if (!IsInsideScreen(aPoint)) {
        return 0;
    }
    return mImagePixels.GetPixelAt(aPoint.mX, aPoint.mY, Color::White);
}


} // namespace rsp::graphics
