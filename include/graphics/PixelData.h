/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_GRAPHICS_PIXEL_DATA_H
#define RSP_CORE_LIB_GRAPHICS_PIXEL_DATA_H

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
    PixelData() = default;
    PixelData(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth = ColorDepth::RGBA);
    PixelData(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const uint8_t *aData, size_t aDataSize, bool aCompressed = false);
    explicit PixelData(const GfxResource &arResource);

    PixelData(const PixelData& arOther);
    PixelData(PixelData&& arOther) noexcept;
    PixelData& operator=(const PixelData& arOther);
    PixelData& operator=(PixelData&& arOther) noexcept;

    PixelData& CopyFrom(const Point &arDestination, const PixelData &arOther, const Rect &arSourceRect, Color aColor);

    [[nodiscard]] PixelData ChangeColorDepth(ColorDepth aDepth, Color aColor = Color::Black) const;

    PixelData& Init(uint32_t aId, GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const uint8_t *apData);

    void Fill(Color aColor);

    /**
     * \brief Adjust alpha channel on all pixels with the given value.
     * \param aAlphaInc
     * \param aFixed Set to set the alpha values to the given value, clear to add.
     * \return self
     */
    PixelData& Fade(int aAlphaInc, bool aFixed = false);

    [[nodiscard]] uint32_t GetId() const { return mId; }

    [[nodiscard]] const uint8_t* GetData() const { return mpData; }
    std::vector<uint8_t>& GetData() { return mData; }

    PixelData& SetData(const uint8_t *apData) { mpData = const_cast<uint8_t*>(apData); return *this; }

    [[nodiscard]] size_t GetDataSize() const;

    [[nodiscard]] GuiUnit_t GetWidth() const { return mRect.GetWidth(); }
    [[nodiscard]] GuiUnit_t GetHeight() const { return mRect.GetHeight(); }
    [[nodiscard]] ColorDepth GetColorDepth() const { return mColorDepth; }
    [[nodiscard]] const Rect& GetRect() const { return mRect; }
    [[nodiscard]] bool IsHit(GuiUnit_t aX, GuiUnit_t aY) const { return mRect.IsHit(aX, aY); }
    [[nodiscard]] bool IsHit(const Point &arPoint) const { return mRect.IsHit(arPoint); }

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
    [[nodiscard]] Color GetPixelAt(GuiUnit_t aX, GuiUnit_t aY, const Color& arColor = Color::Black) const;
    [[nodiscard]] Color GetPixel(const Point &arPoint, Color aColor = Color::Black) const { return GetPixelAt(arPoint.mX, arPoint.mY, aColor); }

    PixelData& SetPixelAt(GuiUnit_t aX, GuiUnit_t aY, Color aColor);
    PixelData& SetPixel(const Point &arPoint, Color aColor) { return SetPixelAt(arPoint.mX, arPoint.mY, aColor); }

    void SaveToCFile(const std::filesystem::path &arFileName, bool aCompress = false, const char *apHeaderFile = nullptr) const;

    bool SetBlend(bool aValue);

    [[nodiscard]] GfxCompressor::CompressedData Compress(bool aCompress = true) const;
    PixelData& Decompress(const GfxCompressor::CompressedData &arCompressedData);
    PixelData& Decompress(GfxCompressor::CompressionType aType, const uint8_t* apData, size_t aSize);

protected:
    ColorDepth mColorDepth = ColorDepth::RGB;
    Rect mRect{};
    const uint8_t *mpData = nullptr;
    std::vector<uint8_t> mData{};
    bool mBlend = true;
    uint32_t mId = 0;

    [[nodiscard]] GfxCompressor::CompressionType getCompressionType(bool aCompress) const;

    friend class ImgLoader;
    void initAfterLoad(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth);

    void assign(const PixelData& arOther);
    void move(PixelData&& arOther);
};

} /* namespace rsp::graphics */

#endif // RSP_CORE_LIB_GRAPHICS_PIXEL_DATA_H
