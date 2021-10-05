#ifndef BITMAP_H
#define BITMAP_H

#include <graphics/primitives/raster/ImgLoader.h>

#include <string>
#include <vector>

namespace rsp::graphics
{

class Bitmap
{
public:
    Bitmap(std::string aImgName);
    Bitmap(const uint32_t *apPixels, int aHeight, int aWidth, int aBytesPerPixel);
    Bitmap(int aHeight, int aWidth, int aBytesPerPixel);
    ~Bitmap();

    uint32_t GetHeight() const {
        return mHeight;
    }

    uint32_t GetWidth() const {
        return mWidth;
    }

    const std::vector<uint32_t>& GetPixels() const {
        return mImagePixels;
    }

    std::string GetFileExtension(const std::string &FileName);

protected:
    ImgLoader& GetRasterLoader(const std::string aFileExtension);

    uint32_t mHeight;                   //Unused --> used by canvas, needs to be set
    uint32_t mWidth;                    //Unused --> used by canvas, needs to be set
    uint16_t mBytesPerPixel;            //Unused
    std::vector<uint32_t> mImagePixels; //Pointer?
};

}
#endif //BITMAP_H
