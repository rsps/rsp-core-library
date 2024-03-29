/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef BITMAP_H
#define BITMAP_H

#include <graphics/primitives/Canvas.h>
#include <graphics/primitives/raster/ImgLoader.h>
#include <memory>

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace rsp::graphics
{

/**
 * Class Bitmap
 *
 * The Bitmap is an object wrapper around raster images.
 * Various raster image formats can be implemented by descending specialized loaders
 * from the ImgLoader class and adding those to the GetRasterLoader method.
 */
class Bitmap: public Canvas
{
public:
    Bitmap() {}

    /**
     * \brief Load bitmap from given file.
     *
     * \param aImgName
     */
    Bitmap(const std::string &arImgName);

    /**
     * \brief Create a bitmap of given pixel data
     *
     * \param apPixels
     * \param aHeight
     * \param aWidth
     * \param aBytesPerPixel
     */
    Bitmap(const uint32_t *apPixels, GuiUnit_t aHeight, GuiUnit_t aWidth, unsigned int aBytesPerPixel);

    /**
     * \brief Create an empty in memory bitmap
     *
     * \param aHeight
     * \param aWidth
     * \param aBytesPerPixel
     */
    Bitmap(GuiUnit_t aHeight, GuiUnit_t aWidth, unsigned int aBytesPerPixel);

    Bitmap& Load(const std::string &arImgName);
    Bitmap& Assign(const uint32_t *apPixels, GuiUnit_t aHeight, GuiUnit_t aWidth, unsigned int aBytesPerPixel);
    Bitmap& Assign(const PixelData &arPixelData);


    inline void SetPixel(const Point &arPoint, const Color &arColor) override
    {
        if (!IsInsideCanvas(arPoint)) {
            return;
        }
        mImagePixels.SetPixelAt(arPoint.GetX(), arPoint.GetY(), arColor);
    }

    std::uint32_t GetPixel(const Point &aPoint, const bool aFront = false) const;

    /**
     * \brief Get a read only reference to the pixel data.
     *
     * \return const std::vector<uint32_t>&
     */
    const PixelData& GetPixelData() const
    {
        return mImagePixels;
    }

protected:
    static std::unordered_map<std::string,std::function<std::shared_ptr<ImgLoader>()>> msFiletypeMap;
    PixelData mImagePixels { };
};

std::ostream& operator<<(std::ostream &os, const Bitmap &arBmp);

} // namespace rsp::graphics
#endif // BITMAP_H
