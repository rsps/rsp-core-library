/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_PRIMITIVES_PIXELDATA_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_PIXELDATA_H_

#include <cstdint>
#include <cstddef>
#include <filesystem>
#include <vector>
#include <graphics/primitives/Color.h>
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
    PixelData(unsigned int aWidth, unsigned int aHeight, ColorDepth aDepth);
    PixelData(unsigned int aWidth, unsigned int aHeight, ColorDepth aDepth, const std::uint8_t *aData);

    PixelData(const PixelData& arOther);
    PixelData(const PixelData&& arOther);
    PixelData& operator=(const PixelData& arOther);
    PixelData& operator=(const PixelData&& arOther);

    PixelData ChangeColorDepth(ColorDepth aDepth) const;

    PixelData& Init(unsigned int aWidth, unsigned int aHeight, ColorDepth aDepth, const std::uint8_t *apData);

    const std::uint8_t* GetData() const { return mpData; }
    std::vector<std::uint8_t>& GetData() { return mData; }

    PixelData& SetData(const std::uint8_t *apData) { mpData = const_cast<std::uint8_t*>(apData); return *this; }

    std::size_t GetDataSize() const;

    unsigned int GetWidth() const { return mWidth; }
    unsigned int GetHeight() const { return mHeight; }
    ColorDepth GetColorDepth() const { return mColorDepth; }

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
    Color GetPixelAt(unsigned int aX, unsigned int aY, Color aColor) const;
    Color GetPixelAt(int aX, int aY, Color aColor) const { return GetPixelAt(static_cast<unsigned int>(aX), static_cast<unsigned int>(aY), aColor); }

    PixelData& SetPixelAt(unsigned int aX, unsigned int aY, Color aColor);
    PixelData& SetPixelAt(int aX, int aY, Color aColor) { return SetPixelAt(static_cast<unsigned int>(aX), static_cast<unsigned int>(aY), aColor); }

    void SaveToCFile(const std::filesystem::path &arFileName);

protected:
    ColorDepth mColorDepth = ColorDepth::RGB;
    unsigned int mWidth = 0;
    unsigned int mHeight = 0;
    const std::uint8_t *mpData = nullptr;
    std::vector<std::uint8_t> mData{};

    friend class ImgLoader;
    void initAfterLoad(unsigned int aWidth, unsigned int aHeight, ColorDepth aDepth);
};

std::ostream& operator<<(std::ostream& os, const PixelData::ColorDepth arDepth);

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_PIXELDATA_H_ */
