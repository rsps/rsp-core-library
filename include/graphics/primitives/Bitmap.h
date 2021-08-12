#ifndef BITMAP_H
#define BITMAP_H

#include <cstring>
#include <iostream>
#include <vector>

#include "../../../src/utils/RSPCoreExceptions.h"

//Should be unsigned?
struct BMPHeader {
    short signature;
    int fileSize;
    int reserved;
    int dataOffset;
    int size;
    int width;
    int heigth;
    short planes;
    short bitsPerPixel;
    int compression;
    int imageSize;
    int xPixelsPerM;
    int yPixelsPerM;
    int coloursUsed;
    int importantColours;
};

class Bitmap {
   public:
    BMPHeader bmpHeader;
    int height;
    int width;
    short bytesPerPixel;
    std::string imgName;
    std::vector<uint32_t> pixels;

    Bitmap(std::string aImgName);
    Bitmap(const uint32_t* apPixels, int aHeight, int aWidth, int aBytesPerPixel);
    Bitmap(int aHeight, int aWidth, int aBytesPerPixel);
    ~Bitmap();

    //Ask and return colour type on pixel
};

#endif  //BITMAP_H
