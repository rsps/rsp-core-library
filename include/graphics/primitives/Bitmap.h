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
    using Canvas::Canvas;

    Bitmap(const std::string &arImgName);
    Bitmap(const uint32_t *apPixels, GuiUnit_t aHeight, GuiUnit_t aWidth, PixelData::ColorDepth aDepth);

    Bitmap& Load(const std::string &arImgName);
    Bitmap& Assign(const uint32_t *apPixels, GuiUnit_t aHeight, GuiUnit_t aWidth, PixelData::ColorDepth aDepth);
    Bitmap& Assign(const PixelData &arPixelData);

protected:
    static std::unordered_map<std::string,std::function<std::shared_ptr<ImgLoader>()>> msFiletypeMap;
};

std::ostream& operator<<(std::ostream &os, const Bitmap &arBmp);

} // namespace rsp::graphics
#endif // BITMAP_H
