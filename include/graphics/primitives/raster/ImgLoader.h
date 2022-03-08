/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
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

    /**
     * \brief Abstract methos for loading an image into memory as a bitmap
     * \param aImgName The relative path to the image
     */
    virtual std::vector<uint32_t> LoadImg(const std::string &aImgName) = 0;

    /**
     * \brief Get the bitmap of the loaded image
     * \return A reference to the vector holding the pixel values
     */
    const std::vector<uint32_t> &GetPixels() const
    {
        return mImagePixels;
    }

    /**
     * \brief Get the height of the bitmap
     * \return Value of the height
     */
    int GetHeight() const
    {
        return mHeight;
    }

    /**
     * \brief Get the width of the bitmap
     * \return Value of the width
     */
    int GetWidth() const
    {
        return mWidth;
    }

  protected:
    std::vector<uint32_t> mImagePixels{};
    int mHeight = 0;
    int mWidth = 0;
};

} // namespace rsp::graphics
#endif // IMGLOADER_H
