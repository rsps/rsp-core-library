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

#include <logging/Logger.h>
#include <utils/CoreException.h>
#include <utils/Crc32.h>
#include "PngLoader.h"

using namespace rsp::utils;
using namespace rsp::logging;

namespace rsp::graphics
{

constexpr std::uint32_t fourcc( char const p[5] )
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (std::uint32_t(p[3]) << 24) | (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[1]) << 8) | std::uint32_t(p[0]);
#else
    return (std::uint32_t(p[0]) << 24) | (std::uint32_t(p[1]) << 16) | (std::uint32_t(p[2]) << 8) | std::uint32_t(p[3]);
#endif
}

std::ostream& operator<<(std::ostream& os, const PngLoader::IHDR &arIhdr)
{
    os << "Width:       " << arIhdr.Width << "\n"
       << "Height:      " << arIhdr.Height << "\n"
       << "Bit Depth:   " << arIhdr.BitDepth << "\n"
       << "Colour Type: " << arIhdr.ColorType << "\n"
       << "Compression: " << arIhdr.CompressionMethod << "\n"
       << "Filter:      " << arIhdr.FilterMethod << "\n"
       << "Interlace:   " << arIhdr.InterlaceMethod;

    return os;
}

std::ostream& operator<<(std::ostream& os, const FourCC &arChunk)
{
    os << arChunk.mAscii[0] << arChunk.mAscii[1] << arChunk.mAscii[2] << arChunk.mAscii[3];
    return os;
}


void PngLoader::LoadImg(const std::string &arImgName)
{
    Logger::GetDefault().Debug() << "PngLoader reading file: " << arImgName;

    //Pass reference to the first element in string, and read as binary
    posix::FileIO file(arImgName, std::ios_base::in);

    //Read signature chunk and verify that it is a png file
    checkSignature(file);

    //-- Chunk reading loop --
    PngChunk chunk;
    while (chunk.LoadFrom(file)) {
        switch(chunk.GetType()) {
            case fourcc("IHDR"):
                mIhdr = chunk.GetHeader();
                mIhdr.Width = be32toh(mIhdr.Width);
                mIhdr.Height = be32toh(mIhdr.Height);
                initAfterLoad(static_cast<GuiUnit_t>(mIhdr.Width), static_cast<GuiUnit_t>(mIhdr.Height), getColorDepth());
                break;

            case fourcc("IDAT"):
                decodeDataChunk(chunk.GetData(), chunk.GetSize());
                break;

            case fourcc("PLTE"):
                THROW_WITH_BACKTRACE1(NotImplementedException, "Palette chunks are not supported yet");
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

void PngLoader::checkSignature(rsp::posix::FileIO &arFile)
{
    const uint8_t png_signature[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
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
        std::cout << "CRC: " << std::hex << std::setw(8) << std::setfill('0') << crc << ", VAL: " << crc_val << std::endl;
        THROW_WITH_BACKTRACE1(ECorruptedFile, "Corrupted PNG chunk in " + arFile.GetFileName());
    }

    return true;
}

PixelData::ColorDepth PngLoader::getColorDepth()
{
    switch(mIhdr.ColorType) {
        case 0:
            if (mIhdr.BitDepth == 1) {
                return PixelData::ColorDepth::Monochrome;
            }
            else {
                return PixelData::ColorDepth::Alpha;
            }

        case 4:
            return PixelData::ColorDepth::Alpha;

        case 2:
        case 3:
        case 6:
        default:
            return PixelData::ColorDepth::RGB;
    }
}

void PngLoader::decodeDataChunk(const std::uint8_t *apData, std::size_t aSize)
{
}

}

