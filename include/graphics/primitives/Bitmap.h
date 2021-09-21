#ifndef BITMAP_H
#define BITMAP_H

#include <utils/BmpLoader.h>
#include <utils/PngLoader.h>
#include <utils/RSPCoreExceptions.h>

#include <cstring>
#include <iostream>
#include <map>
#include <vector>
//Removed
/*
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
*/
class Bitmap {
   public:
    /*enum FileTypes {
        bmp,
        png
    };*/
    std::map<std::string, ImgLoader*> filetypeMap = {
        {"bmp", new BmpLoader()},
        {"png", new PngLoader()}};

    /*std::map<std::string, FileTypes> filetypeMap = {
        {"bmp", FileTypes::bmp},
        {"png", FileTypes::png}};*/
    //BMPHeader bmpHeader;              //Removed
    uint32_t height;                    //Unused --> used by canvas, needs to be set
    uint32_t width;                     //Unused --> used by canvas, needs to be set
    uint16_t bytesPerPixel;             //Unused
    std::string imgName;                //Unused
    std::vector<uint32_t> imagePixels;  //Pointer?
    //ImgLoader* loader;

    Bitmap();
    Bitmap(std::string aImgName);
    Bitmap(const uint32_t* apPixels, int aHeight, int aWidth, int aBytesPerPixel);
    Bitmap(int aHeight, int aWidth, int aBytesPerPixel);
    ~Bitmap();

    std::string GetFileExtension(const std::string& FileName);

    //Ask and return colour type on pixel
};

#endif  //BITMAP_H
