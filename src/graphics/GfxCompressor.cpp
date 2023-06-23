/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cstring>
#include <graphics/GfxCompressor.h>
#include <magic_enum.hpp>
#include <iostream>
#include <exceptions/BackTrace.h>

namespace rsp::graphics {


GfxCompressor::CompressedData GfxCompressor::Compress(CompressionType aType, const data_type *apData, size_type aSize)
{
    std::cout << "GfxCompress(" << magic_enum::enum_name(aType) << ")" << std::endl;
//    exceptions::BackTrace bt(0);
//    std::cout << bt << std::endl;

    switch (aType) {
        default:
        case CompressionType::None:
            return nullCompress(apData, aSize);

        case CompressionType::Alpha:
            return alphaCompress(apData, aSize);

        case CompressionType::RGB:
            return rgbCompress(apData, aSize);

        case CompressionType::RGBA:
            return rgbaCompress(apData, aSize);
    }
}

GfxCompressor::DecompressedData GfxCompressor::Decompress(const CompressedData &arCompressedData)
{
    return Decompress(arCompressedData.mType, arCompressedData.mData.data(), arCompressedData.mData.size());
}

GfxCompressor::DecompressedData GfxCompressor::Decompress(CompressionType aType, const data_type *apData, size_type aSize)
{
    std::cout << "GfxDecompress(" << magic_enum::enum_name(aType) << ")" << std::endl;
//    exceptions::BackTrace bt(0);
//    std::cout << bt << std::endl;

    switch (aType) {
        default:
        case CompressionType::None:
            return nullDecompress(apData, aSize);

        case CompressionType::Alpha:
            return alphaDecompress(apData, aSize);

        case CompressionType::RGB:
            return rgbDecompress(apData, aSize);

        case CompressionType::RGBA:
            return rgbaDecompress(apData, aSize);
    }
}

GfxCompressor::CompressedData GfxCompressor::nullCompress(const data_type *apData, size_type aSize)
{
    CompressedData result;

    result.mType = CompressionType::None;
    result.mData.resize(aSize);

    for (auto &val : result.mData) {
        val = *apData++;
    }

    return result;
}

GfxCompressor::DecompressedData GfxCompressor::nullDecompress(const data_type *apData, size_type aSize)
{
    DecompressedData result;
    result.resize(aSize);
    for (auto &val : result) {
        val = *apData++;
    }
    return result;
}

GfxCompressor::CompressedData GfxCompressor::alphaCompress(const data_type* apData, size_t aSize)
{
    CompressedData result;
    result.mType = CompressionType::Alpha;
    result.mData.reserve(aSize);

    data_type val = apData[0];
    data_type count = 0;

    for (size_t i = 0; i < aSize ; ++i) {
        if ((val != apData[i]) || (count == 255)) {
            result.mData.push_back(count);
            result.mData.push_back(val);
            val = apData[i];
            count = 0;
        }
        count++;
    }
    if (aSize) {
        result.mData.push_back(count);
        result.mData.push_back(val);
    }

    return result;
}

GfxCompressor::DecompressedData GfxCompressor::alphaDecompress(const data_type* apData, size_type aSize)
{
    DecompressedData result;
    result.reserve(aSize*2);

    for (size_t i = 0; i < aSize ; i += 2) {
        data_type count = apData[i];
        data_type val = apData[i+1];
        result.insert(result.end(), size_type(count), val);
    }

    return result;
}

GfxCompressor::CompressedData GfxCompressor::rgbCompress(const data_type *apData, size_type aSize)
{
    CompressedData result;
    result.mType = CompressionType::RGB;
    result.mData.reserve(aSize);

    data_type val[3] = { apData[0], apData[1], apData[2] };
    data_type count = 0;

    for (size_t i = 0; i < aSize ; i += 3) {
        if (memcmp(val, &apData[i], 3) || (count == 255)) {
            result.mData.push_back(count);
            result.mData.insert(result.mData.end(), &val[0], &val[2]);
            val[0] = apData[i+0];
            val[1] = apData[i+1];
            val[2] = apData[i+2];
            count = 0;
        }
        count++;
    }
    if (count) {
        result.mData.push_back(count);
        result.mData.insert(result.mData.end(), &val[0], &val[2]);
    }

    return result;
}

GfxCompressor::DecompressedData GfxCompressor::rgbDecompress(const data_type *apData, size_type aSize)
{
    DecompressedData result;
    result.reserve(aSize*2);

    for (size_t i = 0; i < aSize ; i += 4) {
        data_type count = apData[i];
        data_type val[3] = { apData[i+1], apData[i+2], apData[i+3] };

        for (data_type n = 0 ; n < count ; n++) {
            result.insert(result.end(), &val[0], &val[3]);
        }
    }

    return result;
}

GfxCompressor::CompressedData GfxCompressor::rgbaCompress(const data_type *apData, size_type aSize)
{
    CompressedData result;
    result.mType = CompressionType::RGBA;
    result.mData.reserve(aSize);

    data_type val[4] = { apData[0], apData[1], apData[2], apData[3] };
    data_type count = 0;

    for (size_t i = 0; i < aSize ; i += 4) {
        if (memcmp(val, &apData[i], 4) || (count == 255)) {
            result.mData.push_back(count);
            result.mData.insert(result.mData.end(), &val[0], &val[4]);
            val[0] = apData[i+0];
            val[1] = apData[i+1];
            val[2] = apData[i+2];
            val[3] = apData[i+3];
            count = 0;
        }
        count++;
    }
    if (count) {
        result.mData.push_back(count);
        result.mData.insert(result.mData.end(), &val[0], &val[4]);
    }

    return result;
}

GfxCompressor::DecompressedData GfxCompressor::rgbaDecompress(const data_type *apData, size_type aSize)
{
    DecompressedData result;
    result.reserve(aSize*2);

    for (size_t i = 0; i < aSize ; i += 5) {
        data_type count = apData[i];
        data_type val[4] = { apData[i+1], apData[i+2], apData[i+3], apData[i+4] };

        for (data_type n = 0 ; n < count ; n++) {
            result.insert(result.end(), &val[0], &val[4]);
        }
    }

    return result;
}

} /* namespace rsp::graphics */
