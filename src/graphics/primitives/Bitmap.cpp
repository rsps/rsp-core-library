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
#include <graphics/primitives/raster/BmpLoader.h>
#include <graphics/primitives/raster/PngLoader.h>

#include <utils/CoreException.h>

using namespace rsp::utils;

namespace rsp::graphics
{

std::unordered_map<std::string, std::function<std::shared_ptr<ImgLoader>()>> Bitmap::msFiletypeMap = {
    {".bmp", std::function<std::shared_ptr<ImgLoader>()>([]() { return std::make_shared<BmpLoader>(); })},
    {".png", std::function<std::shared_ptr<ImgLoader>()>([]() { return std::make_shared<PngLoader>(); })}};

Bitmap::Bitmap(const std::string &arImgName)
    : Canvas()
{
    std::filesystem::path filename(arImgName);

    auto loader = GetRasterLoader(filename.extension());
    // Get raw data
    mImagePixels = loader->LoadImg(filename);
    mHeight = loader->GetHeight();
    mWidth = loader->GetWidth();
}

Bitmap::Bitmap(const uint32_t *apPixels, int aHeight, int aWidth, int aBytesPerPixel)
    : Canvas(aHeight, aWidth, aBytesPerPixel), mImagePixels(static_cast<long unsigned int>(aWidth * aHeight))
{
    for (int y = 0; y < mHeight; y++) {
        for (int x = 0; x < mWidth; x++) {
            mImagePixels[static_cast<long unsigned int>(x + (y * mWidth))] = *apPixels++;
        }
    }
}

Bitmap::Bitmap(int aHeight, int aWidth, int aBytesPerPixel)
    : Canvas(aHeight, aWidth, aBytesPerPixel), mImagePixels(static_cast<long unsigned int>(aWidth * aHeight))
{
}

uint32_t Bitmap::GetPixel(const Point &aPoint, const bool aFront) const
{
    if (!IsInsideScreen(aPoint)) {
        return 0;
    }
    long location = (mWidth * aPoint.mY) + aPoint.mX;
    return mImagePixels[static_cast<long unsigned int>(location)];
}

std::shared_ptr<ImgLoader> Bitmap::GetRasterLoader(const std::string aFileType)
{
    try {
        return msFiletypeMap.at(aFileType)();
    } catch (const std::out_of_range &e) {
        throw std::out_of_range(std::string("Filetype loader not found") + ": " + e.what());
    }
}
} // namespace rsp::graphics
