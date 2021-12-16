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

#include <utils/RSPCoreExceptions.h>

using namespace rsp::utils;

namespace rsp::graphics
{

std::unordered_map<std::string, std::function<std::shared_ptr<ImgLoader>()>> Bitmap::filetypeMap = {
    {".bmp", std::function<std::shared_ptr<ImgLoader>()>([]() { return std::make_shared<BmpLoader>(); })},
    {".png", std::function<std::shared_ptr<ImgLoader>()>([]() { return std::make_shared<PngLoader>(); })}};

Bitmap::Bitmap(std::string aImgName)
    : mBytesPerPixel(0)
{
    std::filesystem::path filename(aImgName);

    auto loader = GetRasterLoader(filename.extension());
    // Get raw data
    mImagePixels = loader->LoadImg(filename);
    mHeight = loader->GetHeight();
    mWidth = loader->GetWidth();
}

Bitmap::Bitmap(const uint32_t *apPixels, int aHeight, int aWidth, int aBytesPerPixel)
    : mHeight(aHeight), mWidth(aWidth), mBytesPerPixel(aBytesPerPixel), mImagePixels(mWidth * mHeight)
{
    for (int y = 0; y < mHeight; y++) {
        for (int x = 0; x < mWidth; x++) {
            mImagePixels[x + (y * mWidth)] = *apPixels++;
        }
    }
}

Bitmap::Bitmap(int aHeight, int aWidth, int aBytesPerPixel)
    : mHeight(aHeight), mWidth(aWidth), mBytesPerPixel(aBytesPerPixel), mImagePixels(mWidth * mHeight)
{
    throw NotImplementedException("");
    // Load file into memory here
    // https://freeimage.sourceforge.io/
    // http://libjpeg.sourceforge.net/
}

Bitmap::~Bitmap()
{
}

std::shared_ptr<ImgLoader> Bitmap::GetRasterLoader(const std::string aFileType)
{
    try {
        return filetypeMap.at(aFileType)();
    } catch (const std::out_of_range &e) {
        throw std::out_of_range(std::string("Filetype loader not found") + ": " + e.what());
    }
}

} // namespace rsp::graphics
