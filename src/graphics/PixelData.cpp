/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cstring>
#include <cstdlib>
#include <graphics/PixelData.h>
#include <logging/Logger.h>
#include <posix/FileSystem.h>
#include <utils/Crc32.h>
#include <utils/CppObjectFile.h>
#include <utils/StrUtils.h>

namespace rsp::graphics {

std::ostream& operator<<(std::ostream& os, const ColorDepth arDepth)
{
    switch (arDepth) {
        case ColorDepth::Monochrome:
            os << "Monochrome";
            break;
        case ColorDepth::Alpha:
            os << "Alpha";
            break;
        case ColorDepth::RGB:
            os << "RGB";
            break;
        case ColorDepth::RGBA:
            os << "RGBA";
            break;
        default:
            os << "Unknown";
            break;
    }
    return os;
}

PixelData::PixelData(const GfxResource &arResource)
    : mColorDepth(arResource.Depth),
      mRect(0, 0, arResource.Width, arResource.Height),
      mpData(arResource.PixData)
{
    if (arResource.Compressed) {
        Decompress(getCompressionType(arResource.Compressed), mpData, arResource.PixDataSize);
    }
    mId = arResource.Id;
}


PixelData::PixelData(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const std::uint8_t *apData, size_t aDataSize, bool aCompressed)
    : mColorDepth(aDepth),
      mRect(0, 0, aWidth, aHeight),
      mpData(apData)
{
    if (aCompressed) {
        Decompress(getCompressionType(aCompressed), mpData, aDataSize);
    }
    mId = uint32_t(uintptr_t(mpData));
}

PixelData::PixelData(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth)
    : mColorDepth(aDepth),
      mRect(0, 0, aWidth, aHeight)
{
    mData.resize(GetDataSize());
    mpData = mData.data();
    mId = uint32_t(uintptr_t(mpData));
}

PixelData::PixelData(const PixelData &arOther)
{
    assign(arOther);
}

PixelData::PixelData(PixelData &&arOther)
{
    move(std::move(arOther));
}

PixelData& PixelData::operator=(const PixelData &arOther)
{
    if (this != &arOther) {
        assign(arOther);
    }
    return *this;
}

PixelData& PixelData::operator=(PixelData &&arOther)
{
    if (this != &arOther) {
        move(std::move(arOther));
    }
    return *this;
}

void PixelData::assign(const PixelData& arOther)
{
    mId = arOther.mId;
    mColorDepth = arOther.mColorDepth;
    mRect = arOther.mRect;
    mData = arOther.mData;
    if (mData.size() > 0) {
        mpData = mData.data();
    }
    else {
        mpData = arOther.mpData;
    }
}

void PixelData::move(PixelData &&arOther)
{
    mId = arOther.mId;
    mColorDepth = arOther.mColorDepth;
    mRect = std::move(arOther.mRect);
    mData = std::move(arOther.mData);
    if (mData.size() > 0) {
        mpData = mData.data();
    }
    else {
        mpData = arOther.mpData;
    }
}


PixelData& PixelData::Init(uint32_t aId, GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const std::uint8_t *apData)
{
    mId = aId;
    mColorDepth = aDepth;
    mRect = Rect(0, 0, aWidth, aHeight);
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

size_t PixelData::GetDataSize() const
{
    int result;

    switch (mColorDepth) {
        case ColorDepth::Monochrome:
            result = (((GetWidth() + 7) >> 3) * GetHeight());
            break;

        case ColorDepth::Alpha:
            result = (GetWidth() * GetHeight());
            break;

        case ColorDepth::RGB:
            result = (GetWidth() * GetHeight()) * 3;
            break;

        case ColorDepth::RGBA:
            result = (GetWidth() * GetHeight()) * 4;
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }

    return size_t(result);
}

Color PixelData::GetPixelAt(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor) const
{
    if (!GetRect().IsHit(aX, aY)) {
        return Color::None;
//        THROW_WITH_BACKTRACE1(std::out_of_range, "Pixel coordinates out of range (" + std::to_string(aX) + ", " + std::to_string(aY) + " not in " + to_string(GetRect()) + ")");
    }
    Color result(arColor);
    int offset;
    switch (mColorDepth) {
        case ColorDepth::Monochrome:
            offset = (((GetWidth() + 7) >> 3) * aY) + (aX >> 3);
            result.SetAlpha( ((mpData[offset] & (1 << (aX % 8))) > 0) ? 255 : 0);
            break;

        case ColorDepth::Alpha:
            offset = (aY * GetWidth()) + aX;
            result.SetAlpha(mpData[offset]);
            break;

        case ColorDepth::RGB:
            offset = ((aY * GetWidth()) + aX) * 3;
            result.SetRed(mpData[offset + 0]);
            result.SetGreen(mpData[offset + 1]);
            result.SetBlue(mpData[offset + 2]);
            break;

        case ColorDepth::RGBA:
            offset = ((aY * GetWidth()) + aX) * 4;
            result.FromRaw(*reinterpret_cast<const std::uint32_t*>(uintptr_t(mpData + offset)));
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }
//    DUMP(aX << ", " << aY << ", " << aColor, result << " from " << GetWidth() << ", " << GetHeight() << ", " << mColorDepth);
    return result;
}

PixelData& PixelData::SetPixelAt(GuiUnit_t aX, GuiUnit_t aY, Color aColor)
{
    if (!GetRect().IsHit(aX, aY)) {
        return *this;
//        THROW_WITH_BACKTRACE1(std::out_of_range, "Pixel coordinates out of range (" + std::to_string(aX) + "<" + std::to_string(GetWidth()) + "," + std::to_string(aY) + "<" + std::to_string(GetHeight()) + ")");
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
            offset = (((GetWidth() + 7) >> 3) * aY) + (aX >> 3);
            if (aColor.GetAlpha() > 0) {
                pdata[offset] |= (1 << (aX % 8));
            }
            else {
                pdata[offset] &= ~(1 << (aX % 8));
            }
            break;

        case ColorDepth::Alpha:
            offset = (aY * GetWidth()) + aX;
            pdata[offset] = aColor.GetAlpha();
            break;

        case ColorDepth::RGB:
            offset = ((aY * GetWidth()) + aX) * 3;
            pdata[offset + 0] = aColor.GetRed();
            pdata[offset + 1] = aColor.GetGreen();
            pdata[offset + 2] = aColor.GetBlue();
            break;

        case ColorDepth::RGBA:
            offset = ((aY * GetWidth()) + aX) * 4;
            if (!mBlend || aColor.GetAlpha() == 255) {
                *reinterpret_cast<std::uint32_t*>(uintptr_t(pdata + offset)) = aColor.AsRaw();
            }
            else {
                Color bg;
                bg.FromRaw(*reinterpret_cast<std::uint32_t*>(uintptr_t(pdata + offset)));
                *reinterpret_cast<std::uint32_t*>(uintptr_t(pdata + offset)) = Color::Blend(bg, aColor).AsRaw();
            }
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }

    return *this;
}

void PixelData::initAfterLoad(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth)
{
    mColorDepth = aDepth;
    mRect = Rect(0, 0, aWidth, aHeight);
    auto sz = GetDataSize();
    if (mData.size() != sz) {
        mData.resize(sz);
    }
    mpData = mData.data();
}

GfxCompressor::CompressionType PixelData::getCompressionType(bool aCompress) const
{
    if (!aCompress) {
        return GfxCompressor::CompressionType::None;
    }

    switch(mColorDepth) {
        case ColorDepth::Monochrome:
        case ColorDepth::Alpha:
            return GfxCompressor::CompressionType::Alpha;

        case ColorDepth::RGB:
            return GfxCompressor::CompressionType::RGB;

        case ColorDepth::RGBA:
            return GfxCompressor::CompressionType::RGBA;

        default:
            return GfxCompressor::CompressionType::None;
    }
}

GfxCompressor::CompressedData PixelData::Compress(bool aCompress) const
{
    GfxCompressor cmp;
    return cmp.Compress(getCompressionType(aCompress), mpData, GetDataSize());
}

PixelData& PixelData::Decompress(const GfxCompressor::CompressedData &arCompressedData)
{
    GfxCompressor cmp;
    mData = cmp.Decompress(arCompressedData.mType, arCompressedData.mData.data(), arCompressedData.mData.size());
    mpData = mData.data();
    return *this;
}

PixelData& PixelData::Decompress(const GfxCompressor::CompressionType aType, const uint8_t* apData, size_t aSize)
{
    GfxCompressor cmp;
    mData = cmp.Decompress(aType, apData, aSize);
    mpData = mData.data();
    return *this;
}

void PixelData::SaveToCFile(const std::filesystem::path &arFileName, bool aCompress, const char *apHeaderFile) const
{
    rsp::utils::CppObjectFile fo(arFileName);

    std::string obj_name = fo.Name();
    utils::StrUtils::ReplaceAll(obj_name, "-", "_");

    fo << "#include \"" << (apHeaderFile ? apHeaderFile : obj_name + ".h") << "\"\n\n"
        << "using namespace rsp::graphics;\n" << std::endl;

    auto result = Compress(aCompress);

    fo << "static const std::uint8_t pixdata[" << result.mData.size() << "]{\n";
    fo.Hex(result.mData.data(), result.mData.size());
    fo << "};\n\n";

    fo << "const GfxResource c" << obj_name << "{" << utils::crc32::HashConst(obj_name.c_str()) << "u, "
        << GetWidth() << "u, " << GetHeight() << "u, ColorDepth::"
        << mColorDepth << ", " << (aCompress ? "true" : "false")
        << ", " << result.mData.size() << ", pixdata};" << std::endl;

    std::ios_base::openmode mode = std::ios_base::out | (apHeaderFile ? std::ios_base::app : std::ios_base::trunc);
    std::filesystem::path hfile = arFileName;
    hfile.replace_extension("h");
    if (apHeaderFile) {
        hfile.replace_filename(apHeaderFile);
    }
    std::fstream header(hfile, mode);
    if (header.tellp() == 0) {
        header << "#include <graphics/GfxResource.h>\n" << std::endl;
    }
    header << "extern const rsp::graphics::GfxResource c" << obj_name << ";" << std::endl;
}


PixelData PixelData::ChangeColorDepth(ColorDepth aDepth, Color aColor) const
{
    if (aDepth == mColorDepth) {
        return PixelData(*this);
    }
    PixelData result(GetWidth(), GetHeight(), aDepth);
    result.CopyFrom(Point(0,0), *this, GetRect(), aColor);
    return result;
}

PixelData& PixelData::CopyFrom(const Point &arDestination, const PixelData &arOther, const Rect &arSourceRect, Color aColor)
{
    Rect r = arSourceRect & arOther.GetRect();
    if (arDestination.GetX() < 0 || arDestination.GetY() < 0) {
        r.Move(arDestination.GetX(), arDestination.GetY()); // Clip source if we are painting partial outside this
        r &= arOther.GetRect();
    }
    auto oy = arDestination.GetY();
    for (int y = r.GetTop(); y < r.GetHeight(); y++) {
        auto ox = arDestination.GetX();
        for (int x = r.GetLeft(); x < (r.GetLeft() + r.GetWidth()); x++) {
            SetPixelAt(ox, oy, arOther.GetPixelAt(x, y, aColor));
            ox++;
        }
        oy++;
    }
    return *this;
}

void PixelData::Fill(Color aColor)
{
    if (mData.size() == 0) {
        return;
    }
    switch (mColorDepth) {
        case ColorDepth::RGBA: {
            std::uint32_t *p = reinterpret_cast<std::uint32_t*>(mData.data());
            std::fill_n(p, mData.size() / sizeof(std::uint32_t), aColor.AsRaw());
            break;
        }

        case ColorDepth::Alpha:
            std::memset(mData.data(), int(aColor.GetAlpha()), mData.size());
            break;

        case ColorDepth::Monochrome:
        case ColorDepth::RGB:
        default:
            for (GuiUnit_t y=0 ; y < GetHeight() ; ++y) {
                for (GuiUnit_t x = 0 ; x < GetWidth() ; ++x) {
                    SetPixelAt(x, y, aColor);
                }
            }
            break;
    }
}

PixelData& PixelData::Fade(int aAlphaInc)
{
    if (mColorDepth != ColorDepth::RGBA) {
        return *this;
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


    for (GuiUnit_t y = 0 ; y < GetHeight() ; y++) {
        for (GuiUnit_t x = 0 ; x < GetWidth() ; x++) {
            int offset = ((y * GetWidth()) + x) * 4;
            Color col;
            col.FromRaw(*reinterpret_cast<const std::uint32_t*>(uintptr_t(pdata + offset)));
            int alpha = int(col.GetAlpha());

            if (aAlphaInc > 0) {
                alpha = std::min(alpha + aAlphaInc, 255);
            }
            else {
                alpha = std::max(alpha + aAlphaInc, 0);
            }
            col.SetAlpha(uint8_t(alpha));
            *reinterpret_cast<std::uint32_t*>(uintptr_t(pdata + offset)) = col.AsRaw();
        }
    }
    return *this;
}

bool PixelData::SetBlend(bool aValue)
{
    bool result = mBlend;
    mBlend = aValue;
    return result;
}

} /* namespace rsp::graphics */
