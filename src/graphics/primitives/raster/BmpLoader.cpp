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
#include <graphics/primitives/Color.h>

namespace rsp::graphics
{

std::ostream& operator <<(std::ostream &os, const BmpLoader::BmpHeader_t &arHeader)
{
    os << "BMP Header: \n"
        << "Signature: " << static_cast<char>(arHeader.signature & 0xFF) << static_cast<char>(arHeader.signature >> 8) << "\n"
        << "File Size: " << arHeader.fileSize << "\n"
        << "reserved: " << arHeader.reserved << "\n"
        << "Data Offset: " << arHeader.dataOffset << "\n"
        <<  "Size: " << arHeader.v1.size << "\n"
        << "Width: " << arHeader.v1.width << "\n"
        << "Heigth: " << arHeader.v1.heigth << "\n"
        << "Planes: " << arHeader.v1.planes << "\n"
        << "Bits Per Pixel: " << arHeader.v1.bitsPerPixel << "\n"
        << "Compression: " << arHeader.v1.compression << "\n"
        << "Image Size: "  << arHeader.v1.imageSize << "\n"
        << "xPixelsPerM: " << arHeader.v1.xPixelsPerM << "\n"
        << "yPixelsPerM: " << arHeader.v1.yPixelsPerM << "\n"
        << "Colors Used: " << arHeader.v1.coloursUsed << "\n"
        << "Important Colors: " << arHeader.v1.importantColours << "\n";

    if (arHeader.v1.size == sizeof(BmpLoader::BitmapV5Header)) {
        os
        << "RedMask: " << std::hex << arHeader.v5.RedMask << "\n"
        << "GreenMask: " << arHeader.v5.GreenMask << "\n"
        << "BlueMask: " << arHeader.v5.BlueMask << "\n"
        << "AlphaMask: " << arHeader.v5.AlphaMask << std::dec << "\n"
        << "CSType: " << arHeader.v5.CSType << "\n"
        << "RedX: " << arHeader.v5.Red.X << "\n"
        << "RedY: " << arHeader.v5.Red.Y << "\n"
        << "RedZ: " << arHeader.v5.Red.Z << "\n"
        << "GreenX: " << arHeader.v5.Green.X << "\n"
        << "GreenY: " << arHeader.v5.Green.Y << "\n"
        << "GreenZ: " << arHeader.v5.Green.Z << "\n"
        << "BlueX: " << arHeader.v5.Blue.X << "\n"
        << "BlueY: " << arHeader.v5.Blue.Y << "\n"
        << "BlueZ: " << arHeader.v5.Blue.Z << "\n"
        << "GammaRed: " << arHeader.v5.GammaRed << "\n"
        << "GammaGreen: " << arHeader.v5.GammaGreen << "\n"
        << "GammaBlue: " << arHeader.v5.GammaBlue << "\n"
        << "Intent: " << arHeader.v5.Intent << "\n"
        << "ProfileData: " << arHeader.v5.ProfileData << "\n"
        << "ProfileSize: " << arHeader.v5.ProfileSize << "\n"
        << "Reserved: " << arHeader.v5.Reserved << "\n";
    }

    return os;
}


std::vector<uint32_t> BmpLoader::LoadImg(const std::string &aImgName)
{
    mImagePixels.clear();

    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(aImgName, std::ifstream::binary);

    ReadHeader(file);
    // TODO: Get Compression and other useful stuff

    // Height can be negative, showing the image is stored from top to bottom
    bool normallyDrawn = true;
    if (mBmpHeader.v1.heigth < 0) {
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
    // Read the 54 byte header
    aFile.read(reinterpret_cast<char *>(&mBmpHeader), sizeof(mBmpHeader));

//    std::cout << mBmpHeader;

    mWidth = mBmpHeader.v1.width;
    mHeight = mBmpHeader.v1.heigth;

    mBytesPerPixel = mBmpHeader.v1.bitsPerPixel / 8; // Might be 1 or 4
    if ((mBmpHeader.v1.bitsPerPixel % 8) > 0) {
        mBytesPerPixel = mBytesPerPixel + 1;
    }
}

void BmpLoader::ReadData(std::ifstream &aFile)
{
    // Figure out amount to read per row
    int paddedRowSize = (mBmpHeader.v1.width * mBytesPerPixel + 3) & (~3);
    // std::cout << "Padded Row size: " << paddedRowSize << std::endl;

    // Initialize containers for reading
    std::vector<uint8_t> pixelRow;
    pixelRow.resize(static_cast<unsigned long int>(paddedRowSize));

    // Skip past the offset
    aFile.seekg(mBmpHeader.dataOffset);

    for (size_t i = 0; i < abs(mBmpHeader.v1.heigth); i++) {
        // Read a Row of pixels with the padding
        aFile.read(reinterpret_cast<char *>(pixelRow.data()), paddedRowSize);

        for (int j = mBmpHeader.v1.width * mBytesPerPixel; j > 0; j -= mBytesPerPixel) {
            uint32_t combined = ReadPixel(pixelRow, static_cast<size_t>(j - mBytesPerPixel));
            mImagePixels.push_back(combined);
        }
    }
}

uint32_t BmpLoader::ReadPixel(const std::vector<uint8_t> &aPixelRow, const size_t &aRowPtr)
{
    Color pixel = 0;
    // Reads other direction than the row loop
    if (mBytesPerPixel == 4) {
        pixel.SetRed(aPixelRow[aRowPtr + 3]);
        pixel.SetGreen(aPixelRow[aRowPtr + 2]);
        pixel.SetBlue(aPixelRow[aRowPtr + 1]);
        pixel.SetAlpha(aPixelRow[aRowPtr + 0]);
    }
    else {
        for (size_t i = 0; i < mBytesPerPixel; i++) {
            pixel = static_cast<uint32_t>(pixel) | ((static_cast<uint32_t>(aPixelRow[aRowPtr + i])) << (8 * i));
        }
    }
    return pixel;
}
} // namespace rsp::graphics
