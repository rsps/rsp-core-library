/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef PNGLOADER_H
#define PNGLOADER_H

#include <cstring>
#include <iostream>
#include <vector>
#include <graphics/primitives/raster/ImgLoader.h>
#include <posix/FileIO.h>
#include <endian.h>

namespace rsp::graphics
{

class FourCC
{
public:
    FourCC() {}
    FourCC(std::uint32_t aValue)
    {
        mValue = htobe32(aValue);
    }

    FourCC(const char *apValue)
    {
        for (unsigned int i = 0; i < sizeof(mAscii) ; i++) {
            mAscii[i] = apValue[i];
        }
    }

    operator std::uint32_t() { return mValue; }

    bool operator==(const FourCC &arOther)
    {
        return mValue == arOther.mValue;
    }

protected:
    friend std::ostream& operator<<(std::ostream& os, const FourCC &arChunk);

    union {
        std::uint32_t mValue = 0;
        char mAscii[4];
    };
};
std::ostream& operator<<(std::ostream& os, const FourCC &arChunk);

class PngLoader: public ImgLoader
{
public:
    struct IHDR
    {
        uint32_t width;
        uint32_t height;
        uint8_t bitDepth;
        uint8_t colourType;
        uint8_t compressionMethod;
        uint8_t filterMethod;
        uint8_t interlaceMethod;
    } __attribute__((packed));

    class PngChunk
    {
    public:
        bool LoadFrom(rsp::posix::FileIO &arFile);

        FourCC GetType() { return mType; }

        std::size_t GetSize() const { return mData.size(); }

        template <typename T>
        const T& GetAs() const
        {
            return *reinterpret_cast<const T*>(mData.data());
        }

        const IHDR& GetHeader() { return GetAs<IHDR>(); }

    protected:
        FourCC mType{};
        std::vector<std::uint8_t> mData{};
    };

    void LoadImg(const std::string &aImgName) override;

protected:

    IHDR mIhdr{};

    void checkSignature(rsp::posix::FileIO &arFile);
    PixelData::ColorDepth getColorDepth();
    void decodeDataChunk(const std::uint8_t *apData, std::size_t aSize);
};

std::ostream& operator<<(std::ostream& os, const PngLoader::IHDR &arIhdr);

} // namespace rsp::graphics

#endif // PNGLOADER_H
