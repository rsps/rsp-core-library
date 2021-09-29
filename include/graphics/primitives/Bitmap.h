#ifndef BITMAP_H
#define BITMAP_H

#include <utils/BmpLoader.h>
#include <utils/PngLoader.h>
#include <utils/RSPCoreExceptions.h>

#include <cstring>
#include <iostream>
#include <map>
#include <vector>

class Bitmap
{
  public:
    std::map<std::string, ImgLoader *> filetypeMap = {
        {"bmp", new BmpLoader()},
        {"png", new PngLoader()}};
    //BMPHeader bmpHeader;             //Removed
    uint32_t mHeight;                   //Unused --> used by canvas, needs to be set
    uint32_t mWidth;                    //Unused --> used by canvas, needs to be set
    uint16_t mBytesPerPixel;            //Unused
    std::string mImgName;               //Unused
    std::vector<uint32_t> mImagePixels; //Pointer?
    //ImgLoader* loader;

    Bitmap();
    Bitmap(std::string aImgName);
    Bitmap(const uint32_t *apPixels, int aHeight, int aWidth, int aBytesPerPixel);
    Bitmap(int aHeight, int aWidth, int aBytesPerPixel);
    ~Bitmap();

    std::string GetFileExtension(const std::string &FileName);

    //Ask and return colour type on pixel
};

#endif //BITMAP_H
