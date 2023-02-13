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
#include <posix/FileSystem.h>
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
        case PixelData::ColorDepth::RGBA:
            os << "RGBA";
            break;
        default:
            os << "Unknown";
            break;
    }
    return os;
}


PixelData::PixelData(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const std::uint8_t *apData)
    : mColorDepth(aDepth),
      mWidth(aWidth),
      mHeight(aHeight),
      mpData(apData)
{
}

PixelData::PixelData(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth)
    : mColorDepth(aDepth),
      mWidth(aWidth),
      mHeight(aHeight)
{
    mData.resize(GetDataSize());
    mpData = mData.data();
}

PixelData& PixelData::Init(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const std::uint8_t *apData)
{
    mColorDepth = aDepth;
    mWidth = aWidth;
    mHeight = aHeight;
    if (apData) {
        mData.clear();
        mpData = apData;
    }
    else {
        mData.resize(GetDataSize());
        mpData = mData.data();
    }
    return *this;
}

std::size_t PixelData::GetDataSize() const
{
    int result;

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

        case ColorDepth::RGBA:
            result = (mWidth * mHeight) * 4;
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }

    return std::size_t(result);
}

Color PixelData::GetPixelAt(GuiUnit_t aX, GuiUnit_t aY, Color aColor) const
{
    if (aX >= mWidth || aY >= mHeight) {
        THROW_WITH_BACKTRACE1(std::out_of_range, "Pixel coordinates out of range (" + std::to_string(aX) + "<" + std::to_string(mWidth) + "," + std::to_string(aY) + "<" + std::to_string(mHeight) + ")");
    }
    Color result(aColor);
    int offset;
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

        case ColorDepth::RGBA:
            offset = ((aY * mWidth) + aX) * 4;
            result.SetRed(mpData[offset + 0]);
            result.SetGreen(mpData[offset + 1]);
            result.SetBlue(mpData[offset + 2]);
            result.SetAlpha(mpData[offset + 3]);
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }
//    DUMP(aX << ", " << aY << ", " << aColor, result << " from " << mWidth << ", " << mHeight << ", " << mColorDepth);
    return result;
}

PixelData& PixelData::SetPixelAt(GuiUnit_t aX, GuiUnit_t aY, Color aColor)
{
    if (aX >= mWidth || aY >= mHeight) {
        THROW_WITH_BACKTRACE1(std::out_of_range, "Pixel coordinates out of range (" + std::to_string(aX) + "<" + std::to_string(mWidth) + "," + std::to_string(aY) + "<" + std::to_string(mHeight) + ")");
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
    std::uint8_t *pdata = mData.data();

//    Color result;
    int offset;
    switch (mColorDepth) {
        case ColorDepth::Monochrome:
            offset = (((mWidth + 7) >> 3) * aY) + (aX >> 3);
            if (aColor.GetAlpha() > 0) {
                pdata[offset] |= (1 << (aX % 8));
            }
            else {
                pdata[offset] &= ~(1 << (aX % 8));
            }
            break;

        case ColorDepth::Alpha:
            offset = (aY * mWidth) + aX;
            pdata[offset] = aColor.GetAlpha();
            break;

        case ColorDepth::RGB:
            offset = ((aY * mWidth) + aX) * 3;
            pdata[offset + 0] = aColor.GetRed();
            pdata[offset + 1] = aColor.GetGreen();
            pdata[offset + 2] = aColor.GetBlue();
            break;

        case ColorDepth::RGBA:
            offset = ((aY * mWidth) + aX) * 4;
            pdata[offset + 0] = aColor.GetRed();
            pdata[offset + 1] = aColor.GetGreen();
            pdata[offset + 2] = aColor.GetBlue();
            pdata[offset + 3] = aColor.GetAlpha();
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
    if (mData.size() > 0) {
        mpData = mData.data();
    }
    else {
        mpData = arOther.mpData;
    }
}

PixelData::PixelData(const PixelData &&arOther)
{
    mColorDepth = arOther.mColorDepth;
    mWidth = arOther.mWidth;
    mHeight = arOther.mHeight;
    mData = std::move(arOther.mData);
    if (mData.size() > 0) {
        mpData = mData.data();
    }
    else {
        mpData = arOther.mpData;
    }
}

PixelData& PixelData::operator =(const PixelData &arOther)
{
    if (this != &arOther) {
        mColorDepth = arOther.mColorDepth;
        mWidth = arOther.mWidth;
        mHeight = arOther.mHeight;
        mData = arOther.mData;
        if (mData.size() > 0) {
            mpData = mData.data();
        }
        else {
            mpData = arOther.mpData;
        }
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
        if (mData.size() > 0) {
            mpData = mData.data();
        }
        else {
            mpData = arOther.mpData;
        }
    }
    return *this;
}

void PixelData::initAfterLoad(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth)
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

    fo << "#include \"" << fo.Name() << ".h\"\n" << std::endl;

    fo << "static const std::uint8_t c" << fo.Name() << "PixData[" << GetDataSize() << "] = {\n";
    fo.Hex(mpData, GetDataSize());
    fo << "};\n\n";

    fo << "using namespace rsp::graphics;\n" << std::endl;

    fo << "const PixelData c"
        << fo.Name() << "(" << mWidth << "u, " << mHeight << "u, PixelData::ColorDepth::"
        << mColorDepth << ", " << "c" << fo.Name() << "PixData);\n" << std::endl;

    std::filesystem::path hfile = arFileName;
    hfile.replace_extension("h");
    std::fstream header(hfile, std::ios_base::out | std::ios_base::trunc);
    header << "#include <graphics/primitives/PixelData.h>\n" << std::endl;
    header << "extern const rsp::graphics::PixelData c" << fo.Name() << ";" << std::endl;
}

PixelData PixelData::ChangeColorDepth(ColorDepth aDepth) const
{
    PixelData pd(GetWidth(), GetHeight(), aDepth);
    for (GuiUnit_t y = 0; y < GetHeight() ; ++y) {
        for (GuiUnit_t x = 0; x < GetWidth() ; ++x) {
            Color pixel = GetPixelAt(x, y, Color::Black);
            pd.SetPixelAt(x, y, pixel);
        }
    }
    return pd;
}

} /* namespace rsp::graphics */
