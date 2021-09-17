#ifndef IMGLOADER_H
#define IMGLOADER_H

#include <cstring>
#include <iostream>
#include <vector>

class ImgLoader {
   public:
    std::vector<uint32_t> imagePixels;

    virtual std::vector<uint32_t> LoadImg(std::string aImgName);
};

#endif  //IMGLOADER_H