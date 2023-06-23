/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_PIXELDATA_H_
#define INCLUDE_GRAPHICS_PIXELDATA_H_

#include <exceptions/CoreException.h>
#include <graphics/Color.h>
#include <graphics/Point.h>
#include <graphics/Rect.h>
#include <graphics/GfxCompressor.h>
#include <graphics/GfxResource.h>
#include <cstdint>
#include <cstddef>
#include <filesystem>
#include <vector>
#include <logging/Logger.h>

namespace rsp::graphics {

class EIllegalColorDepth : public exceptions::CoreException
{
public:
    EIllegalColorDepth() : CoreException("Unsupported Color Depth") {}
};


class PixelData
{
public:
    PixelData() noexcept {}
    PixelData(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth = ColorDepth::RGBA);
    PixelData(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const std::uint8_t *aData, size_t aDataSize, bool aCompressed = false);
    PixelData(const GfxResource &arResource);

    PixelData(const PixelData& arOther);
    PixelData(PixelData&& arOther);
    PixelData& operator=(const PixelData& arOther);
    PixelData& operator=(PixelData&& arOther);

    PixelData& CopyFrom(const Point &arDestination, const PixelData &arOther, const Rect &arSourceRect, Color aColor);

    PixelData ChangeColorDepth(ColorDepth aDepth, Color aColor = Color::Black) const;

    PixelData& Init(uint32_t aId, GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const std::uint8_t *apData);

    void Fill(Color aColor);

    uint32_t GetId() const { return mId; }

    const std::uint8_t* GetData() const { return mpData; }
    std::vector<std::uint8_t>& GetData() { return mData; }

    PixelData& SetData(const std::uint8_t *apData) { mpData = const_cast<std::uint8_t*>(apData); return *this; }

    size_t GetDataSize() const;

    GuiUnit_t GetWidth() const { return mRect.GetWidth(); }
    GuiUnit_t GetHeight() const { return mRect.GetHeight(); }
    ColorDepth GetColorDepth() const { return mColorDepth; }
    const Rect& GetRect() const { return mRect; }
    bool IsHit(GuiUnit_t aX, GuiUnit_t aY) const { return mRect.IsHit(aX, aY); }
    bool IsHit(const Point &arPoint) const { return mRect.IsHit(arPoint); }

    /**
     * \brief Get the color value for the pixel at given position
     *
     * Monochrome and Alpha depths change the alpha channel on the given color.
     * RGB depth uses the alpha channel from the given color.
     *
     * \param aX
     * \param aY
     * \param aColor
     * \return Color
     */
    Color GetPixelAt(GuiUnit_t aX, GuiUnit_t aY, const Color& arColor = Color::Black) const;
    Color GetPixel(const Point &arPoint, Color aColor = Color::Black) const { return GetPixelAt(arPoint.mX, arPoint.mY, aColor); }

    PixelData& SetPixelAt(GuiUnit_t aX, GuiUnit_t aY, Color aColor);
    PixelData& SetPixel(const Point &arPoint, Color aColor) { return SetPixelAt(arPoint.mX, arPoint.mY, aColor); }

    void SaveToCFile(const std::filesystem::path &arFileName, bool aCompress = false, const char *apHeaderFile = nullptr) const;

    bool SetBlend(bool aValue);

    GfxCompressor::CompressedData Compress(bool aCompress = true) const;
    PixelData& Decompress(const GfxCompressor::CompressedData &arCompressedData);
    PixelData& Decompress(const GfxCompressor::CompressionType aType, const uint8_t* apData, size_t aSize);

protected:
    ColorDepth mColorDepth = ColorDepth::RGB;
    Rect mRect{};
    const std::uint8_t *mpData = nullptr;
    std::vector<std::uint8_t> mData{};
    bool mBlend = true;
    uint32_t mId = 0;

    GfxCompressor::CompressionType getCompressionType(bool aCompress) const;

    friend class ImgLoader;
    void initAfterLoad(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth);

    void assign(const PixelData& arOther);
    void move(PixelData&& arOther);
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PIXELDATA_H_ */
