#ifndef BMPLOADER_H
#define BMPLOADER_H

#include <graphics/primitives/raster/ImgLoader.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

namespace rsp::graphics
{

class BmpLoader: public ImgLoader
{
public:
    std::vector<uint32_t> LoadImg(const std::string &aImgName);

protected:
    struct BMPHeader
    {
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
    } __attribute__((packed)) bmpHeader; //To stop alignment
    uint16_t bytesPerPixel;

    void ReadHeader(std::ifstream &arFile);
    void ReadData(std::ifstream &arFile);
};

}
#endif //BMPLOADER_H
