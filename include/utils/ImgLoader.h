#ifndef IMGLOADER_H
#define IMGLOADER_H

#include <cstring>
#include <iostream>
#include <vector>

class ImgLoader {
   public:
    std::vector<uint32_t> imagePixels;
    uint32_t height;
    uint32_t width;
    virtual ~ImgLoader() = default;
    virtual std::vector<uint32_t> LoadImg(const std::string &aImgName) = 0;
};

#endif  //IMGLOADER_H