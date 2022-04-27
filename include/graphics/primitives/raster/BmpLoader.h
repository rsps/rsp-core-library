/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef BMPLOADER_H
#define BMPLOADER_H

#include <cstring>
#include <fstream>
#include <graphics/primitives/raster/ImgLoader.h>
#include <iostream>
#include <vector>

namespace rsp::graphics
{

class BmpLoader : public ImgLoader
{
  public:
    /**
     * \brief Loads an image into memory as a bitmap
     * \param aImgName The relative path to the image
     */
    std::vector<uint32_t> LoadImg(const std::string &aImgName);

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
    } __attribute__((packed)); // To stop alignment

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
    } __attribute__((packed)); // To stop alignment

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
    } __attribute__((packed)); // To stop alignment

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

    friend std::ostream &operator<<(std::ostream &os, const BmpLoader::BmpHeader_t &arHeader);

    uint16_t mBytesPerPixel = 0;

    void ReadHeader(std::ifstream &arFile);
    void ReadData(std::ifstream &arFile);
    uint32_t ReadPixel(const std::vector<uint8_t> &aPixelRow, const size_t &aRowPtr);
};

std::ostream &operator<<(std::ostream &os, const BmpLoader::BmpHeader_t &arHeader);

} // namespace rsp::graphics
#endif // BMPLOADER_H
