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
#include "PixelData.h"

namespace rsp::graphics {

enum class GfxBlendOperation {
    Copy,        // Copy source to destination
    SourceAlpha, // Blend source with destination, using source alpha value
    ColorKey     // Omit drawing pixels with same color value as ColorKey (transparent color)
};

/**
 * \struct VideoSurface
 * \brief Data structure with information about a 2D area in video memory
 */
struct VideoSurface
{
    using PixelPtr_t = std::unique_ptr<uint32_t[], std::function<void(uint32_t[])> >;

    PixelPtr_t mpVirtAddr{}; // Pointer to 32-bit ARGB pixels, accessible from user space
    uintptr_t mRowPitch = 0; // Number of bytes per row in physical memory (alignment vs. pixel width)
    GuiUnit_t mWidth = 0;
    GuiUnit_t mHeight = 0;
    int mRotation = 0; // Rotation of this surface. Only supports: 0, 90, 180, 270
    GfxBlendOperation mBlendOperation = GfxBlendOperation::Copy;
    uint32_t mColorKey = Color::None; // Value of transparent pixel on this surface (source key)
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
    using OptionalRect = rsp::utils::OptionalPtr<const Rect>;

    /**
     * \brief Get an instance. Can be a static factory or just a facade.
     * \return self
     */
    static GfxHal& Get();

    virtual ~GfxHal() {}

    /**
     * \brief Allocate a buffer in video memory
     *
     * This allocates the requested amount of video memory returns a VideoSurface object with
     * the attributes of allocation.
     * The mpVirtAddr unique pointer is allocated with a specialized deallocator so
     * standard smart pointer operations work on designated memory.
     * To free it, simply destroy the VideoSurface or set mpVirtAddr to null.
     *
     * \param arSurface The surface struct to fill
     * \param aWidth
     * \param aHeight
     * \return shared_ptr to VideoSurface, multiple references can be made to the same physical surface, e.g. textures used by many components.
     */
    virtual std::shared_ptr<VideoSurface> Alloc(int aWidth, int aHeight) = 0;

    /**
     * \brief Copy an area onto another
     *
     * \param arDst Destination surface
     * \param arSrc Source surface
     * \param aDstRect Optional area on destination surface, if null the entire destination is filled
     * \param aSrcRect Optional area from source surface, if null, the entire source is copied
     */
    virtual void Blit(VideoSurface &arDst, const VideoSurface &arSrc, OptionalRect aDstRect = nullptr, OptionalRect aSrcRect = nullptr) = 0;

    /**
     * \brief Copy an area of PixelData into a VideoSurface
     * \param arDst Destination surface
     * \param arPixelData
     * \param aDstRect
     * \param aSrcRect
     */
    virtual void CopyFrom(VideoSurface &arDst, const PixelData &arPixelData, uint32_t aColor, OptionalRect aDstRect = nullptr, OptionalRect aSrcRect = nullptr) = 0;

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
    virtual void Fill(VideoSurface &arDst, uint32_t aColor, OptionalRect aDest = nullptr) = 0;

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
     * \brief Execute all queued operations, returns when finished
     */
    virtual void Sync() = 0;
};


} /* namespace rsp::graphics */


#endif /* INCLUDE_GRAPHICS_GFXHAL_H_ */
