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
#include "Color.h"
#include "GuiUnit.h"
#include "Rect.h"

namespace rsp::graphics {

/**
 * \class OptionalPtr
 * \brief Helper to support optional pointers or references
 *
 * \tparam T
 */
template <class T>
class OptionalPtr
{
public:
    OptionalPtr(const T* apValue = nullptr) : mPtr(apValue) {}
    OptionalPtr(const T &aValue) : mPtr(&aValue) {}
    operator bool() const { return (mPtr); }
    const T& operator->() const { return *mPtr; }
    const T& operator*() const { return *mPtr; }
protected:
    const T* mPtr;
};

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
    AlphaKey     // Omit drawing pixels with same color value as ColorKey (transparent color)
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
    virtual void Blit(VideoSurface &arDst, const VideoSurface &arSrc, OptionalPtr<Rect> aDstRect = nullptr, OptionalPtr<Rect> aSrcRect = nullptr) = 0;

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
    virtual void Fill(VideoSurface &arDst, uint32_t aColor, OptionalPtr<Rect> aDest = nullptr) = 0;

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
