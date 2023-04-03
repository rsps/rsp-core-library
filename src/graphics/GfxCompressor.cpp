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


const GfxCompressor::Result GfxCompressor::Compress(const data_type *apData, size_type aSize)
{
    std::cout << "GfxCompress(" << magic_enum::enum_name(mType) << ")" << std::endl;
//    exceptions::BackTrace bt(0);
//    std::cout << bt << std::endl;

    switch (mType) {
        default:
        case GfxCompression::None:
            return Result{apData, aSize};

        case GfxCompression::Alpha:
            return alphaCompress(apData, aSize);

        case GfxCompression::RGB:
            return rgbCompress(apData, aSize);
    }
}

const GfxCompressor::Result GfxCompressor::Decompress(const data_type *apData, size_type aSize)
{
    std::cout << "GfxDecompress(" << magic_enum::enum_name(mType) << ")" << std::endl;
//    exceptions::BackTrace bt(0);
//    std::cout << bt << std::endl;

    switch (mType) {
        default:
        case GfxCompression::None:
            return Result{apData, aSize};

        case GfxCompression::Alpha:
            return alphaDecompress(apData, aSize);

        case GfxCompression::RGB:
            return rgbDecompress(apData, aSize);
    }
}

const GfxCompressor::Result GfxCompressor::alphaCompress(const data_type* apData, size_t aSize)
{
    mData.reserve(aSize);

    data_type val = apData[0];
    data_type count = 0;

    for (size_t i = 0; i < aSize ; ++i) {
        if ((val != apData[i]) || (count == 255)) {
            mData.push_back(count);
            mData.push_back(val);
            val = apData[i];
            count = 0;
        }
        count++;
    }
    if (aSize) {
        mData.push_back(count);
        mData.push_back(val);
    }

    return Result{mData.data(), mData.size()};
}

const GfxCompressor::Result GfxCompressor::alphaDecompress(const data_type* apData, size_t aSize)
{
    mData.reserve(aSize*2);

    for (size_t i = 0; i < aSize ; i += 2) {
        data_type count = apData[i];
        data_type val = apData[i+1];
        mData.insert(mData.end(), size_type(count), val);
    }

    return Result{mData.data(), mData.size()};
}

const GfxCompressor::Result GfxCompressor::rgbCompress(const data_type *apData, size_t aSize)
{
    mData.reserve(aSize);

    data_type val[3] = { apData[0], apData[1], apData[2] };
    data_type count = 0;

    for (size_t i = 0; i < aSize ; i += 3) {
        if (memcmp(val, &apData[i], 3) || (count == 255)) {
            mData.push_back(count);
            mData.push_back(val[0]);
            mData.push_back(val[1]);
            mData.push_back(val[2]);
            val[0] = apData[i+0];
            val[1] = apData[i+1];
            val[2] = apData[i+2];
            count = 0;
        }
        count++;
    }
    if (count) {
        mData.push_back(count);
        mData.push_back(val[0]);
        mData.push_back(val[1]);
        mData.push_back(val[2]);
    }

    return Result{mData.data(), mData.size()};
}

const GfxCompressor::Result GfxCompressor::rgbDecompress(const data_type *apData, size_t aSize)
{
    mData.reserve(aSize*2);

    for (size_t i = 0; i < aSize ; i += 4) {
        data_type count = apData[i];
        data_type val[3] = { apData[i+1], apData[i+2], apData[i+3] };

        for (data_type n = 0 ; n < count ; n++) {
            mData.insert(mData.end(), &val[0], &val[3]);
        }
    }

    return Result{mData.data(), mData.size()};
}

} /* namespace rsp::graphics */
