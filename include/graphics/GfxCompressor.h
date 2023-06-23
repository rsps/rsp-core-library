/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_GFXCOMPRESSOR_H_
#define INCLUDE_GRAPHICS_GFXCOMPRESSOR_H_

#include <cstdint>
#include <cstddef>
#include <vector>

namespace rsp::graphics {

enum class GfxCompression {None, Alpha, RGB, RGBA};

/**
 * \brief Interface for graphic compression algorithm
 */
class GfxCompressor
{
public:
    using size_type = size_t;
    using data_type = std::uint8_t;

    struct Result {
        const data_type *mpData;
        size_type mSize;
    };

    GfxCompressor(GfxCompression aType) : mType(aType) {}
    const Result Compress(const data_type* apData, size_type aSize);
    const Result Decompress(const data_type* apData, size_type aSize);

protected:
    GfxCompression mType;
    std::vector<data_type> mData{};

    const Result alphaCompress(const data_type* apData, size_t aSize);
    const Result alphaDecompress(const data_type* apData, size_t aSize);

    const Result rgbCompress(const data_type* apData, size_t aSize);
    const Result rgbDecompress(const data_type* apData, size_t aSize);

    const Result rgbaCompress(const data_type* apData, size_t aSize);
    const Result rgbaDecompress(const data_type* apData, size_t aSize);
};

}; // namespace rsp::graphics

#endif /* INCLUDE_GRAPHICS_GFXCOMPRESSOR_H_ */
