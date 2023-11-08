/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <exceptions/CoreException.h>
#include "BmpLoader.h"
#include <iostream>
#include <string>
#include <utils/FourCC.h>

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

    if (arHeader.v1.size >= sizeof(BmpLoader::BitmapV4Header)) {
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
//    std::cout << "BmpHeader_t size: " << sizeof(BmpHeader_t) << std::endl;
//    std::cout << "BitmapInfoHeader size: " << sizeof(BitmapInfoHeader) << std::endl;
//    std::cout << "BitmapV4Header size: " << sizeof(BitmapV4Header) << std::endl;
//    std::cout << "BitmapV5Header size: " << sizeof(BitmapV5Header) << std::endl;

    mPixelData.GetData().clear();

    mLogger.Debug() << "Loading Bitmap: " << aImgName;

    rsp::posix::FileIO file(aImgName, std::ios::in);

    ReadHeader(file);

//    std::cout << "Header for " << aImgName << "\n" << mBmpHeader << std::endl;
    if (mBmpHeader.v1.bitsPerPixel <= 8) {
        ReadPalette(file);
    }

    ReadData(file);
}

ColorDepth BmpLoader::bitsPerPixelToColorDepth(std::uint32_t aBpp)
{
    switch (aBpp) {
        case 1:
            return ColorDepth::Monochrome;
        case 2:
        case 4:
        case 8:
            return ColorDepth::Alpha;

        case 32:
            return ColorDepth::RGBA;

        default:
            return ColorDepth::RGB;

    }
}

void BmpLoader::ReadHeader(rsp::posix::FileIO &arFile)
{
    // Read the 54 byte header
    arFile.ExactRead(reinterpret_cast<char *>(&mBmpHeader), sizeof(mBmpHeader));

//    mLogger.Debug() << mBmpHeader;

    mBytesPerPixel = mBmpHeader.v1.bitsPerPixel / 8; // Might be 1 or 4
    if ((mBmpHeader.v1.bitsPerPixel % 8) > 0) {
        mBytesPerPixel = mBytesPerPixel + 1;
    }

    initAfterLoad(static_cast<GuiUnit_t>(std::abs(mBmpHeader.v1.width)), static_cast<GuiUnit_t>(std::abs(mBmpHeader.v1.heigth)), bitsPerPixelToColorDepth(mBmpHeader.v1.bitsPerPixel));
    mPixelData.SetBlend(false);

    if (mBmpHeader.v1.size >= sizeof(BmpLoader::BitmapV4Header)) {
        if (mBmpHeader.v5.RedMask == rsp::utils::fourcc("BGRs")) {
            mRedIndex   = 2;
            mGreenIndex = 1;
            mBlueIndex  = 0;
        }
        else {
            mRedIndex   = maskToIndex(mBmpHeader.v5.RedMask);
            mGreenIndex = maskToIndex(mBmpHeader.v5.GreenMask);
            mBlueIndex  = maskToIndex(mBmpHeader.v5.BlueMask);
            mAlphaIndex = maskToIndex(mBmpHeader.v5.AlphaMask);
        }
    }
}

void BmpLoader::ReadPalette(rsp::posix::FileIO &arFile)
{
    mPalette.resize((mBmpHeader.v4.coloursUsed > 0) ? mBmpHeader.v4.coloursUsed : 2^mBmpHeader.v1.bitsPerPixel);
    arFile.Seek(mBmpHeader.v1.size + 14);

    std::uint8_t data[4];
    for (Color &color : mPalette) {
        arFile.ExactRead(data, 4);
        color.SetBlue(data[mBlueIndex]);
        color.SetGreen(data[mGreenIndex]);
        color.SetRed(data[mRedIndex]);
        color.SetAlpha(data[mAlphaIndex]);
    }
}

void BmpLoader::ReadData(rsp::posix::FileIO &arFile)
{
    // Figure out amount to read per row
    size_t paddedRowSize = static_cast<size_t>(((static_cast<size_t>(std::abs(mBmpHeader.v1.width) * mBmpHeader.v1.bitsPerPixel) + 7) / 8) + 3u) & static_cast<size_t>(~3);
    mLogger.Debug() << "Padded Row size: " << paddedRowSize;

    // Initialize container for reading
    std::vector<std::uint8_t> pixelRows(paddedRowSize * static_cast<size_t>(std::abs(mBmpHeader.v1.heigth)));
    mLogger.Debug() << "Container size: " << pixelRows.size();

    // Skip past the offset
    arFile.Seek(mBmpHeader.dataOffset);
    arFile.ExactRead(pixelRows.data(), pixelRows.size());

    // Height can be negative, showing the image is stored from top to bottom
    auto h = static_cast<size_t>(abs(mBmpHeader.v1.heigth));
    auto w = static_cast<size_t>(mBmpHeader.v1.width);

    std::uint8_t *data = pixelRows.data();
    if (mBmpHeader.v1.heigth < 0) { // If height is negative, then image is stored top to bottom.
        for (std::uint32_t y = 0; y < h ; y++) {
            for (std::uint32_t x = 0; x < w; x++) {
                Color color(ReadPixel(data, x, y, paddedRowSize));
                mPixelData.SetPixelAt(GuiUnit_t(x), GuiUnit_t(y), color);
            }
        }
    }
    else { // Image is stored from bottom to top
        for (std::uint32_t y = 0; y < h ; y++) {
            for (std::uint32_t x = 0; x < w; x++) {
                Color color(ReadPixel(data, x, h-1-y, paddedRowSize));
                mPixelData.SetPixelAt(GuiUnit_t(x), GuiUnit_t(y), color);
            }
//            std::cout << std::endl;
        }
    }

    std::cout << "Loaded " << arFile.GetFileName() << " into PixelData (" << w << "x" << h << ")," << std::endl;
//    mPixelData.GetPixelAt(0, 0, Color::White);
}

int BmpLoader::maskToIndex(uint32_t aMask)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    int result = 3;
    while(aMask && (aMask != 0xFF000000)) {
        aMask <<= 8;
        result--;
    }
#else
    int result = 3;
    while(aMask) {
        aMask >>= 8;
        result++;
    }
#endif
    return result;
}

Color BmpLoader::ReadPixel(const uint8_t* apPixelData, std::uint32_t aX, std::uint32_t aY, size_t aPaddedRowSize)
{
    Color pixel = 0;

    switch (mBmpHeader.v1.bitsPerPixel) {
        case 1:
            apPixelData += (aY * aPaddedRowSize) + (aX / 8);
//            if ((aX % 8) == 0) {
//                std::cout << std::hex << std::setw(2) << std::setfill('0') << int(*apPixelData) << ' ';
//            }
            if (*apPixelData & (0x80 >> (aX % 8))) {
                pixel = mPalette[1];
                pixel.SetAlpha(255);
            }
            else {
                pixel = mPalette[0];
                pixel.SetAlpha(0);
            }
            break;

        case 24:
            apPixelData += (aY * aPaddedRowSize) + (aX * 3);
            pixel.SetRed(apPixelData[mRedIndex]);
            pixel.SetGreen(apPixelData[mGreenIndex]);
            pixel.SetBlue(apPixelData[mBlueIndex]);
            pixel.SetAlpha(255);
            break;

        case 32: {
            apPixelData += (aY * aPaddedRowSize) + (aX * 4);
//            std::cout << "CSType: " << mBmpHeader.v5.CSType[0] << mBmpHeader.v5.CSType[1] << mBmpHeader.v5.CSType[2] << mBmpHeader.v5.CSType[3] << std::endl;
            pixel.SetRed(apPixelData[mRedIndex]);
            pixel.SetGreen(apPixelData[mGreenIndex]);
            pixel.SetBlue(apPixelData[mBlueIndex]);
            pixel.SetAlpha(apPixelData[mAlphaIndex]);
            break;
        }

        default:
            THROW_WITH_BACKTRACE1(rsp::exceptions::NotImplementedException, "BmpLoader does not support images with a color depth of " + std::to_string(int(mBmpHeader.v1.bitsPerPixel)) + " bpp");
    }

    return pixel;
}
} // namespace rsp::graphics
