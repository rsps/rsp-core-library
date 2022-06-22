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
#include <utils/CppObjectFile.h>
#include <logging/Logger.h>

namespace rsp::graphics {

std::ostream& operator<<(std::ostream& os, const PixelData::ColorDepth arDepth)
{
    switch (arDepth) {
        case PixelData::ColorDepth::Monochrome:
            os << "Monochrome";
            break;
        case PixelData::ColorDepth::Alpha:
            os << "Alpha";
            break;
        case PixelData::ColorDepth::RGB:
            os << "RGB";
            break;
        default:
            os << "Unknown";
            break;
    }
    return os;
}


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
    Color result(aColor);
    unsigned int offset;
    switch (mColorDepth) {
        case ColorDepth::Monochrome:
            offset = (((mWidth + 7) >> 3) * aY) + (aX >> 3);
            result.SetAlpha( ((mpData[offset] & (1 << (aX % 8))) > 0) ? 255 : 0);
            break;

        case ColorDepth::Alpha:
            offset = (aY * mWidth) + aX;
            result.SetAlpha(mpData[offset]);
            break;

        case ColorDepth::RGB:
            offset = ((aY * mWidth) + aX) * 3;
            result.SetRed(mpData[offset + 0]);
            result.SetGreen(mpData[offset + 1]);
            result.SetBlue(mpData[offset + 2]);
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }
//    DUMP(aX << ", " << aY << ", " << aColor, result << " from " << mWidth << ", " << mHeight << ", " << mColorDepth);
    return result;
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

    Color result;
    unsigned int offset;
    switch (mColorDepth) {
        case ColorDepth::Monochrome:
            offset = (((mWidth + 7) >> 3) * aY) + (aX >> 3);
            if (aColor.GetAlpha() > 0) {
                mpData[offset] |= (1 << (aX % 8));
            }
            else {
                mpData[offset] &= ~(1 << (aX % 8));
            }
            offset = (((mWidth + 7) >> 3) * aY) + (aX >> 3);
            result.SetAlpha( ((mpData[offset] & (1 << (aX % 8))) > 0) ? 255 : 0);
            break;

        case ColorDepth::Alpha:
            offset = (aY * mWidth) + aX;
            mpData[offset] = aColor.GetAlpha();
            offset = (aY * mWidth) + aX;
            result.SetAlpha(mpData[offset]);
            break;

        case ColorDepth::RGB:
            offset = ((aY * mWidth) + aX) * 3;
            mpData[offset + 0] = aColor.GetRed();
            mpData[offset + 1] = aColor.GetGreen();
            mpData[offset + 2] = aColor.GetBlue();
            result.SetRed(mpData[offset + 0]);
            result.SetGreen(mpData[offset + 1]);
            result.SetBlue(mpData[offset + 2]);
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }

    return *this;
}

PixelData::PixelData(const PixelData &arOther)
{
    mColorDepth = arOther.mColorDepth;
    mWidth = arOther.mWidth;
    mHeight = arOther.mHeight;
    mData = arOther.mData;
    mpData = mData.data();
}

PixelData::PixelData(const PixelData &&arOther)
{
    mColorDepth = arOther.mColorDepth;
    mWidth = arOther.mWidth;
    mHeight = arOther.mHeight;
    mData = std::move(arOther.mData);
    mpData = mData.data();
}

PixelData& PixelData::operator =(const PixelData &arOther)
{
    if (this != &arOther) {
        mColorDepth = arOther.mColorDepth;
        mWidth = arOther.mWidth;
        mHeight = arOther.mHeight;
        mData = arOther.mData;
        mpData = mData.data();
    }
    return *this;
}

PixelData& PixelData::operator =(const PixelData &&arOther)
{
    if (this != &arOther) {
        mColorDepth = arOther.mColorDepth;
        mWidth = arOther.mWidth;
        mHeight = arOther.mHeight;
        mData = std::move(arOther.mData);
        mpData = mData.data();
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

void PixelData::SaveToCFile(const std::filesystem::path &arFileName)
{
    rsp::utils::CppObjectFile fo(arFileName);

    fo << "static const std::uint8_t c" << fo.Name() << "PixData[] = {\n";
    fo.Hex(mpData, GetDataSize());
    fo << "};\n\n";

    fo << "const PixelData c"
        << fo.Name() << "(" << mWidth << ", " << mHeight << ", PixelData::ColorDepth::"
        << mColorDepth << ", " << "c" << fo.Name() << "PixData);\n" << std::endl;
}

} /* namespace rsp::graphics */
