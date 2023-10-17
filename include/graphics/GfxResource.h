/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_GFXRESOURCE_H_
#define INCLUDE_GRAPHICS_GFXRESOURCE_H_

#include <cstdint>
#include <ostream>

namespace rsp::graphics {

enum class ColorDepth : uint16_t { Monochrome = 1, Alpha = 8, RGB = 24, RGBA = 32 };

std::ostream& operator<<(std::ostream& os, ColorDepth aDepth);

struct GfxResource
{
    std::uint32_t Id;
    std::uint16_t Width;
    std::uint16_t Height;
    ColorDepth    Depth;
    std::uint16_t Compressed;
    std::uint32_t PixDataSize;
    const std::uint8_t *PixData;
};

} // namespace rsp::graphics

#endif /* INCLUDE_GRAPHICS_GFXRESOURCE_H_ */
