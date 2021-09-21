#ifndef PNGLOADER_H
#define PNGLOADER_H

#include <utils/ImgLoader.h>

#include <cstring>
#include <iostream>
#include <vector>

class PngLoader : public ImgLoader {
   public:
    std::vector<uint32_t> LoadImg(const std::string &aImgName);
};

#endif  //PNGLOADER_H