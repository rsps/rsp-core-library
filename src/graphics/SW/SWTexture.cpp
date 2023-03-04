/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#include <graphics/SW/SWTexture.h>

namespace rsp::graphics {

void SWTexture::Fill(Color aColor)
{
//    mPixelData.Fill(aColor);
}

SWTexture::SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, PixelData::ColorDepth aDepth)
//    : mPixelData(aWidth, aHeight, aDepth)
{
}

void SWTexture::Update(const PixelData &arPixelData, Color aColor)
{
    mpPixelData = &arPixelData;
//    mPixelData.CopyFrom(Point(0,0), arPixelData, arPixelData.GetRect(), aColor);
}

} /* namespace rsp::graphics */
