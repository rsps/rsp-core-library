#ifndef BMPLOADER_H
#define BMPLOADER_H

#include <utils/ImgLoader.h>

#include <cstring>
#include <iostream>
#include <vector>

struct BMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t dataOffset;
    uint32_t size;
    uint32_t width;
    uint32_t heigth;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    uint32_t xPixelsPerM;
    uint32_t yPixelsPerM;
    uint32_t coloursUsed;
    uint32_t importantColours;
} __attribute__((packed));  //To stop alignment

class BmpLoader : public ImgLoader {
   public:
    BMPHeader bmpHeader;
    uint16_t bytesPerPixel;

    std::vector<uint32_t> LoadImg(const std::string &aImgName);
};

#endif  //BMPLOADER_H