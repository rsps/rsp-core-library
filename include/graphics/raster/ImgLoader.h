/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_GRAPHICS_RASTER_IMG_LOADER_H
#define RSP_CORE_LIB_GRAPHICS_RASTER_IMG_LOADER_H

#include <exceptions/CoreException.h>
#include <graphics/PixelData.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>

namespace rsp::graphics
{

class ImgLoaderException: public exceptions::CoreException
{
public:
    explicit ImgLoaderException(const std::string &arMsg)
        : CoreException(arMsg)
    {
    }
};

class ECorruptedFile : public ImgLoaderException
{
public:
    explicit ECorruptedFile(const std::string &arMsg)
        : ImgLoaderException(arMsg)
    {
    }
};

class EUnsupportedFileFormat : public ImgLoaderException
{
public:
    explicit EUnsupportedFileFormat(const std::string &arMsg)
        : ImgLoaderException(arMsg)
    {
    }
};


class ImgLoader
{
  public:
    static std::shared_ptr<ImgLoader> GetRasterLoader(const std::string &arFileType);

    ImgLoader() noexcept = default;
    virtual ~ImgLoader() = default;

    /**
     * \brief Abstract methos for loading an image into memory as a bitmap
     * \param aImgName The relative path to the image
     */
    virtual void LoadImg(const std::string &aImgName) = 0;

    /**
     * \brief Get the bitmap of the loaded image
     * \return A reference to the vector holding the pixel values
     */
    [[nodiscard]] const PixelData& GetPixelData() const
    {
        return mPixelData;
    }

    /**
     * \brief Get the height of the bitmap
     * \return Value of the height
     */
    [[nodiscard]] GuiUnit_t GetHeight() const
    {
        return mPixelData.GetHeight();
    }

    /**
     * \brief Get the width of the bitmap
     * \return Value of the width
     */
    [[nodiscard]] GuiUnit_t GetWidth() const
    {
        return mPixelData.GetWidth();
    }

  protected:
    PixelData mPixelData{};

    void initAfterLoad(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth);
};

} // namespace rsp::graphics

#endif // RSP_CORE_LIB_GRAPHICS_RASTER_IMG_LOADER_H
