/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/primitives/raster/PngLoader.h>
#include <utils/CoreException.h>
#include <cerrno>
#include <iomanip>

using namespace rsp::utils;

namespace rsp::graphics
{


std::vector<uint32_t> PngLoader::LoadImg(const std::string &aImgName)
{
    throw NotImplementedException("Png file format is not supported");
    std::cout << "Bitmap reading file: " << aImgName << std::endl;
    errno = 0;

    //Pass reference to the first element in string, and read as binary
    FILE *file = fopen(aImgName.c_str(), "rb");
    if (file == NULL) {
        std::cout << "File is null" << std::endl;
        std::cout << "Error: " << errno << std::endl;
    }

    //Read header and verify that it is a png file
    uint8_t signature[8];
    if (fread(signature, sizeof(signature), 1 , file) != 1) {
        THROW_SYSTEM("Could not read signature.");
    }
    if (!CheckSignature(signature, 8)) {
        throw CoreException("Signature not matching"); //return;  //Some error, probably just throw exception
    }

    // TODO: Declare structs for data handling
    PNGChunk pngchunk;

    //-- Chunk reading loop begins here --

    //Temp loop for testing
    for (size_t i = 0; i < 3; i++) {
        //Read length and type
        if (fread(&pngchunk, sizeof(pngchunk.length) + sizeof(pngchunk.type), 1, file) != 1) {
            THROW_SYSTEM("Could not read PNG chunk header.");
        }
        //Flip length
        pngchunk.length = be32toh(pngchunk.length);

        std::cout << "Type: " << pngchunk.type << std::endl;
        std::cout << "Length: " << pngchunk.length << std::endl;

        //If critcal chunk
        if (std::isupper(pngchunk.type[0])) {
            std::cout << "Chunk is Critical" << std::endl;
            //Chunk is critical
            //If IHDR chunk
            if (std::strcmp(pngchunk.type, "IHDR") == 0) {
                std::cout << "IHDR chunk recognized" << std::endl;

                //Allocate buffer size of chunk length plus crc
                pngchunk.data = new uint8_t[pngchunk.length + sizeof(uint32_t)];

                //Use length to read the chunks data field
                //Read chunk and crc into data buffer
                if (fread(pngchunk.data, pngchunk.length + sizeof(uint32_t), 1, file) != 1) { // read whole chunk
                    THROW_SYSTEM("Could not read PNG chunk data.");
                }

                //Flip time
                //Only flip things that are larger than a byte
                pngchunk.ihdr->width = be32toh(pngchunk.ihdr->width);
                pngchunk.ihdr->height = be32toh(pngchunk.ihdr->height);
                /* TODO: Maybe, maybe not flip crc
                 * (uint32_t*)(&pngchunk.data[pngchunk.length]) = be32toh(*(uint32_t*)(&pngchunk.data[pngchunk.length]));
                 */

                //Set important bitmap variables
                mWidth = static_cast<int>(pngchunk.ihdr->width);
                mHeight = static_cast<int>(pngchunk.ihdr->height);

                std::cout << "Width       :" << pngchunk.ihdr->width << std::endl;
                std::cout << "Height      :" << pngchunk.ihdr->height << std::endl;
                std::cout << "Bit Depth   :" << +pngchunk.ihdr->bitDepth << std::endl;
                std::cout << "Colour Type :" << +pngchunk.ihdr->colourType << std::endl;
                std::cout << "Compression :" << +pngchunk.ihdr->compressionMethod << std::endl;
                std::cout << "Filter      :" << +pngchunk.ihdr->filterMethod << std::endl;
                std::cout << "Interlace   :" << +pngchunk.ihdr->interlaceMethod << std::endl;

                std::cout << "Crc         :" << std::hex << *reinterpret_cast<uint32_t*>(&pngchunk.data[pngchunk.length]) << std::endl;
                std::cout << std::dec;

                // TODO Stuff save or use header info

            }
            //If IDAT chunk
            else if (std::strcmp(pngchunk.type, "IDAT") == 0) {
                std::cout << "IDAT chunk recognized" << std::endl;

                //Allocate buffer size of chunk length plus crc
                pngchunk.data = new uint8_t[pngchunk.length + sizeof(uint32_t)];

                //Use length to read the chunks data field
                if (fread(pngchunk.data, pngchunk.length + sizeof(uint32_t), 1, file) != 1) {
                    THROW_SYSTEM("Could not read PNG IDAT chunk.");
                }

                //Decode

                /* TODO: Maybe, maybe not flip crc
                 *(uint32_t*)(&pngchunk.data[pngchunk.length]) = be32toh(*(uint32_t*)(&pngchunk.data[pngchunk.length]));
                 * TODO do something with crc
                 */
                std::cout << "Crc: " << std::hex << *reinterpret_cast<uint32_t*>(&pngchunk.data[pngchunk.length]) << std::endl;
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
            } else {
                std::cout << pngchunk.type << " Chunk type not recognized" << std::endl;
            }
        } else {
            std::cout << "Chunk is Non-Critical" << std::endl;
            //Chunk is not critical
            //Seek the chunks length of data plus its crc
            fseek(file, static_cast<long int>(pngchunk.length + sizeof(uint32_t)), SEEK_CUR);
            //Read next chunk
        }
        //Clean up memory
        if (pngchunk.data != nullptr) {
            delete[] pngchunk.data;
            pngchunk.data = nullptr;
        }
        std::cout << std::endl;
    }

    fclose(file);

    return mImagePixels;
}

bool PngLoader::CheckSignature(const uint8_t *aSig, const uint8_t &aSize)
{
    //Do loop they decay to pointers
    //Or use std::array
    for (size_t i = 0; i < 8; i++) {
        if (aSig[i] != mPngSignature[i]) {
            return false;
        }
    }
    return true;
}
void PngLoader::ReadHeader(FILE *file)
{
}
void PngLoader::ReadData(FILE *file)
{
}

}

