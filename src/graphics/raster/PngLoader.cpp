/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */

#ifdef USE_PNG

#include <iostream>
#include <exceptions/CoreException.h>
#include "PngLoader.h"
#include <logging/Logger.h>
#include <utils/Crc32.h>
#include <utils/HexStream.h>

#define ZLIB_CONST
#include <zlib.h>

using namespace rsp::utils;
using namespace rsp::logging;

namespace rsp::graphics
{

std::string ZlibException::formatError(const char *apMsg, int aErrorCode)
{
    std::string result("Error in ZLib: ");
    result += std::string(apMsg) + " (" + std::to_string(aErrorCode) + "): ";

    switch(aErrorCode) {
        case Z_OK:
            result += "OK";
            break;

        case Z_STREAM_END:
            result += "End of stream";
            break;

        case Z_NEED_DICT:
            result += "Need dictionary";
            break;

        case Z_ERRNO:
            result += "Errno: " + std::to_string(errno);
            break;

        case Z_STREAM_ERROR:
            result += "Stream error";
            break;

        case Z_DATA_ERROR:
            result += "Data error";
            break;

        case Z_MEM_ERROR:
            result += "Memory error";
            break;

        case Z_BUF_ERROR:
            result += "Buffer error";
            break;

        case Z_VERSION_ERROR:
            result += "Version error";
            break;

        default:
            result += "Unknown error!!";
            break;
    }
    return result;
}

std::ostream& operator<<(std::ostream &os, const PngLoader::IHDR &arIhdr)
{
    os <<    "Width:       " << arIhdr.Width
        << "\nHeight:      " << arIhdr.Height
        << "\nBit Depth:   " << int(arIhdr.BitDepth)
        << "\nColour Type: " << int(arIhdr.ColorType)
        << "\nCompression: " << int(arIhdr.CompressionMethod)
        << "\nFilter:      " << int(arIhdr.FilterMethod)
        << "\nInterlace:   " << int(arIhdr.InterlaceMethod);
    return os;
}

void PngLoader::LoadImg(const std::string &arImgName)
{
    Logger::GetDefault().Debug() << "PngLoader reading file: " << arImgName;

    //Pass reference to the first element in string, and read as binary
    posix::FileIO file(arImgName, std::ios_base::in);

    //Read signature chunk and verify that it is a png file
    checkSignature(file);

    mFilter = {};

    //-- Chunk reading loop --
    PngChunk chunk;
    while (chunk.LoadFrom(file)) {
        switch (chunk.GetType()) {
            case fourcc("IHDR"):
                mIhdr = chunk.GetHeader();
                mIhdr.Width = be32toh(mIhdr.Width);
                mIhdr.Height = be32toh(mIhdr.Height);
                Logger::GetDefault().Debug() << "PNG Header:\n" << mIhdr;
                initAfterLoad(static_cast<GuiUnit_t>(mIhdr.Width), static_cast<GuiUnit_t>(mIhdr.Height), getColorDepth());
                break;

            case fourcc("IDAT"):
                decodeDataChunk(chunk.GetData(), chunk.GetSize());
                break;

            case fourcc("PLTE"):
                THROW_WITH_BACKTRACE1(rsp::exceptions::NotImplementedException, "Palette chunks are not supported yet");
                break;

            case fourcc("IEND"):
                return;

            case fourcc("pHYs"):
                mPhys = chunk.GetAs<pHYs>();
                break;

            default:
                Logger::GetDefault().Warning() << "Chunk type '" << chunk.GetType() << "' was ignored";
                break;
        }
    }
}

void PngLoader::checkSignature(rsp::posix::FileIO &arFile) const
{
    const uint8_t png_signature[8] = { 0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n' };
    uint8_t buffer[sizeof(png_signature)];

    arFile.ExactRead(buffer, sizeof(png_signature));

    for (size_t i = 0; i < sizeof(png_signature); i++) {
        if (buffer[i] != png_signature[i]) {
            THROW_WITH_BACKTRACE1(ECorruptedFile, "Corrupted PNG signature in " + arFile.GetFileName());
        }
    }
}

bool PngLoader::PngChunk::LoadFrom(rsp::posix::FileIO &arFile)
{
    std::uint32_t length;
    auto sz = arFile.Read(&length, sizeof(length));
    if (sz == 0) {
        return false;
    }
    length = be32toh(length);

    arFile.ExactRead(&mType, sizeof(mType));
    mData.resize(length);
    arFile.ExactRead(mData.data(), length);

    std::uint32_t crc_val;
    arFile.ExactRead(&crc_val, sizeof(crc_val));
    crc_val = be32toh(crc_val);

    auto crc = Crc32::Calc(&mType, sizeof(mType));
    crc = Crc32::Calc(mData.data(), mData.size(), crc);

    if (crc != crc_val) {
        std::cerr << "PNG Chunk CRC: " << std::hex << std::setw(8) << std::setfill('0') << crc << " should be " << crc_val << std::endl;
        THROW_WITH_BACKTRACE1(ECorruptedFile, "Corrupted PNG chunk in " + arFile.GetFileName());
    }

    return true;
}

ColorDepth PngLoader::getColorDepth() const
{
    if (mIhdr.BitDepth == 16) {
        THROW_WITH_BACKTRACE1(rsp::exceptions::NotImplementedException, "Pixel samples of 16-bit depth are not supported yet");
    }
    switch (mIhdr.ColorType) {
        case 0:
            if (mIhdr.BitDepth == 1) {
                return ColorDepth::Monochrome;
            }
            else {
                return ColorDepth::Alpha;
            }

        case 4:
            return ColorDepth::Alpha;

        case 2:
        case 3:
        case 6:
        default:
            return ColorDepth::RGB;
    }
}

void PngLoader::decodeDataChunk(const std::uint8_t *apData, size_t aSize)
{
//    std::cout << HexStream(apData, aSize) << std::endl;
    decompressData(apData, aSize);
}

void PngLoader::decompressData(const std::uint8_t *apData, size_t aSize)
{
    // init the decompression stream
    z_stream stream;

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

//    int ret = inflateInit2_(&stream, -MAX_WBITS, ZLIB_VERSION, int(sizeof(z_stream)));
    int ret = inflateInit(&stream);
    if (ret != Z_OK) {
        THROW_WITH_BACKTRACE2(ZlibException, "inflateInit2_", ret);
    }

    // Set the starting point and total data size to be read
    stream.avail_in = aSize;
    stream.next_in = apData;

    std::vector<std::uint8_t> buffer;
    size_t line_size = 1 + getScanlineWidth();

    buffer.resize(2 * line_size);
    mFilter.ScanLines[0] = buffer.data();
    mFilter.ScanLines[1] = buffer.data() + line_size;

    // Start decompressing
    while (stream.avail_in != 0) {
        stream.next_out = buffer.data();
        stream.avail_out = buffer.size();

        ret = inflate(&stream, Z_NO_FLUSH);

        if (ret == Z_STREAM_END) {
            // only store the data we have left in the stream
            buffer.resize(buffer.size() - stream.avail_out);
            defilterScanLine(buffer);
            break;
        }
        else if (ret == Z_OK) {
            defilterScanLine(buffer);
        }
        else {
            std::cout << "Available: " << stream.avail_in << ", buffer: " << buffer.size() << std::endl;
            THROW_WITH_BACKTRACE2(ZlibException, "inflate", ret);
        }
    }

    ret = inflateEnd(&stream);
    if (ret != Z_OK) {
        THROW_WITH_BACKTRACE2(ZlibException, "inflateEnd", ret);
    }
}

void PngLoader::defilterScanLine(const std::vector<std::uint8_t> &arData)
{
    std::cout << "Defiltering: " << int(arData[0]) << std::endl;
//    std::cout << HexStream(arData) << std::endl;

    auto it = arData.begin();
    std::uint8_t type = *it++;

    for (; it != arData.end(); ++it) {
        auto orig = unFilter(type, *it);
    }
}

std::uint8_t PngLoader::unFilter(std::uint8_t aType, std::uint8_t aValue)
{
    switch(aType) {
        default:
            break;
    }
//    mPixelData
//    mFilter.X
    return 0;
}

size_t PngLoader::getScanlineWidth()
{
    size_t result = mIhdr.Width * mIhdr.BitDepth;
    switch (mIhdr.ColorType) {
        case 2:
        case 6:
            result *= 3;
            break;
        default:
            break;
    }
    return (result / 8);
}

} // rsp::graphics

#endif // USE_PNG
