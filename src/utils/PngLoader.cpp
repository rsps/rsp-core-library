/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <utils/PngLoader.h>
#include <utils/RSPCoreExceptions.h>

#include <cerrno>

std::vector<uint32_t> PngLoader::LoadImg(const std::string& aImgName) {
    std::cout << "Bitmap reading file: " << aImgName << std::endl;
    errno = 0;

    //Pass reference to the first element in string, and read as binary
    FILE* file = fopen(aImgName.c_str(), "rb");
    if (file == NULL) {
        std::cout << "File is null" << std::endl;
        std::cout << "Error: " << errno << std::endl;
    }

    //Read header and verify that it is a png file
    uint8_t signature[8];
    fread(signature, sizeof(uint8_t), sizeof(signature), file);
    if (!CheckSignature(signature, 8)) {
        throw RSPCoreException("Signature not matching");  //return;  //Some error, probably just throw exception
    }

    // TODO: Declare structs for data handling

    //Read first data chunk (IHDR)
    PNGChunk pngchunk;
    fread(&pngchunk, sizeof(uint8_t), sizeof(pngchunk.length) + sizeof(pngchunk.type), file);
    pngchunk.length = be32toh(pngchunk.length);
    //pngchunk.type = be32toh(pngchunk.type);

    fread(&pngchunk.data[0], sizeof(uint8_t), pngchunk.length + sizeof(pngchunk.crc), file);

    pngchunk.crc = be32toh(pngchunk.crc);
    std::cout << "Length: " << pngchunk.length << std::endl;
    std::cout << "Type: " << pngchunk.type << std::endl;
    std::cout << "Crc: " << std::hex << *(uint32_t*)(&pngchunk.data[pngchunk.length]) << std::endl;

    //fseek(file, 8 + 3, SEEK_SET);
    //fread(&nothing, sizeof(uint8_t), sizeof(nothing), file);
    //nothing = be32toh(nothing);
    //std::cout << "Nothing: " << nothing << std::endl;
    //std::cout << "Bytes:" << std::endl;
    //for (size_t i = 0; i < 4; i++) {
    //    fread(&byteVal, sizeof(uint8_t), sizeof(uint8_t), file);
    //    std::cout << byteVal << std::endl;
    //}

    //fread(&type, sizeof(uint8_t), sizeof(type), file);
    //std::cout << "Type: " << std::to_string(type) << std::endl;

    //fread(&pngChunk, sizeof(uint8_t), sizeof(pngChunk), file);
    //std::cout << "Length: " << pngChunk.length << std::endl;
    //std::cout << "Type: " << std::to_string(pngChunk.type) << std::endl;
    //std::cout << "Crc: " << pngChunk.crc << std::endl;

    //fread(&ihdrChunk, sizeof(uint8_t), sizeof(ihdrChunk), file);
    //std::cout << "Width:       " << ihdrChunk.width << std::endl;
    //std::cout << "Height:      " << ihdrChunk.height << std::endl;
    //std::cout << "Bit Depth:   " << ihdrChunk.bitDepth << std::endl;
    //std::cout << "Colour Type: " << ihdrChunk.colourType << std::endl;
    //std::cout << "Compression: " << ihdrChunk.compressionMethod << std::endl;
    //std::cout << "Filter:      " << ihdrChunk.filterMethod << std::endl;
    //std::cout << "Interlace:   " << ihdrChunk.interlaceMethod << std::endl;

    fclose(file);

    return imagePixels;
}

bool PngLoader::CheckSignature(const uint8_t* aSig, const uint8_t& aSize) {
    /*std::cout << "Given Sig: ";
    for (size_t i = 0; i < aSize; i++) {
        std::cout << std::to_string(aSig[i]) << ", ";
    }
    std::cout << std::endl;
    std::cout << "Png Header: ";
    for (size_t i = 0; i < aSize; i++) {
        std::cout << std::to_string(pngHeader[i]) << ", ";
    }*/

    //Do loop they decay to pointers
    //Or use std::array
    for (size_t i = 0; i < 8; i++) {
        if (aSig[i] != pngSignature[i]) {
            return false;
        }
    }
    return true;
}
