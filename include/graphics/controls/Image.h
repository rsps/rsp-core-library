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
    Image() : Control(rsp::utils::MakeTypeInfo<Image>()) {};
    Image(const rsp::utils::TypeInfo &arInfo) : Control(arInfo) {}

    Image(const Image &) = default;
    Image(Image &&) = default;

    Image& operator=(const Image &) = default;
    Image& operator=(Image &&arOther) = default;

    Image& SetArea(const Rect &arRect) override;

    void ClearSection();
    void SetSection(const Rect &arSection);

  protected:
    Rect mSection{}; // Section of bitmap to display

    void paint(Canvas &arCanvas, const Style &arStyle) override;
};

} // namespace rsp::graphics
#endif // IMAGE_H
