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

namespace rsp::graphics
{

class Image : public Control
{
public:
    Image() {  initTypeInfo<Image>(); }

    void ClearSection();
    void SetSection(const Rect &arSection);

    const BitmapView& GetBitmap() const { return mBitmap; }
    BitmapView& GetBitmap() { return mBitmap; }

protected:
    BitmapView mBitmap{};

    void paint(Canvas &arCanvas, const Style &arStyle) override;
};

} // namespace rsp::graphics

#endif // IMAGE_H
