/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_GRAPHICS_IMAGE_H
#define RSP_CORE_LIB_GRAPHICS_IMAGE_H

#include "Control.h"
#include "BitmapView.h"
#include "Texture.h"

namespace rsp::graphics
{

class Image : public Control
{
public:
    Image();

    /**
     * \brief Make Image from raw pixel data. This is a conversion constructor.
     *
     * \param arPixelData
     */
    Image(const PixelData &arPixelData); // NOLINT
    explicit Image(const BitmapView &arBitmap);
    explicit Image(BitmapView &&arBitmap);

    Image& operator=(const BitmapView &arBitmap);
    Image& operator=(const BitmapView &&arBitmap);

    BitmapView& GetBitmap() { return mBitmap; }

protected:
    BitmapView mBitmap{};
    TexturePtr_t mpTexture = nullptr;

    void doSetArea(const Rect &arRect, const Point &arOriginDifference) override;
    void update() override;
    bool render(Renderer &arRenderer) const override;
};

} // namespace rsp::graphics

#endif // RSP_CORE_LIB_GRAPHICS_IMAGE_H
