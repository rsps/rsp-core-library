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

#ifdef USE_PNG

#include <string>
#include <vector>
#include <exceptions/CoreException.h>
#include <graphics/raster/ImgLoader.h>
#include <posix/FileIO.h>
#include <utils/FourCC.h>

namespace rsp::graphics
{

class ZlibException: public exceptions::CoreException
{
public:
    explicit ZlibException(const char *apMsg, int aErrorCode)
    : CoreException(formatError(apMsg, aErrorCode))
    {
    }

    std::string formatError(const char *apMsg, int aErrorCode);
};


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

        utils::FourCC GetType() const { return mType; }

        size_t GetSize() const { return mData.size(); }

        template <typename T>
        const T& GetAs() const
        {
            return reinterpret_cast<const T&>(*mData.data());
        }

        const std::uint8_t* GetData() const { return mData.data(); }

        const IHDR& GetHeader() const { return GetAs<IHDR>(); }

    protected:
        utils::FourCC mType{};
        std::vector<std::uint8_t> mData{};
    };

    void LoadImg(const std::string &aImgName) override;

protected:
    enum class FilterTypes { None, Sub, Up, Average, Paeth };
    struct Filter
    {
        std::uint8_t *ScanLines[2]{};
        int Current = 0;
        int X = 0;
        int Y = 0;
    };
    Filter mFilter{};

    IHDR mIhdr{};
    pHYs mPhys{};

    void checkSignature(rsp::posix::FileIO &arFile) const;
    ColorDepth getColorDepth() const;
    void decodeDataChunk(const std::uint8_t *apData, size_t aSize);
    void decompressData(const std::uint8_t *apData, size_t aSize);
    void defilterScanLine(const std::vector<std::uint8_t> &arData);
    std::uint8_t unFilter(std::uint8_t aType, std::uint8_t aValue);
    size_t getScanlineWidth();
};

std::ostream& operator<<(std::ostream& os, const PngLoader::IHDR &arIhdr);

} // namespace rsp::graphics

#endif // USE_PNG

#endif // PNGLOADER_H
