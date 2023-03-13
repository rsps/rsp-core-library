/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_TEXTURE_H_
#define INCLUDE_GRAPHICS_TEXTURE_H_

#include <memory>
#include <graphics/Color.h>
#include <graphics/GfxHal.h>

namespace rsp::graphics {

/**
 * \brief Interface for raster images kept in video memory for fast rendering operations.
 */
class Texture
{
public:
    static std::shared_ptr<Texture> Create(const PixelData &arPixelData, Color aColor);
    static std::shared_ptr<Texture> Create(GuiUnit_t aWidth, GuiUnit_t aHeight);

    virtual ~Texture() {}

    /**
     * \brief Update this texture with content from the given pixel data
     * \param arPixelData
     * \param aColor Color to use if pixel data is monochrome or alpha
     * \return self
     */
    virtual void Update(const PixelData &arPixelData, Color aColor) = 0;

    /**
     * \brief Fill this texture with the given color
     * \param aColor
     */
    virtual void Fill(Color aColor, GfxHal::Optional<const Rect> arRect = nullptr) = 0;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_TEXTURE_H_ */
