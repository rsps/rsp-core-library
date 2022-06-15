/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/primitives/PixelData.h>

namespace rsp::graphics {

PixelData::PixelData(unsigned int aWidth, unsigned int aHeight, ColorDepth aDepth, const std::uint8_t *aData)
    : mColorDepth(aDepth),
      mWidth(aWidth),
      mHeight(aHeight),
      mpData(const_cast<std::uint8_t*>(aData))
{
}

PixelData::PixelData(unsigned int aWidth, unsigned int aHeight, ColorDepth aDepth)
    : mColorDepth(aDepth),
      mWidth(aWidth),
      mHeight(aHeight)
{
    mData.resize(GetDataSize());
    mpData = mData.data();
}

std::size_t PixelData::GetDataSize() const
{
    std::size_t result;

    switch (mColorDepth) {
        case ColorDepth::Monochrome:
            result = (((mWidth + 7) >> 3) * mHeight);
            break;

        case ColorDepth::Alpha:
            result = (mWidth * mHeight);
            break;

        case ColorDepth::RGB:
            result = (mWidth * mHeight) * 3;
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }

    return result;
}

Color PixelData::GetPixelAt(unsigned int aX, unsigned int aY, Color aColor) const
{
    if (aX >= mWidth || aY >= mHeight) {
        THROW_WITH_BACKTRACE1(std::out_of_range, "Pixel coordinates out of range");
    }
    unsigned int offset;
    switch (mColorDepth) {
        case ColorDepth::Monochrome:
            offset = (((mWidth + 7) >> 3) * aY) + (aX >> 3);
            aColor.SetAlpha( ((mpData[offset] & (1 << (aX % 8))) > 0) ? 255 : 0);
            break;

        case ColorDepth::Alpha:
            offset = (aY * mWidth) + aX;
            aColor.SetAlpha(mpData[offset]);
            break;

        case ColorDepth::RGB:
            offset = ((aY * mWidth) + aX) * 3;
            aColor.SetRed(mpData[offset + 0]);
            aColor.SetGreen(mpData[offset + 1]);
            aColor.SetBlue(mpData[offset + 2]);
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }
    return aColor;
}

PixelData& PixelData::SetPixelAt(unsigned int aX, unsigned int aY, Color aColor)
{
    if (aX >= mWidth || aY >= mHeight) {
        THROW_WITH_BACKTRACE1(std::out_of_range, "Pixel coordinates out of range");
    }

    if (mData.size() == 0) {
        // Copy const data to internal mData buffer
        mData.resize(GetDataSize());
        for (auto &v : mData) {
            v = *mpData;
            mpData++;
        }
        mpData = mData.data();
    }

    unsigned int offset;
    switch (mColorDepth) {
        case ColorDepth::Monochrome:
            offset = (((mWidth + 7) >> 3) * aY) + (aX >> 3);
            mpData[offset] = (aColor.GetAlpha() > 0) ?(1 << (aX % 8)) : 0;
            break;

        case ColorDepth::Alpha:
            offset = (aY * mWidth) + aX;
            mpData[offset] = aColor.GetAlpha();
            break;

        case ColorDepth::RGB:
            offset = ((aY * mWidth) + aX) * 3;
            mpData[offset + 0] = aColor.GetRed();
            mpData[offset + 1] = aColor.GetGreen();
            mpData[offset + 2] = aColor.GetBlue();
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }

    return *this;
}

void PixelData::initAfterLoad(unsigned int aWidth, unsigned int aHeight, ColorDepth aDepth)
{
    mColorDepth = aDepth;
    mWidth = aWidth;
    mHeight = aHeight;
    auto sz = GetDataSize();
    if (mData.size() != sz) {
        mData.resize(sz);
    }
    mpData = mData.data();
}

} /* namespace rsp::graphics */
