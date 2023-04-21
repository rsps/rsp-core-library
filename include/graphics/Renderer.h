/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_RENDERER_H_
#define INCLUDE_GRAPHICS_RENDERER_H_

#include <functional>
#include <memory>
#include <optional>
#include "Color.h"
#include "GuiUnit.h"
#include "PixelData.h"
#include "Rect.h"
#include "Texture.h"

namespace rsp::graphics {

/**
 * \class Renderer
 * \brief A Renderer represents a presentation surface, e.g. a display device.
 *
 * Content is drawn on the display by using the drawing operations in this class,
 * and made visible on the display device by calling the Present method.
 *
 */
class Renderer
{
public:
    /**
     * \brief Create the default renderer instance
     * \param aWidth
     * \param aHeight
     * \return self
     */
    static Renderer& Init(GuiUnit_t aWidth, GuiUnit_t aHeight);

    /**
     * \brief Get the default Renderer instance
     *
     * \return self
     */
    static Renderer& Get();

    virtual ~Renderer() {}

    virtual GuiUnit_t GetHeight() const = 0;
    virtual GuiUnit_t GetWidth() const = 0;
    virtual ColorDepth GetColorDepth() const = 0;

    /**
     * \brief Draw a rectangle of width 1px
     *
     * \param aColor
     * \param arRect
     * \return self
     */
    virtual Renderer& DrawRect(const Color &arColor, const Rect &arRect) = 0;

    /**
     * \brief Fill the presentation surface with the given color
     *
     * \param aColor
     * \param aDestination Optional area on the surface to fill
     * \return self
     */
    virtual Renderer& Fill(const Color &arColor, OptionalRect aDestination = nullptr) = 0;

    /**
     * \brief Copy the given texture onto this presentation surface
     *
     * \param arTexture
     * \return self
     */
    virtual Renderer& Blit(const Texture &arTexture) = 0;

    /**
     * \brief Limit the drawing area on this presentation surface
     *
     * \param aClipRect
     * \return
     */
    virtual Renderer& SetClipRect(const Rect &arClipRect) = 0;

    /**
     * \brief Make sure the presentation surface is visible on the display device.
     *
     * Mainly used to wait for acceleration hardware to finish and setting view-port
     * on multi-buffer architectures.
     */
    virtual void Present() = 0;

    /**
     * \brief Sets a single pixel to the given Color
     * \param aPoint Reference to the coordinate for the pixel to be set
     * \param aColor Reference to the color the pixel is set to
     */
    virtual Renderer& SetPixel(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor) = 0;

    /**
     * \brief Gets a single pixel to the given Color
     * \param aPoint Reference to the coordinate for the pixel to get
     * \param aFront Gets pixels from the backbuffer by default, set true to read front buffer
     */
    virtual Color GetPixel(GuiUnit_t aX, GuiUnit_t aY, bool aFront = false) const = 0;
    Color GetPixel(const Point &arPoint, bool aFront = false) const { return GetPixel(arPoint.GetX(), arPoint.GetY(), aFront); }
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_RENDERER_H_ */
