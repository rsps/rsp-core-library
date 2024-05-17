/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef RSP_CORE_LIB_SRC_GRAPHICS_RASTER_BMP_LOADER_H
#define RSP_CORE_LIB_SRC_GRAPHICS_RASTER_BMP_LOADER_H

#include <graphics/Color.h>
#include <graphics/raster/ImgLoader.h>
#include <logging/LogChannel.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <posix/FileIO.h>

namespace rsp::graphics
{

class BmpLoader : public ImgLoader, public logging::NamedLogger<BmpLoader>
{
public:
    /**
     * \brief Loads an image into memory as a bitmap
     * \param aImgName The relative path to the image
     */
    void LoadImg(const std::string &aImgName) override;

protected:
    struct BitmapInfoHeader {
        uint32_t size;
        int32_t width;
        int32_t heigth;
        uint16_t planes;
        uint16_t bitsPerPixel;
        uint32_t compression;
        uint32_t imageSize;
        uint32_t xPixelsPerM;
        uint32_t yPixelsPerM;
        uint32_t coloursUsed;
        uint32_t importantColours;
    } __attribute__((packed)); // Size: 40

    struct ColorCoordinate_t {
        uint32_t X;
        uint32_t Y;
        uint32_t Z;
    } __attribute__((packed));

    struct BitmapV4Header {
        uint32_t size;
        int32_t width;
        int32_t heigth;
        uint16_t planes;
        uint16_t bitsPerPixel;
        uint32_t compression;
        uint32_t imageSize;
        uint32_t xPixelsPerM;
        uint32_t yPixelsPerM;
        uint32_t coloursUsed;
        uint32_t importantColours;
        uint32_t RedMask;
        uint32_t GreenMask;
        uint32_t BlueMask;
        uint32_t AlphaMask;
        uint32_t CSType;
        ColorCoordinate_t Red;
        ColorCoordinate_t Green;
        ColorCoordinate_t Blue;
        uint32_t GammaRed;
        uint32_t GammaGreen;
        uint32_t GammaBlue;
    } __attribute__((packed)); // Size: 108

    struct BitmapV5Header {
        uint32_t size;
        int32_t width;
        int32_t heigth;
        uint16_t planes;
        uint16_t bitsPerPixel;
        uint32_t compression;
        uint32_t imageSize;
        uint32_t xPixelsPerM;
        uint32_t yPixelsPerM;
        uint32_t coloursUsed;
        uint32_t importantColours;
        uint32_t RedMask;
        uint32_t GreenMask;
        uint32_t BlueMask;
        uint32_t AlphaMask;
        char CSType[4];
        ColorCoordinate_t Red;
        ColorCoordinate_t Green;
        ColorCoordinate_t Blue;
        uint32_t GammaRed;
        uint32_t GammaGreen;
        uint32_t GammaBlue;
        uint32_t Intent;
        uint32_t ProfileData;
        uint32_t ProfileSize;
        uint32_t Reserved;
    } __attribute__((packed)); // Size: 124

    struct BmpHeader_t {
        uint16_t signature;
        uint32_t fileSize;
        uint32_t reserved;
        uint32_t dataOffset;
        union {
            struct BitmapInfoHeader v1;
            struct BitmapV4Header v4;
            struct BitmapV5Header v5;
        };
    } __attribute__((packed)) mBmpHeader{};

    int mRedIndex = 0;
    int mGreenIndex = 0;
    int mBlueIndex = 0;
    int mAlphaIndex = 0;

    friend std::ostream &operator<<(std::ostream &os, const BmpLoader::BmpHeader_t &arHeader);

    size_t mBytesPerPixel = 0;
    std::vector<Color> mPalette{};

    void ReadHeader(rsp::posix::FileIO &arFile);
    void ReadPalette(rsp::posix::FileIO &arFile);
    void ReadData(rsp::posix::FileIO &arFile);
    Color ReadPixel(const uint8_t* apPixelData, uint32_t aX, uint32_t aY, size_t aPaddedRowSize);

    static ColorDepth bitsPerPixelToColorDepth(uint32_t aBpp);
    static int maskToIndex(uint32_t aMask);

};

std::ostream &operator<<(std::ostream &os, const BmpLoader::BmpHeader_t &arHeader);

} // namespace rsp::graphics
#endif // RSP_CORE_LIB_SRC_GRAPHICS_RASTER_BMP_LOADER_H
