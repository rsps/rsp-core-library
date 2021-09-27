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
#include <iomanip>

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
    PNGChunk pngchunk;

    //-- Chunk reading loop begins here --
    /*while (std::strcmp(pngchunk.type, "IEND") != 0) {
        
    }*/

    //Temp loop for testing
    for (size_t i = 0; i < 3; i++) {
        //Read length and type
        fread(&pngchunk, sizeof(uint8_t), sizeof(pngchunk.length) + sizeof(pngchunk.type), file);
        //Flip length
        pngchunk.length = be32toh(pngchunk.length);

        std::cout << "Length: " << pngchunk.length << std::endl;
        std::cout << "Type: " << pngchunk.type << std::endl;

        //If ciritcal chunk
        if (std::isupper(pngchunk.type[0])) {
            std::cout << "Chunk is Critical" << std::endl;
            //Chunk is critical
            //If IHDR chunk
            if (std::strcmp(pngchunk.type, "IHDR") == 0) {
                std::cout << "IHDR chunk recognized" << std::endl;
                //Use length to read the chunks data field
                fread(&pngchunk.ihdr, sizeof(uint8_t), sizeof(pngchunk.ihdr) + sizeof(pngchunk.crc), file);  // read whole chunk

                //Flip time
                //Only flip things that are larger than a byte
                pngchunk.ihdr.width = be32toh(pngchunk.ihdr.width);
                pngchunk.ihdr.height = be32toh(pngchunk.ihdr.height);
                *(uint32_t*)(&pngchunk.data[pngchunk.length]) = be32toh(*(uint32_t*)(&pngchunk.data[pngchunk.length]));

                //Set important bitmap variables
                width = pngchunk.ihdr.width;
                height = pngchunk.ihdr.height;

                std::cout << "Width       :" << pngchunk.ihdr.width << std::endl;
                std::cout << "Height      :" << pngchunk.ihdr.height << std::endl;
                std::cout << "Bit Depth   :" << +pngchunk.ihdr.bitDepth << std::endl;
                std::cout << "Colour Type :" << +pngchunk.ihdr.colourType << std::endl;
                std::cout << "Compression :" << +pngchunk.ihdr.compressionMethod << std::endl;
                std::cout << "Filter      :" << +pngchunk.ihdr.filterMethod << std::endl;
                std::cout << "Interlace   :" << +pngchunk.ihdr.interlaceMethod << std::endl;

                std::cout << "Crc         :" << std::hex << *(uint32_t*)(&pngchunk.data[pngchunk.length]) << std::endl;
                std::cout << std::dec;
            }
            //If IDAT chunk
            else if (std::strcmp(pngchunk.type, "IDAT") == 0) {
                std::cout << "IDAT chunk recognized" << std::endl;
                //Use length to read the chunks data field
                fread(&pngchunk.data[0], sizeof(uint8_t), pngchunk.length + sizeof(pngchunk.crc), file);

                //Flip crc
                *(uint32_t*)(&pngchunk.data[pngchunk.length]) = be32toh(*(uint32_t*)(&pngchunk.data[pngchunk.length]));
                // TODO do something with crc
                std::cout << "Crc: " << std::hex << *(uint32_t*)(&pngchunk.data[pngchunk.length]) << std::endl;
                std::cout << std::dec;
            }
            //If PLTE chunk | Do i really need?
            else if (std::strcmp(pngchunk.type, "PLTE") == 0) {
                std::cout << "PLTE chunk recognized" << std::endl;
                throw NotImplementedException("Don't know how to handle PLTE chunks");
            }
            //If IEND
            else if (std::strcmp(pngchunk.type, "IEND") == 0) {
                std::cout << "IEND chunk recognized" << std::endl;
                //Stop reading
            }
        } else {
            std::cout << "Chunk is Non-Critical" << std::endl;
            //Chunk is not critical
            fseek(file, pngchunk.length + sizeof(pngchunk.crc), SEEK_CUR);
            //Read next chunk
        }
    }

    //Use length to read the chunks data field
    //fread(&pngchunk.data[0], sizeof(uint8_t), pngchunk.length + sizeof(pngchunk.crc), file);
    //
    //pngchunk.crc = be32toh(pngchunk.crc);
    //std::cout << "Length: " << pngchunk.length << std::endl;
    //std::cout << "Type: " << pngchunk.type << std::endl;
    //std::cout << "Crc: " << std::hex << *(uint32_t*)(&pngchunk.data[pngchunk.length]) << std::endl;

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
