/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef IMAGE_H
#define IMAGE_H

#include "graphics/controls/Control.h"
#include "graphics/primitives/Bitmap.h"

namespace rsp::graphics
{

class Image : public Control
{
  public:
    Image() : Control(){};
    Image(std::string aNormal, std::string aPressed, Rect &aRect);
    ~Image();

    /**
     * \brief Override method for rendering the object
     * \param aCanvas The canvas the object is rendered on
     */
    void Render(Canvas &aCanvas) override;

    /**
     * \brief Handles the callback from a touchable area
     * \param aCanvas The canvas the object is rendered on
     */
    void HandleCallback(States aState);

  protected:
    std::map<States, Bitmap> mStateMap{};
};

} // namespace rsp::graphics
#endif // IMAGE_H
