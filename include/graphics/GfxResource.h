/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_GRAPHICS_GFX_RESOURCE_H
#define RSP_CORE_LIB_GRAPHICS_GFX_RESOURCE_H

#include <cstdint>
#include <ostream>

namespace rsp::graphics {

enum class ColorDepth : uint16_t { Monochrome = 1, Alpha = 8, RGB = 24, RGBA = 32 };

std::ostream& operator<<(std::ostream& os, ColorDepth aDepth);

struct GfxResource
{
    uint32_t Id;
    uint16_t Width;
    uint16_t Height;
    ColorDepth    Depth;
    uint16_t Compressed;
    uint32_t PixDataSize;
    const uint8_t *PixData;
};

} // namespace rsp::graphics

#endif // RSP_CORE_LIB_GRAPHICS_GFX_RESOURCE_H
