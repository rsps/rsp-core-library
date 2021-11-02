#ifndef IMGLOADER_H
#define IMGLOADER_H

#include <cstring>
#include <iostream>
#include <vector>

namespace rsp::graphics
{

class ImgLoader
{
  public:
    virtual ~ImgLoader() = default;
    virtual std::vector<uint32_t> LoadImg(const std::string &aImgName) = 0;

    const std::vector<uint32_t> &GetPixels() const
    {
        return mImagePixels;
    }

    uint32_t GetHeight() const
    {
        return mHeight;
    }
    uint32_t GetWidth() const
    {
        return mWidth;
    }

  protected:
    std::vector<uint32_t> mImagePixels;
    uint32_t mHeight;
    uint32_t mWidth;
};

} // namespace rsp::graphics
#endif //IMGLOADER_H
