#ifndef IMGLOADER_H
#define IMGLOADER_H

#include <cstring>
#include <iostream>
#include <vector>

class ImgLoader
{
  public:
    std::vector<uint32_t> mImagePixels;
    uint32_t mHeight;
    uint32_t mWidth;
    virtual ~ImgLoader() = default;
    virtual std::vector<uint32_t> LoadImg(const std::string &aImgName) = 0;
};

#endif //IMGLOADER_H