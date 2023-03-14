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

/**
 * \brief Wrapper for raster images kept in video memory for fast rendering operations.
 */
class Texture
{
public:
    Texture() {}

    virtual ~Texture() {}

    Texture(const PixelData &arPixelData, Color aColor)
        : mpImpl(Interface::Create(arPixelData, aColor))
    {
    }

    Texture(GuiUnit_t aWidth, GuiUnit_t aHeight)
        : mpImpl(Interface::Create(aWidth, aHeight))
    {
    }

    Texture(const Texture &arOther)
        : mpImpl(arOther.mpImpl->Clone())
    {
    }

    Texture& operator=(const Texture &arOther)
    {
        if (this != &arOther) {
            mpImpl = arOther.mpImpl->Clone();
        }
        return *this;
    }

    Texture(Texture &&arOther) = default;
    Texture& operator=(Texture &&arOther) = default;

    /**
     * \brief Test if texture is assigned.
     */
    operator bool() const { return bool(mpImpl); }

    /**
     * \brief Update this texture with content from the given pixel data
     * \param arPixelData
     * \param aColor Color to use if pixel data is monochrome or alpha
     * \return self
     */
    Texture& Update(const PixelData &arPixelData, Color aColor = Color::None)
    {
        mpImpl->Update(arPixelData, aColor);
        return *this;
    }

    /**
     * \brief Fill this texture with the given color
     * \param aColor
     */
    Texture& Fill(Color aColor, GfxHal::Optional<const Rect> arRect = nullptr)
    {
        mpImpl->Fill(aColor, arRect);
        return *this;
    }

    /**
     * \brief Set the blend operation to use when updating this texture. Defaults to "Copy".
     * \param aOp
     * \param aColorKey
     * \return self
     */
    Texture& SetBlendOperation(GfxBlendOperation aOp, Color aColorKey = Color::None)
    {
        mpImpl->SetBlendOperation(aOp, aColorKey);
        return *this;
    }

    /**
     * \brief Set the source area to use when reading from this texture. Defaults to entire texture.
     *
     * \param arRect
     * \return self
     */
    Texture& SetSourceRect(const Rect &arRect)
    {
        mpImpl->SetSourceRect(arRect);
        return *this;
    }

    /**
     * \brief Set the destination to use when rendering this texture. Defaults to 0,0.
     *
     * \param arRect
     * \return self
     */
    Texture& SetDestination(const Point &arPoint)
    {
        mpImpl->SetDestination(arPoint);
        return *this;
    }

    class Interface
    {
    public:
        static std::unique_ptr<Interface> Create(const PixelData &arPixelData, Color aColor);
        static std::unique_ptr<Interface> Create(GuiUnit_t aWidth, GuiUnit_t aHeight);

        virtual ~Interface() {}

        virtual void Update(const PixelData &arPixelData, Color aColor = Color::None) = 0;
        virtual void Fill(Color aColor, GfxHal::Optional<const Rect> arRect = nullptr) = 0;
        virtual void SetBlendOperation(GfxBlendOperation aOp, Color aColorKey = Color::None) = 0;
        virtual void SetSourceRect(const Rect &arRect) = 0;
        virtual void SetDestination(const Point &arPoint) = 0;

        virtual std::unique_ptr<Interface> Clone() const = 0;
    };

protected:
    std::unique_ptr<Interface> mpImpl{};
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_TEXTURE_H_ */
