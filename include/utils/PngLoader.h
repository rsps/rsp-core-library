#ifndef PNGLOADER_H
#define PNGLOADER_H

#include <utils/ImgLoader.h>

#include <cstring>
#include <iostream>
#include <vector>

class PngLoader : public ImgLoader {
   public:
    struct IDAT {
    } __attribute__((packed));  //To stop alignment;
    struct IEND {
    } __attribute__((packed));  //To stop alignment;
    struct IHDRChunk {
        uint32_t width;
        uint32_t height;
        uint8_t bitDepth;
        uint8_t colourType;
        uint8_t compressionMethod;
        uint8_t filterMethod;
        uint8_t interlaceMethod;
    } __attribute__((packed));  //To stop alignment
    struct PNGChunk {
        uint32_t length;
        char type[4];
        union {
            std::uint8_t data[231];
            struct IHDRChunk ihdr;
            struct IDAT idat;
            struct IEND iend;
        };
        uint32_t crc;
    } __attribute__((packed));  //To stop alignment

    uint8_t pngSignature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    std::vector<uint32_t> LoadImg(const std::string &aImgName);

    uint8_t byteVal;
    uint32_t nothing;

    bool CheckSignature(const uint8_t *aSig, const uint8_t &aSize);
};

#endif  //PNGLOADER_H