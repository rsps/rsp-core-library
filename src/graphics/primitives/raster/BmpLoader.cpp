/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <algorithm>
#include <iostream>
#include <logging/Logger.h>
#include "BmpLoader.h"

using namespace rsp::logging;

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

    if (arHeader.v1.size == sizeof(BmpLoader::BitmapV4Header)) {
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
        << "GammaBlue: " << arHeader.v5.GammaBlue << "\n";
    }

    if (arHeader.v1.size == sizeof(BmpLoader::BitmapV5Header)) {
        os
        << "Intent: " << arHeader.v5.Intent << "\n"
        << "ProfileData: " << arHeader.v5.ProfileData << "\n"
        << "ProfileSize: " << arHeader.v5.ProfileSize << "\n"
        << "Reserved: " << arHeader.v5.Reserved << "\n";
    }

    return os;
}


void BmpLoader::LoadImg(const std::string &aImgName)
{
    std::cout << "BmpHeader_t size: " << sizeof(BmpHeader_t) << std::endl;
    std::cout << "BitmapInfoHeader size: " << sizeof(BitmapInfoHeader) << std::endl;
    std::cout << "BitmapV4Header size: " << sizeof(BitmapV4Header) << std::endl;
    std::cout << "BitmapV5Header size: " << sizeof(BitmapV5Header) << std::endl;

    mPixelData.GetData().clear();

    rsp::posix::FileIO file(aImgName, std::ios::in);

    ReadHeader(file);
    // TODO: Get Compression and other useful stuff

    ReadData(file);
}

PixelData::ColorDepth BmpLoader::bitsPerPixelToColorDepth(unsigned int aBpp)
{
    switch (aBpp) {
        case 1:
            return PixelData::ColorDepth::Monochrome;
        case 2:
        case 4:
        case 8:
            return PixelData::ColorDepth::Alpha;

        default:
            return PixelData::ColorDepth::RGB;

    }
}

void BmpLoader::ReadHeader(rsp::posix::FileIO &arFile)
{
    // Read the 54 byte header
    arFile.ExactRead(reinterpret_cast<char *>(&mBmpHeader), sizeof(mBmpHeader));

    Logger::GetDefault().Debug() << mBmpHeader << std::endl;;

    mBytesPerPixel = mBmpHeader.v1.bitsPerPixel / 8; // Might be 1 or 4
    if ((mBmpHeader.v1.bitsPerPixel % 8) > 0) {
        mBytesPerPixel = mBytesPerPixel + 1;
    }

    initAfterLoad(static_cast<uint32_t>(std::abs(mBmpHeader.v1.width)), static_cast<uint32_t>(std::abs(mBmpHeader.v1.heigth)), bitsPerPixelToColorDepth(mBmpHeader.v1.bitsPerPixel));
}

void BmpLoader::ReadData(rsp::posix::FileIO &arFile)
{
    // Figure out amount to read per row
    std::size_t paddedRowSize = static_cast<std::size_t>((static_cast<std::size_t>(std::abs(mBmpHeader.v1.width)) * mBytesPerPixel + 3u) & static_cast<std::size_t>(~3));
    Logger::GetDefault().Debug() << "Padded Row size: " << paddedRowSize << std::endl;;

    // Initialize container for reading
    std::vector<std::uint8_t> pixelRows(paddedRowSize * static_cast<std::size_t>(std::abs(mBmpHeader.v1.heigth)));
    Logger::GetDefault().Debug() << "Container size: " << pixelRows.size() << std::endl;;
    //    pixelRows.resize();

    // Skip past the offset
    arFile.Seek(mBmpHeader.dataOffset);
    arFile.ExactRead(pixelRows.data(), pixelRows.size());

    // Height can be negative, showing the image is stored from top to bottom
    std::size_t h = static_cast<std::size_t>(abs(mBmpHeader.v1.heigth));

    std::uint8_t *data = pixelRows.data();
    unsigned int i = 0;
    unsigned int inc = 1;
    if (mBmpHeader.v1.heigth < 0) {
        i = static_cast<unsigned int>(std::abs(mBmpHeader.v1.heigth));
        inc = static_cast<unsigned int>(-1);
    }

    for (unsigned int y = 0; y < h ; y++) {
        unsigned int x = 0;
        for (unsigned int j = static_cast<unsigned int>(std::abs(mBmpHeader.v1.width)); j > 0; j--) {
            Color color(ReadPixel(&data[(i * paddedRowSize) + (j * mBytesPerPixel)]));
            mPixelData.SetPixelAt(x++, y, color);
        }
        i += inc;
    }
    std::cout << "Loaded " << arFile.GetFileName() << " into PixelData." << std::endl;
    mPixelData.GetPixelAt(0, 0, Color::White);
}

Color BmpLoader::ReadPixel(const uint8_t* apPixelRow)
{
    Color pixel = 0;
    // Reads other direction than the row loop
    if (mBytesPerPixel == 4) {
        pixel.SetRed(apPixelRow[3]);
        pixel.SetGreen(apPixelRow[2]);
        pixel.SetBlue(apPixelRow[1]);
        pixel.SetAlpha(apPixelRow[0]);
    }
    else {
        for (size_t i = 0; i < mBytesPerPixel; i++) {
            pixel = static_cast<uint32_t>(pixel) | ((static_cast<uint32_t>(apPixelRow[i])) << (8 * i));
        }
    }
    return pixel;
}
} // namespace rsp::graphics
