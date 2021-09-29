/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/primitives/Bitmap.h>

#include <algorithm>
#include <cerrno>

Bitmap::Bitmap() {}
Bitmap::Bitmap(std::string aImgName)
    : mImgName(aImgName)
{
    //Detect filetype
    std::string filetype = GetFileExtension(aImgName);
    //std::cout << "File type read as: " << filetype << std::endl;

    //Get raw data
    mImagePixels = filetypeMap[filetype]->LoadImg(aImgName);
    mHeight = filetypeMap[filetype]->mHeight;
    mWidth = filetypeMap[filetype]->mWidth;
}

Bitmap::Bitmap(const uint32_t *apPixels, int aHeight, int aWidth, int aBytesPerPixel)
{
    throw NotImplementedException("");
}

Bitmap::Bitmap(int aHeight, int aWidth, int aBytesPerPixel)
    : mHeight(aHeight),
      mWidth(aWidth),
      mBytesPerPixel(aBytesPerPixel),
      mImagePixels(mWidth * mHeight)
{
    throw NotImplementedException("");
    //Load file into memory here
    //https://freeimage.sourceforge.io/
    //http://libjpeg.sourceforge.net/
}

Bitmap::~Bitmap()
{
}

std::string Bitmap::GetFileExtension(const std::string &FileName)
{
    if (FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".") + 1);
    return "";
}
