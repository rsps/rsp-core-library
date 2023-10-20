/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021-2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_GRAPHICS_BITMAP_H
#define RSP_CORE_LIB_GRAPHICS_BITMAP_H

#include <graphics/Canvas.h>
#include <graphics/raster/ImgLoader.h>
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
    using Canvas::Canvas;

    explicit Bitmap(const std::string &arImgName);
    Bitmap(const uint32_t *apPixels, GuiUnit_t aHeight, GuiUnit_t aWidth, ColorDepth aDepth);
    explicit Bitmap(const PixelData &arPixelData);

    Bitmap& Load(const std::string &arImgName);
    Bitmap& Assign(const uint32_t *apPixels, GuiUnit_t aHeight, GuiUnit_t aWidth, ColorDepth aDepth);
    Bitmap& Assign(const PixelData &arPixelData);

protected:
    static std::unordered_map<std::string,std::function<std::shared_ptr<ImgLoader>()>> msFiletypeMap;
};

std::ostream& operator<<(std::ostream &os, const Bitmap &arBmp);

} // namespace rsp::graphics
#endif // RSP_CORE_LIB_GRAPHICS_BITMAP_H
