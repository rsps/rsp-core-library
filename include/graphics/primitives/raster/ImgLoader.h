/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
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
