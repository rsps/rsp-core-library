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
#include <posix/FileIO.h>
#include <endian.h>
#include <graphics/raster/ImgLoader.h>

namespace rsp::graphics
{

class FourCC
{
public:
    FourCC() noexcept {}
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
        std::cout << "FourCC compare(" << mValue << ", " << arOther.mValue << ")" << std::endl;
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
        uint32_t Width;
        uint32_t Height;
        uint8_t  BitDepth;
        uint8_t  ColorType;
        uint8_t  CompressionMethod;
        uint8_t  FilterMethod;
        uint8_t  InterlaceMethod;
    } __attribute__((packed));
    struct pHYs
    {
        uint32_t PixelPerUnitX;
        uint32_t PixelPerUnitY;
        uint8_t  UnitSpecifier;
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
            return reinterpret_cast<const T&>(*mData.data());
        }

        const std::uint8_t* GetData() { return mData.data(); }

        const IHDR& GetHeader() { return GetAs<IHDR>(); }

    protected:
        FourCC mType{};
        std::vector<std::uint8_t> mData{};
    };

    void LoadImg(const std::string &aImgName) override;

protected:

    IHDR mIhdr{};
    pHYs mPhys{};

    void checkSignature(rsp::posix::FileIO &arFile);
    PixelData::ColorDepth getColorDepth();
    void decodeDataChunk(const std::uint8_t *apData, std::size_t aSize);
};

std::ostream& operator<<(std::ostream& os, const PngLoader::IHDR &arIhdr);

} // namespace rsp::graphics

#endif // PNGLOADER_H
