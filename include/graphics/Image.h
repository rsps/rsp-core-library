/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef IMAGE_H
#define IMAGE_H

#include "Control.h"
#include "BitmapView.h"
#include "Texture.h"

namespace rsp::graphics
{

class Image : public Control
{
public:
    Image();
    Image(const PixelData &arPixelData);
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

#endif // IMAGE_H
