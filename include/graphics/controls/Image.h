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

#include <graphics/primitives/Bitmap.h>
#include "Control.h"

namespace rsp::graphics
{

class Image : public Control
{
public:
    TYPEINFO(Image)

    Image() : Control(rsp::utils::MakeTypeInfo<Image>()) {}
    Image(const rsp::utils::TypeInfo &arInfo) : Control(arInfo) {};

    void ClearSection();
    void SetSection(const Rect &arSection);

protected:
    void paint(Canvas &arCanvas, const Style &arStyle) override;
};

} // namespace rsp::graphics
#endif // IMAGE_H
