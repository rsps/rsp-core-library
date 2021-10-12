/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/primitives/raster/BmpLoader.h>

#include <algorithm>
//#include <cerrno>
#include <fstream>
#include <iostream>

namespace rsp::graphics
{

std::vector<uint32_t> BmpLoader::LoadImg(const std::string &aImgName)
{
    mImagePixels.clear();

    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(aImgName, std::ifstream::binary);

    ReadHeader(file);
    // TODO: Get Compression and other useful stuff

    //Height can be negative, showing the image is stored from top to bottom
    bool normallyDrawn = true;
    if (bmpHeader.heigth < 0) {
        normallyDrawn = false;
    }
    ReadData(file);

    file.close();

    if (normallyDrawn) {
        std::reverse(mImagePixels.begin(), mImagePixels.end());
    }
    return mImagePixels;
}

void BmpLoader::ReadHeader(std::ifstream &aFile)
{
    //Read the 54 byte header
    aFile.read(reinterpret_cast<char *>(&bmpHeader), sizeof(bmpHeader));

    mWidth = bmpHeader.width;
    mHeight = bmpHeader.heigth;

    mBytesPerPixel = bmpHeader.bitsPerPixel / 8; //Might be 1 or 4
    if ((bmpHeader.bitsPerPixel % 8) > 0) {
        mBytesPerPixel = mBytesPerPixel + 1;
    }
}

void BmpLoader::ReadData(std::ifstream &aFile)
{
    //Figure out amount to read per row
    int paddedRowSize = (bmpHeader.width * 3 + 3) & (~3);
    std::cout << "Padded Row size: " << paddedRowSize << std::endl;

    //Initialize containers for reading
    std::vector<uint8_t> pixelRow;
    pixelRow.resize(paddedRowSize);

    //Skip past the offset
    aFile.seekg(bmpHeader.dataOffset);

    for (size_t i = 0; i < abs(bmpHeader.heigth); i++) {
        //Read a Row of pixels with the padding
        aFile.read(reinterpret_cast<char *>(pixelRow.data()), paddedRowSize);

        for (size_t j = bmpHeader.width * mBytesPerPixel; j > 0; j -= mBytesPerPixel) {
            uint32_t combined = ReadPixel(pixelRow, j - mBytesPerPixel);
            mImagePixels.push_back(combined);
        }
    }
}

uint32_t BmpLoader::ReadPixel(const std::vector<uint8_t> &aPixelRow, const size_t &aRowPtr)
{
    uint32_t pixel = 0;
    //Reads other direction than the row loop
    for (size_t i = 0; i < mBytesPerPixel; i++) {
        pixel |= (((uint32_t)aPixelRow[aRowPtr + i]) << (8 * i));
    }
    return pixel;
}
} // namespace rsp::graphics
