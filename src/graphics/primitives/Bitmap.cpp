/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "Bitmap.h"

#include <errno.h>

#include <algorithm>

Bitmap::Bitmap() {}
Bitmap::Bitmap(std::string aImgName)
    : imgName(aImgName) {
    std::cout << "Bitmap reading file name: " << aImgName << std::endl;
    errno = 0;
    //Pass reference to the first element in string, and read as binary
    FILE* file = fopen(imgName.c_str(), "rb");
    if (file == NULL) {
        std::cout << "File is null" << std::endl;
        std::cout << "Error: " << errno << std::endl;
    } else {
        std::cout << "File opened" << std::endl;
    }
    //Read the 54 byte header
    fread(&bmpHeader, sizeof(uint8_t), sizeof(bmpHeader), file);
    std::cout << "File header read" << std::endl;

    width = bmpHeader.width;
    height = bmpHeader.heigth;

    std::cout << "Width:            " << width << std::endl;
    std::cout << "Height:           " << height << std::endl;

    //short bitsPerPixel;
    //memcpy(&bitsPerPixel, info + 28, sizeof(short));
    bytesPerPixel = bmpHeader.bitsPerPixel / 8;  //Might be 1 or 4
    //std::cout << "BytesPerPixel Pre Calc: " << bytesPerPixel << std::endl;
    //std::cout << "Mod calc:               " << (bytesPerPixel % 8) << std::endl;
    if ((bmpHeader.bitsPerPixel % 8) > 0) {
        bytesPerPixel = bytesPerPixel + 1;
    }
    //std::cout << "Signature:        " << std::to_string(bmpHeader.signature) << std::endl;
    //std::cout << "FileSize:         " << bmpHeader.fileSize << std::endl;
    //std::cout << "Reserved:         " << bmpHeader.reserved << std::endl;
    std::cout << "DataOffset:       " << bmpHeader.dataOffset << std::endl;
    //std::cout << "Size:             " << bmpHeader.size << std::endl;
    //std::cout << "Width:            " << bmpHeader.width << std::endl;
    //std::cout << "Height:           " << bmpHeader.height << std::endl;
    //std::cout << "Planes:           " << bmpHeader.planes << std::endl;
    std::cout << "BitsPerPix:       " << bmpHeader.bitsPerPixel << std::endl;
    std::cout << "Compression:      " << bmpHeader.compression << std::endl;
    std::cout << "ImageSize:        " << bmpHeader.imageSize << std::endl;
    //std::cout << "xPixelsPerM:      " << bmpHeader.xPixelsPerM << std::endl;
    //std::cout << "yPixelsPerM:      " << bmpHeader.yPixelsPerM << std::endl;
    std::cout << "ColoursUSed:      " << bmpHeader.coloursUsed << std::endl;
    std::cout << "ImportantColours: " << bmpHeader.importantColours << std::endl;
    std::cout << "BytesPerPix:      " << bytesPerPixel << std::endl;

    /* TODO: Get Compression and other useful stuff */

    //Height can be negative, showing the image is stored from top to bottom
    /*int heightSign = 1;
    if (height < 0) {
        heightSign = -1;
    }*/
    /* TODO: Use HeightSign to flip the image correctly */

    //Figure out amount to read
    int size = bytesPerPixel * abs(bmpHeader.heigth) * bmpHeader.width;
    std::cout << "Size: " << size << std::endl;
    std::vector<uint8_t> pixelBuffer;
    pixelBuffer.resize(size);
    //pixels.resize(size);
    //May not place elements correctly
    //May not read correct size for an element
    fseek(file, bmpHeader.dataOffset, SEEK_SET);
    fread(pixelBuffer.data(), sizeof(uint8_t), size, file);
    std::cout << "Number of Pixels: " << pixels.size() << std::endl;
    std::cout << "Bitmap read into memory" << std::endl;
    //int counter = 0;
    for (auto iter = pixelBuffer.begin(); iter != pixelBuffer.end(); std::advance(iter, 3)) {
        uint8_t blue = *iter;
        uint8_t green = *std::next(iter, 1);
        uint8_t red = *std::next(iter, 2);
        uint8_t alpha = 0x00;
        //std::cout << "Current:  " << std::hex << (int)current << std::endl;
        //std::cout << "Next:     " << std::hex << (int)next << std::endl;
        //std::cout << "NextNext: " << std::hex << (int)nextnext << std::endl;

        uint32_t combined = (((uint32_t)red) << 24) |
                            (((uint32_t)green) << 16) |
                            (((uint32_t)blue) << 8) |
                            ((uint32_t)alpha);
        /*if (counter++ < 265) {
            std::cout << "Combined hex val: " << std::hex << combined << std::endl;
        }*/
        pixels.push_back(combined);
        //std::cout << "Combined: " << std::hex << combined << std::endl;
    }
    std::reverse(pixels.begin(), pixels.end());
    fclose(file);
}
Bitmap::Bitmap(const uint32_t* apPixels, int aHeight, int aWidth, int aBytesPerPixel) {
    throw NotImplementedException("");
}
Bitmap::Bitmap(int aHeight, int aWidth, int aBytesPerPixel)
    : height(aHeight),
      width(aWidth),
      bytesPerPixel(aBytesPerPixel),
      pixels(width * height) {
    throw NotImplementedException("");
    //Load file into memory here
    //https://freeimage.sourceforge.io/
    //http://libjpeg.sourceforge.net/
}
Bitmap::~Bitmap() {
}
