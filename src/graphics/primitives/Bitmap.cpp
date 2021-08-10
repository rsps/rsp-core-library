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

Bitmap::Bitmap(std::string aImgName)
    : imgName(aImgName) {
    FILE* file = fopen(&imgName[0], "rb");   //Pass reference to the first element in string, and read as binary
    unsigned char info[54];                  //Bmp file header (14 bytes) and InforHeader(40 bytes)
    fread(info, sizeof(uint8_t), 54, file);  //Read the 54 byte header

    //Get information from header info
    memcpy(&width, info + 18, sizeof(int));
    memcpy(&height, info + 22, sizeof(int));
    short bitsPerPixel;
    memcpy(&bitsPerPixel, info + 28, sizeof(short));
    bytesPerPixel = bitsPerPixel / 8;  //Might be 1 or 4
    if (bytesPerPixel % 8 > 0) {
        bytesPerPixel = bytesPerPixel + 1;
    }

    std::cout << "Width: " << width << std::endl;
    std::cout << "Height: " << height << std::endl;
    std::cout << "BitsPerPix: " << bitsPerPixel << std::endl;
    std::cout << "BytesPerPix: " << bytesPerPixel << std::endl;

    /* TODO: Get Compression and other useful stuff */

    //Height can be negative, showing the image is stored from top to bottom
    int heightSign = 1;
    if (height < 0) {
        heightSign = -1;
    }

    //Figure out amount to read
    int pixelAmount = height * width;
    pixels.resize(pixelAmount);
    //May not place elements correctly
    //May not read correct size for an element
    fread(&pixels, bytesPerPixel, pixelAmount, file);
    std::cout << "Bitmap read into memoery" << std::endl;
}
Bitmap::Bitmap(const uint32_t* apPixels, int aHeight, int aWidth, int aBytesPerPixel) {
    throw NotImplementedException("");
}
Bitmap::Bitmap(int aHeight, int aWidth, int aBytesPerPixel)
    : height(aHeight),
      width(aWidth),
      bytesPerPixel(aBytesPerPixel),
      pixels(width * height) {
    //Load file into memory here
    //https://freeimage.sourceforge.io/
    //http://libjpeg.sourceforge.net/
}
Bitmap::~Bitmap() {
}
