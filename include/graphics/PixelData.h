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

#include <graphics/Color.h>
#include <graphics/Point.h>
#include <graphics/Rect.h>
#include <cstdint>
#include <cstddef>
#include <filesystem>
#include <vector>
#include <utils/CoreException.h>
#include <logging/Logger.h>

namespace rsp::graphics {

class EIllegalColorDepth : public rsp::utils::CoreException
{
public:
    EIllegalColorDepth() : rsp::utils::CoreException("Unsupported Color Depth") {}
};


class PixelData
{
public:
    enum class ColorDepth { Monochrome = 1, Alpha = 8, RGB = 24, RGBA = 32 };

    PixelData() noexcept {}
    PixelData(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth = PixelData::ColorDepth::RGBA);
    PixelData(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const std::uint8_t *aData);

    PixelData(const PixelData& arOther);
    PixelData(const PixelData&& arOther);
    PixelData& operator=(const PixelData& arOther);
    PixelData& operator=(const PixelData&& arOther);

    PixelData& Assign(const PixelData& arOther);

    PixelData ChangeColorDepth(ColorDepth aDepth) const;

    PixelData& Init(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth, const std::uint8_t *apData);

    void Fill(Color aColor);

    uintptr_t GetId() { return uintptr_t(mpData); }

    const std::uint8_t* GetData() const { return mpData; }
    std::vector<std::uint8_t>& GetData() { return mData; }

    PixelData& SetData(const std::uint8_t *apData) { mpData = const_cast<std::uint8_t*>(apData); return *this; }

    std::size_t GetDataSize() const;

    GuiUnit_t GetWidth() const { return mWidth; }
    GuiUnit_t GetHeight() const { return mHeight; }
    ColorDepth GetColorDepth() const { return mColorDepth; }
    Rect GetRect() const { return Rect(0, 0, mWidth, mHeight); }
    bool IsHit(GuiUnit_t aX, GuiUnit_t aY) const { return (aX < mWidth && aY < mHeight); }
    bool IsHit(const Point &arPoint) const { return IsHit(arPoint.mX, arPoint.mY); }

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
    Color GetPixelAt(GuiUnit_t aX, GuiUnit_t aY, Color aColor) const;
    Color GetPixel(const Point &arPoint, Color aColor = Color::None) const { return GetPixelAt(arPoint.mX, arPoint.mY, aColor); }

    PixelData& SetPixelAt(GuiUnit_t aX, GuiUnit_t aY, Color aColor);
    PixelData& GetPixel(const Point &arPoint, Color aColor) { return SetPixelAt(arPoint.mX, arPoint.mY, aColor); }

    void SaveToCFile(const std::filesystem::path &arFileName);

protected:
    ColorDepth mColorDepth = ColorDepth::RGB;
    GuiUnit_t mWidth = 0;
    GuiUnit_t mHeight = 0;
    const std::uint8_t *mpData = nullptr;
    std::vector<std::uint8_t> mData{};

    friend class ImgLoader;
    void initAfterLoad(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth);
};

std::ostream& operator<<(std::ostream& os, const PixelData::ColorDepth arDepth);

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PIXELDATA_H_ */
