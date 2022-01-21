/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
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
    std::vector<uint32_t> LoadImg(const std::string &aImgName);

  protected:
    struct BMPHeader {
        uint16_t signature;
        uint32_t fileSize;
        uint32_t reserved;
        uint32_t dataOffset;
        uint32_t size;
        uint32_t width;
        uint32_t heigth;
        uint16_t planes;
        uint16_t bitsPerPixel;
        uint32_t compression;
        uint32_t imageSize;
        uint32_t xPixelsPerM;
        uint32_t yPixelsPerM;
        uint32_t coloursUsed;
        uint32_t importantColours;
    } __attribute__((packed)) bmpHeader; //To stop alignment

    uint16_t mBytesPerPixel;

    void ReadHeader(std::ifstream &arFile);
    void ReadData(std::ifstream &arFile);
    uint32_t ReadPixel(const std::vector<uint8_t> &aPixelRow, const size_t &aRowPtr);
};

} // namespace rsp::graphics
#endif //BMPLOADER_H
