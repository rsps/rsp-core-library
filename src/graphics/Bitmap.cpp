/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/Bitmap.h>
#include <filesystem>
#include <utils/Crc32.h>

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

Bitmap::Bitmap(const uint32_t *apPixels, const GuiUnit_t aHeight, const GuiUnit_t aWidth, const ColorDepth aDepth)
    : Canvas(aWidth, aHeight, aDepth)
{
    Init(reinterpret_cast<uintptr_t>(this), aWidth, aHeight, aDepth, reinterpret_cast<const uint8_t*>(apPixels));
}

Bitmap::Bitmap(const PixelData &arPixelData)
{
    Assign(arPixelData);
}

Bitmap& Bitmap::Load(const std::string &arImgName)
{
    const std::filesystem::path filename(arImgName);

    const auto loader = ImgLoader::GetRasterLoader(filename.extension());
    // Get raw data
    loader->LoadImg(filename);
    Assign(loader->GetPixelData());
    mId = Crc32::Calc(arImgName.c_str(), arImgName.size());
    return *this;
}

Bitmap& Bitmap::Assign(const uint32_t *apPixels, const GuiUnit_t aHeight, const GuiUnit_t aWidth, const ColorDepth aDepth)
{
    Init(reinterpret_cast<uintptr_t>(this), aWidth, aHeight, aDepth, reinterpret_cast<const uint8_t*>(apPixels));

    return *this;
}

Bitmap& Bitmap::Assign(const PixelData &arPixelData)
{
    PixelData::operator=(arPixelData);
//    Init(arPixelData.GetWidth(), arPixelData.GetHeight(), arPixelData.GetColorDepth(), arPixelData.GetData());
    return *this;
}

} // namespace rsp::graphics
