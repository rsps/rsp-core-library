/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_PRIMITIVES_TEXTURE_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_TEXTURE_H_

#include <memory>
#include "Color.h"
#include "PixelData.h"

namespace rsp::graphics {

class Renderer;

/**
 * \brief Raster image in video memory for fast rendering operations.
 */
class Texture
{
public:
    Texture() {}
    Texture(const Renderer &arRenderer, const PixelData &arPixelData);
    Texture(Texture &&arOther);
    virtual ~Texture() {}

    Texture& operator=(Texture &&arOther);
    Texture& operator=(const PixelData &arPixelData);

protected:
    std::unique_ptr<Texture> mpImpl{};

    virtual void assign(const PixelData &arPixelData);
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_TEXTURE_H_ */
