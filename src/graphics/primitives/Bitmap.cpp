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

Bitmap::Bitmap(const uint32_t *apPixels, GuiUnit_t aHeight, GuiUnit_t aWidth, PixelData::ColorDepth aDepth)
    : Canvas(aHeight, aWidth, aDepth)
{
    mPixelData.Init(aWidth, aHeight, aDepth, reinterpret_cast<const std::uint8_t*>(apPixels));
}


Bitmap& Bitmap::Load(const std::string &arImgName)
{
    std::filesystem::path filename(arImgName);

    auto loader = ImgLoader::GetRasterLoader(filename.extension());
    // Get raw data
    loader->LoadImg(filename);
    mPixelData = loader->GetPixelData();
    mClipRect = Rect(0, 0, mPixelData.GetWidth(), mPixelData.GetHeight());
    return *this;
}

Bitmap& Bitmap::Assign(const uint32_t *apPixels, GuiUnit_t aHeight, GuiUnit_t aWidth, PixelData::ColorDepth aDepth)
{
    mPixelData.Init(aWidth, aHeight, aDepth, reinterpret_cast<const std::uint8_t*>(apPixels));

    return *this;
}

Bitmap& Bitmap::Assign(const PixelData &arPixelData)
{
    mPixelData = arPixelData;
    return *this;
}

} // namespace rsp::graphics
