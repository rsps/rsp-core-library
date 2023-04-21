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
#include <graphics/GuiUnit.h>
#include <graphics/PixelData.h>

namespace rsp::graphics {

class Texture;
typedef std::unique_ptr<Texture> TexturePtr_t;
typedef std::unique_ptr<const Texture> cTexturePtr_t;

/**
 * \brief Wrapper for raster images kept in video memory for fast rendering operations.
 */
class Texture
{
public:
    enum class BlendOperation {
        Copy,        // Copy source to destination
        SourceAlpha, // Blend source with destination, using source alpha value
        ColorKey     // Omit drawing pixels with same color value as ColorKey (transparent color)
    };

    static TexturePtr_t Create(const PixelData &arPixelData, const Color &arColor = Color::None, Point aDestPos = {0,0}, Point aDestOffset = {0,0});
    static TexturePtr_t Create(GuiUnit_t aWidth, GuiUnit_t aHeight, Point aDestPos = {0,0}, Point aDestOffset = {0,0});

    virtual ~Texture() {}

    virtual GuiUnit_t GetHeight() const = 0;
    virtual GuiUnit_t GetWidth() const = 0;

    /**
     * \brief Fill this texture with the given color
     * \param aColor
     */
    virtual Texture& Fill(const Color &arColor, OptionalRect arRect = nullptr) = 0;

    /**
     * \brief Update this texture with content from the given pixel data
     * \param arPixelData
     * \param aColor Color to use if pixel data is monochrome or alpha
     * \return self
     */
    virtual Texture& Update(const PixelData &arPixelData, const Color &arColor = Color::None) = 0;

    /**
     * \brief Set the blend operation to use when updating this texture. Defaults to "Copy".
     * \param aOp
     * \param aColorKey
     * \return self
     */
    virtual Texture& SetBlendOperation(BlendOperation aOp, const Color &arColorKey = Color::None) = 0;

    /**
     * \brief Set/get the source area to use when reading from this texture. Defaults to entire texture.
     *
     * \param arRect
     * \return self
     */
    virtual Texture& SetSourceRect(const Rect &arRect) = 0;
    virtual const Rect& GetSourceRect() const = 0;

    /**
     * \brief Set/get the destination to use when rendering this texture. Defaults to 0,0.
     *
     * \param arRect
     * \return self
     */
    virtual Texture& SetDestination(const Point &arPoint) = 0;
    virtual Point GetDestination() const = 0;
    virtual Texture& SetOffset(const Point &arPoint) = 0;
    virtual Rect GetDestinationRect() const = 0;

    /**
     * \brief Make a safe copy of this texture, possibly referencing the same internal raster storage.
     * \return Texture ptr
     */
    virtual TexturePtr_t Clone() const = 0;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_TEXTURE_H_ */
