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
#include <iostream>
#include <fstream>

namespace rsp::graphics
{

std::vector<uint32_t> BmpLoader::LoadImg(const std::string &aImgName)
{
    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(aImgName, std::ifstream::binary);

    //std::cout << "Bitmap reading file: " << aImgName << std::endl;
//    errno = 0;
//    //Pass reference to the first element in string, and read as binary
//    FILE *file = fopen(aImgName.c_str(), "rb");
//    if (file == NULL) {
//        std::cout << "File is null" << std::endl;
//        std::cout << "Error: " << errno << std::endl;
//    }

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
    aFile.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));

    //Read the 54 byte header
//    fread(&bmpHeader, sizeof(uint8_t), sizeof(bmpHeader), aFile);
    //std::cout << "File header read" << std::endl;

    mWidth = bmpHeader.width;
    mHeight = bmpHeader.heigth;

    bytesPerPixel = bmpHeader.bitsPerPixel / 8; //Might be 1 or 4
    if ((bmpHeader.bitsPerPixel % 8) > 0) {
        bytesPerPixel = bytesPerPixel + 1;
    }
}

void BmpLoader::ReadData(std::ifstream &aFile)
{
    //Figure out amount to read
    int paddedRowSize = (bmpHeader.width * 3 + 3) & (~3);

    //Initialize containers for reading
    std::vector<uint8_t> pixelRow;
    pixelRow.resize(paddedRowSize);

    //Skip past the offset
    aFile.seekg(bmpHeader.dataOffset);
//    fseek(aFile, bmpHeader.dataOffset, SEEK_SET);

    for (size_t i = 0; i < abs(bmpHeader.heigth); i++) {
        //Read a Row of pixels with the padding
        aFile.read(reinterpret_cast<char*>(pixelRow.data()), paddedRowSize);
//        fread(pixelRow.data(), sizeof(uint8_t), paddedRowSize, aFile);
        for (size_t j = 0; j < bmpHeader.width * bytesPerPixel; j += bytesPerPixel) {
            uint8_t blue = pixelRow[j];      //*iter;
            uint8_t green = pixelRow[j + 1]; //*std::next(iter, 1);
            uint8_t red = pixelRow[j + 2];   //*std::next(iter, 2);
            uint8_t alpha = 0x00;

            uint32_t combined = (static_cast<uint32_t>(red) << 24) | (static_cast<uint32_t>(green) << 16)
                | (static_cast<uint32_t>(blue) << 8) | static_cast<uint32_t>(alpha);
            mImagePixels.push_back(combined);
        }
    }
}

}

