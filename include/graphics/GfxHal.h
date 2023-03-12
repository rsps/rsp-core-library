/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_GFXHAL_H_
#define INCLUDE_GRAPHICS_GFXHAL_H_

#include <stdint.h>
#include <memory>
#include <functional>
#include <utils/OptionalPtr.h>
#include "Color.h"
#include "GuiUnit.h"
#include "Rect.h"

namespace rsp::graphics {

/**
 * \struct VideoSurface
 * \brief Data structure with information about a 2D area in video memory
 */
struct VideoSurface
{
    std::unique_ptr<uint32_t[]> mpPhysAddr{}; // Pointer to 32-bit ARGB pixels
    uintptr_t mRowPitch = 0; // Number of bytes per row in physical memory (alignment vs. width)
    GuiUnit_t mWidth = 0;
    GuiUnit_t mHeight = 0;
    int mRotation = 0; // Rotation of this surface. Only supports: 0, 90, 180, 270
};

enum class GfxBlendOperation {
    Copy,        // Copy source to destination
    SourceAlpha, // Blend source with destination, using source alpha value
    ColorKey     // Omit drawing pixels with same color value as ColorKey (transparent color)
};

/**
 * \class GfxHal
 * \brief Low level abstraction layer for simple graphic operations.
 *
 * Depending on the underlying hardware, the graphic operations can
 * be queued and only performed when the Sync() method is called.
 */
class GfxHal
{
public:
    template <class T>
    using Optional = rsp::utils::OptionalPtr<T>;

    /**
     * \brief Get an instance. Can be a static factory or just a facade.
     * \return self
     */
    static GfxHal& Get();

    virtual ~GfxHal() {}

    /**
     * \brief Allocate a buffer in video memory
     *
     * This fills out the attributes of the given VideoSurface.
     * The mpPhysAddr pointer is allocated with a specialized deallocator so
     * standard smart pointer operations work on designated memory.
     * To free it, simply destroy the VideoSurface or set mpPhysAddr to null.
     *
     * \param arSurface The surface struct to fill
     * \param aWidth
     * \param aHeight
     */
    virtual void Alloc(VideoSurface &arSurface, int aWidth, int aHeight) = 0;

    /**
     * \brief Copy an area onto another
     *
     * \param arDst Destination surface
     * \param arSrc Source surface
     * \param aDstRect Optional area on destination surface, if null the entire destination is filled
     * \param aSrcRect Optional area from source surface, if null, the entire source is copied
     */
    virtual void Blit(VideoSurface &arDst, const VideoSurface &arSrc, Optional<Rect> aDstRect = nullptr, Optional<Rect> aSrcRect = nullptr) = 0;

    /**
     * \brief Draw a rectangle with single pixel line width in a given color
     *
     * \param arDst Destination surface
     * \param aColor
     * \param arRect
     */
    virtual void DrawRect(VideoSurface &arDst, uint32_t aColor, const Rect &arRect) = 0;

    /**
     * \brief Fill an area with the given color
     *
     * \param arDst Destination surface
     * \param aColor
     * \param aDest Optional area to fill on destination, if not given the entire surface is filled
     */
    virtual void Fill(VideoSurface &arDst, uint32_t aColor, Optional<Rect> aDest = nullptr) = 0;

    /**
     * \brief Set the value of a single pixel, with respect to the selected blend operation.
     * \param arSurface Destination surface
     * \param aX
     * \param aY
     * \param aColor
     */
    virtual void SetPixel(VideoSurface &arSurface, GuiUnit_t aX, GuiUnit_t aY, uint32_t aColor) = 0;

    /**
     * \brief Get the value of a single pixel.
     * \param arSurface Source surface
     * \param aX
     * \param aY
     * \return Color value
     */
    virtual uint32_t GetPixel(const VideoSurface &arSurface, GuiUnit_t aX, GuiUnit_t aY, bool aFrontBuffer = false) const = 0;


    /**
     * \brief Set the blend operation to use during drawing operations
     * \param aOp
     */
    virtual void SetBlendOperation(GfxBlendOperation aOp) = 0;

    /**
     * \brief Set the color to use if blend operation is set to a *Key operation
     * \param aColor
     */
    virtual void SetColorKey(uint32_t aColor) = 0;

    /**
     * \brief Execute all queued operations, returns when finished
     */
    virtual void Sync() = 0;

protected:
    GfxHal() {}
    GfxHal(const GfxHal&) = delete;
    GfxHal(GfxHal&&) = delete;
    GfxHal& operator=(const GfxHal&) = delete;
    GfxHal& operator=(GfxHal&&) = delete;
};



} /* namespace rsp::graphics */


#endif /* INCLUDE_GRAPHICS_GFXHAL_H_ */
