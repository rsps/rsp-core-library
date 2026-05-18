/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cassert>
#include <cstring>
#include <graphics/PixelData.h>
#include <utils/Crc32.h>
#include <utils/CppObjectFile.h>
#include <utils/StrUtils.h>

namespace {

constexpr uint32_t* assume_aligned_u32(uint8_t* p)
{
    assert((reinterpret_cast<uintptr_t>(p) % alignof(uint32_t)) == 0);
    return static_cast<uint32_t*>(static_cast<void*>(p));
}

constexpr const uint32_t* assume_aligned_u32(const uint8_t* p)
{
    assert((reinterpret_cast<uintptr_t>(p) % alignof(uint32_t)) == 0);
    return static_cast<const uint32_t*>(static_cast<const void*>(p));
}

} // namespace

namespace rsp::graphics {

std::ostream& operator<<(std::ostream& os, const ColorDepth aDepth)
{
    switch (aDepth) {
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


PixelData::PixelData(const GuiUnit_t aWidth, const GuiUnit_t aHeight, const ColorDepth aDepth, const uint8_t *apData, const size_t aDataSize, const bool aCompressed)
    : mColorDepth(aDepth),
      mRect(0, 0, aWidth, aHeight),
      mpData(apData)
{
    if (aCompressed) {
        Decompress(getCompressionType(aCompressed), mpData, aDataSize);
    }
    mId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(mpData));
}

PixelData::PixelData(const GuiUnit_t aWidth, const GuiUnit_t aHeight, const ColorDepth aDepth)
    : mColorDepth(aDepth),
      mRect(0, 0, aWidth, aHeight)
{
    mData.resize(GetDataSize());
    mpData = mData.data();
    mId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(mpData));
}

PixelData::PixelData(const PixelData &arOther)
{
    assign(arOther);
}

PixelData::PixelData(PixelData &&arOther) noexcept
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

PixelData& PixelData::operator=(PixelData &&arOther) noexcept
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
    if (!mData.empty()) {
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
    mRect = arOther.mRect;
    mData = std::move(arOther.mData);
    if (!mData.empty()) {
        mpData = mData.data();
    }
    else {
        mpData = arOther.mpData;
    }
}


PixelData& PixelData::Init(const uint32_t aId, const GuiUnit_t aWidth, const GuiUnit_t aHeight, const ColorDepth aDepth, const uint8_t *apData)
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
    }

    return static_cast<size_t>(result);
}

Color PixelData::GetPixelAt(const GuiUnit_t aX, const GuiUnit_t aY, const Color &arColor) const
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
            result.SetAlpha( ((mpData[offset] & (1 << (aX % 8))) != 0) ? 255 : 0);
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
            result.FromRaw(*assume_aligned_u32(mpData + offset));
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
    }
//    DUMP(aX << ", " << aY << ", " << aColor, result << " from " << GetWidth() << ", " << GetHeight() << ", " << mColorDepth);
    return result;
}

PixelData& PixelData::SetPixelAt(const GuiUnit_t aX, const GuiUnit_t aY, const Color& arColor)
{
    if (!GetRect().IsHit(aX, aY)) {
        return *this;
//        THROW_WITH_BACKTRACE1(std::out_of_range, "Pixel coordinates out of range (" + std::to_string(aX) + "<" + std::to_string(GetWidth()) + "," + std::to_string(aY) + "<" + std::to_string(GetHeight()) + ")");
    }

    if (mData.empty()) {
        // Copy const data to internal mData buffer
        mData.resize(GetDataSize());
        for (auto &v : mData) {
            v = *mpData;
            mpData++;
        }
        mpData = mData.data();
    }
    uint8_t *p_data = mData.data();

//    Color result;
    int offset;
    switch (mColorDepth) {
        case ColorDepth::Monochrome:
            offset = (((GetWidth() + 7) >> 3) * aY) + (aX >> 3);
            if (arColor.GetAlpha() > 0) {
                p_data[offset] |= (1 << (aX % 8));
            }
            else {
                p_data[offset] &= ~(1 << (aX % 8));
            }
            break;

        case ColorDepth::Alpha:
            offset = (aY * GetWidth()) + aX;
            p_data[offset] = arColor.GetAlpha();
            break;

        case ColorDepth::RGB:
            offset = ((aY * GetWidth()) + aX) * 3;
            p_data[offset + 0] = arColor.GetRed();
            p_data[offset + 1] = arColor.GetGreen();
            p_data[offset + 2] = arColor.GetBlue();
            break;

        case ColorDepth::RGBA:
            offset = ((aY * GetWidth()) + aX) * 4;
            if (!mBlend || arColor.GetAlpha() == 255) {
                *assume_aligned_u32(p_data + offset) = arColor.AsRaw();
            }
            else {
                Color bg;
                bg.FromRaw(*assume_aligned_u32(p_data + offset));
                *assume_aligned_u32(p_data + offset) = Color::Blend(bg, arColor).AsRaw();
            }
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
    }

    return *this;
}

void PixelData::initAfterLoad(const GuiUnit_t aWidth, const GuiUnit_t aHeight, const ColorDepth aDepth)
{
    mColorDepth = aDepth;
    mRect = Rect(0, 0, aWidth, aHeight);
    auto sz = GetDataSize();
    if (mData.size() != sz) {
        mData.resize(sz);
    }
    mpData = mData.data();
}

GfxCompressor::CompressionType PixelData::getCompressionType(const bool aCompress) const
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

GfxCompressor::CompressedData PixelData::Compress(const bool aCompress) const
{
    return GfxCompressor::Compress(getCompressionType(aCompress), mpData, GetDataSize());
}

PixelData& PixelData::Decompress(const GfxCompressor::CompressedData &arCompressedData)
{
    mData = GfxCompressor::Decompress(arCompressedData.mType, arCompressedData.mData.data(), arCompressedData.mData.size());
    mpData = mData.data();
    return *this;
}

PixelData& PixelData::Decompress(const GfxCompressor::CompressionType aType, const uint8_t* apData, const size_t aSize)
{
    mData = GfxCompressor::Decompress(aType, apData, aSize);
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

    const auto [c_type, c_data] = Compress(aCompress);

    fo << "static const uint8_t pix_data[" << c_data.size() << "]{\n";
    fo.Hex(c_data.data(), c_data.size());
    fo << "};\n\n";

    fo << "const GfxResource c" << obj_name << "{" << utils::crc32::HashConst(obj_name) << "u, "
        << GetWidth() << "u, " << GetHeight() << "u, ColorDepth::"
        << mColorDepth << ", " << (aCompress ? "true" : "false")
        << ", " << c_data.size() << ", pix_data};" << std::endl;

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


PixelData PixelData::ChangeColorDepth(const ColorDepth aDepth, const Color& arColor) const
{
    if (aDepth == mColorDepth) {
        return {*this};
    }
    PixelData result(GetWidth(), GetHeight(), aDepth);
    result.CopyFrom(Point(0,0), *this, GetRect(), arColor);
    return result;
}

PixelData& PixelData::CopyFrom(const Point &arDestination, const PixelData &arOther, const Rect &arSourceRect, const Color& arColor)
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
            SetPixelAt(ox, oy, arOther.GetPixelAt(x, y, arColor));
            ox++;
        }
        oy++;
    }
    return *this;
}

void PixelData::Fill(const Color& arColor)
{
    if (mData.empty()) {
        return;
    }
    switch (mColorDepth) {
        case ColorDepth::RGBA: {
            std::fill_n(assume_aligned_u32(mData.data()), mData.size() / sizeof(uint32_t), arColor.AsRaw());
            break;
        }

        case ColorDepth::Alpha:
            std::memset(mData.data(), static_cast<int>(arColor.GetAlpha()), mData.size());
            break;

        case ColorDepth::Monochrome:
        case ColorDepth::RGB:
        default:
            for (GuiUnit_t y=0 ; y < GetHeight() ; ++y) {
                for (GuiUnit_t x = 0 ; x < GetWidth() ; ++x) {
                    SetPixelAt(x, y, arColor);
                }
            }
            break;
    }
}

PixelData& PixelData::Fade(const int aAlphaInc, const bool aFixed)
{
    if (mColorDepth != ColorDepth::RGBA) {
        return *this;
    }

    if (mData.empty()) {
        // Copy const data to internal mData buffer
        mData.resize(GetDataSize());
        for (auto &v : mData) {
            v = *mpData;
            mpData++;
        }
        mpData = mData.data();
    }
    uint8_t *p_data = mData.data();

    for (GuiUnit_t y = 0 ; y < GetHeight() ; y++) {
        for (GuiUnit_t x = 0 ; x < GetWidth() ; x++) {
            const int offset = ((y * GetWidth()) + x) * 4;
            Color col;
            col.FromRaw(*assume_aligned_u32(p_data + offset));
            if (col == Color::None) {
                continue;
            }
            int alpha = static_cast<int>(col.GetAlpha());

            if (aFixed) {
                alpha = aAlphaInc;
            }
            else if (aAlphaInc > 0) {
                alpha = std::min(alpha + aAlphaInc, 255);
            }
            else {
                alpha = std::max(alpha + aAlphaInc, 0);
            }
            col.SetAlpha(static_cast<uint8_t>(alpha));
            *assume_aligned_u32(p_data + offset) = col.AsRaw();
        }
    }
    return *this;
}

bool PixelData::SetBlend(const bool aValue)
{
    const bool result = mBlend;
    mBlend = aValue;
    return result;
}

} /* namespace rsp::graphics */
