#ifndef BITMAP_H
#define BITMAP_H

#include <cstring>
#include <iostream>
#include <vector>

#include "../../../src/utils/RSPCoreExceptions.h"

class Bitmap {
   public:
    int height;
    int width;
    short bytesPerPixel;
    std::string imgName;
    std::vector<uint32_t> pixels;

    Bitmap(std::string aImgName);
    Bitmap(const uint32_t* apPixels, int aHeight, int aWidth, int aBytesPerPixel);
    Bitmap(int aHeight, int aWidth, int aBytesPerPixel);
    ~Bitmap();

    //Ask and return color type on pixel
};

#endif  //BITMAP_H