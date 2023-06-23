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

/**
 * \brief Interface for graphic compression algorithm
 */
class GfxCompressor
{
public:
    enum class CompressionType : uint32_t {
        None, Alpha, RGB, RGBA
    };

    using data_type = std::uint8_t;
    using DecompressedData = std::vector<data_type>;
    using size_type = DecompressedData::size_type;

    struct CompressedData {
        CompressionType mType = CompressionType::None;
        std::vector<data_type> mData{};
    };

    CompressedData Compress(CompressionType aType, const data_type* apData, size_type aSize);
    DecompressedData Decompress(const CompressedData &arCompressedData);
    DecompressedData Decompress(CompressionType aType, const data_type* apData, size_type aSize);

protected:
    CompressedData nullCompress(const data_type* apData, size_type aSize);
    DecompressedData nullDecompress(const data_type* apData, size_type aSize);

    CompressedData alphaCompress(const data_type* apData, size_type aSize);
    DecompressedData alphaDecompress(const data_type* apData, size_type aSize);

    CompressedData rgbCompress(const data_type* apData, size_type aSize);
    DecompressedData rgbDecompress(const data_type* apData, size_type aSize);

    CompressedData rgbaCompress(const data_type* apData, size_type aSize);
    DecompressedData rgbaDecompress(const data_type* apData, size_type aSize);
};

}; // namespace rsp::graphics

#endif /* INCLUDE_GRAPHICS_GFXCOMPRESSOR_H_ */
