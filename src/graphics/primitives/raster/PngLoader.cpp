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
    return (std::uint32_t(p[0]) << 24) | (std::uint32_t(p[1]) << 16) | (std::uint32_t(p[2]) << 8) | std::uint32_t(p[3]);
}

std::ostream& operator<<(std::ostream& os, const PngLoader::IHDR &arIhdr)
{
    os << "Width:       " << arIhdr.width << "\n"
       << "Height:      " << arIhdr.height << "\n"
       << "Bit Depth:   " << arIhdr.bitDepth << "\n"
       << "Colour Type: " << arIhdr.colourType << "\n"
       << "Compression: " << arIhdr.compressionMethod << "\n"
       << "Filter:      " << arIhdr.filterMethod << "\n"
       << "Interlace:   " << arIhdr.interlaceMethod;

    return os;
}

std::ostream& operator<<(std::ostream& os, const FourCC &arChunk)
{
    os << arChunk.mAscii[0] << arChunk.mAscii[1] << arChunk.mAscii[2] << arChunk.mAscii[3];
    return os;
}


void PngLoader::LoadImg(const std::string &arImgName)
{
    Logger::GetDefault().Debug() << "PngLoader reading file: " << arImgName << std::endl;

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
                mIhdr.width = be32toh(mIhdr.width);
                mIhdr.height = be32toh(mIhdr.height);
                initAfterLoad(mIhdr.width, mIhdr.height, getColorDepth());
                break;

            case fourcc("IDAT"):
                decodeDataChunk(chunk.GetAs<const std::uint8_t*>(), chunk.GetSize());
                break;

            case fourcc("PLTE"):
                THROW_WITH_BACKTRACE1(NotImplementedException, "Palette chunks are not supported yet");
                break;

            case fourcc("IEND"):
                return;

            default:
                Logger::GetDefault().Warning() << "Chunk type '" << chunk.GetType() << "' was ignored" << std::endl;
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

    arFile.ExactRead(&mType, sizeof(mType));
    mData.resize(length);
    arFile.ExactRead(mData.data(), length);

    std::uint32_t crc_val;
    arFile.ExactRead(&crc_val, sizeof(crc_val));

    auto crc = Crc32::Calc(&mType, sizeof(mType));
    crc = Crc32::Calc(mData.data(), mData.size(), crc);

    if (crc != crc_val) {
        THROW_WITH_BACKTRACE1(ECorruptedFile, "Corrupted PNG chunk in " + arFile.GetFileName());
    }

    return true;
}

PixelData::ColorDepth PngLoader::getColorDepth()
{
    switch(mIhdr.colourType) {
        case 0:
            if (mIhdr.bitDepth == 1) {
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

