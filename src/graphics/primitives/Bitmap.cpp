/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/primitives/Bitmap.h>

#include <algorithm>
#include <cerrno>

Bitmap::Bitmap() {}
Bitmap::Bitmap(std::string aImgName)
    : imgName(aImgName) {
    //Detect filetype
    std::string filetype = GetFileExtension(aImgName);
    std::cout << "File type read as: " << filetype << std::endl;

    //Get raw data
    imagePixels = filetypeMap[filetype]->LoadImg(aImgName);
    height = filetypeMap[filetype]->height;
    width = filetypeMap[filetype]->width;

    //Convert to int via enum and map
    //https://www.codeguru.com/cplusplus/switch-on-strings-in-c/
    //Better way?

    /*switch (filetypeMap[filetype]) {
        case FileTypes::bmp:
            std::cout << "It is a bmp file" << std::endl;
            //loader = new BmpLoader();
            imagePixels = loader->LoadImg(aImgName);
            break;
        case FileTypes::png:
            //png file
            throw NotImplementedException("Png is not supported");
            break;
        default:
            throw NotImplementedException("The file type is not supported");
            break;
    }*/
    /*
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

    //std::cout << "Width:            " << width << std::endl;
    //std::cout << "Height:           " << height << std::endl;

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
    //std::cout << "DataOffset:       " << bmpHeader.dataOffset << std::endl;
    //std::cout << "Size:             " << bmpHeader.size << std::endl;
    //std::cout << "Width:            " << bmpHeader.width << std::endl;
    //std::cout << "Height:           " << bmpHeader.height << std::endl;
    //std::cout << "Planes:           " << bmpHeader.planes << std::endl;
    //std::cout << "BitsPerPix:       " << bmpHeader.bitsPerPixel << std::endl;
    //std::cout << "Compression:      " << bmpHeader.compression << std::endl;
    //std::cout << "ImageSize:        " << bmpHeader.imageSize << std::endl;
    //std::cout << "xPixelsPerM:      " << bmpHeader.xPixelsPerM << std::endl;
    //std::cout << "yPixelsPerM:      " << bmpHeader.yPixelsPerM << std::endl;
    //std::cout << "ColoursUSed:      " << bmpHeader.coloursUsed << std::endl;
    //std::cout << "ImportantColours: " << bmpHeader.importantColours << std::endl;
    //std::cout << "BytesPerPix:      " << bytesPerPixel << std::endl;

    // TODO: Get Compression and other useful stuff

    //Height can be negative, showing the image is stored from top to bottom
    bool normallyDrawn = true;
    if (height < 0) {
        normallyDrawn = false;
    }

    //Figure out amount to read
    int paddedRowSize = (bmpHeader.width * 3 + 3) & (~3);  //bytesPerPixel * abs(bmpHeader.heigth) * bmpHeader.width;
    //std::cout << "Padded row size: " << paddedRowSize << std::endl;

    //Initialize containers for reading
    std::vector<uint8_t> pixelRow;
    pixelRow.resize(paddedRowSize);

    //Skip past the offset
    fseek(file, bmpHeader.dataOffset, SEEK_SET);

    for (size_t i = 0; i < abs(bmpHeader.heigth); i++) {
        //Read a Row of pixels with the padding
        fread(pixelRow.data(), sizeof(uint8_t), paddedRowSize, file);
        //std::cout << "Bitmap row " << i << " read into memory" << std::endl;
        for (size_t j = 0; j < bmpHeader.width * 3; j += 3) {
            uint8_t blue = pixelRow[j];       //*iter;
            uint8_t green = pixelRow[j + 1];  //*std::next(iter, 1);
            uint8_t red = pixelRow[j + 2];    //*std::next(iter, 2);
            uint8_t alpha = 0x00;

            uint32_t combined = (((uint32_t)red) << 24) |
                                (((uint32_t)green) << 16) |
                                (((uint32_t)blue) << 8) |
                                ((uint32_t)alpha);
            imagePixels.push_back(combined);
            //std::cout << "Combined: " << std::hex << combined << std::endl;
        }
    }
    if (normallyDrawn) {
        std::reverse(imagePixels.begin(), imagePixels.end());
    }

    fclose(file);
    */
}
Bitmap::Bitmap(const uint32_t* apPixels, int aHeight, int aWidth, int aBytesPerPixel) {
    throw NotImplementedException("");
}
Bitmap::Bitmap(int aHeight, int aWidth, int aBytesPerPixel)
    : height(aHeight),
      width(aWidth),
      bytesPerPixel(aBytesPerPixel),
      imagePixels(width * height) {
    throw NotImplementedException("");
    //Load file into memory here
    //https://freeimage.sourceforge.io/
    //http://libjpeg.sourceforge.net/
}
Bitmap::~Bitmap() {
}

std::string Bitmap::GetFileExtension(const std::string& FileName) {
    if (FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".") + 1);
    return "";
}
