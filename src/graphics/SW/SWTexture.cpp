/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#include <graphics/SW/SWTexture.h>

namespace rsp::graphics {

//void SWTexture::Fill(Color aColor)
//{
////    mPixelData.Fill(aColor);
//}

SWTexture::SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, PixelData::ColorDepth aDepth)
//    : mPixelData(aWidth, aHeight, aDepth)
{
//    mPixelData.SetBlend(false);
}

void SWTexture::Update(const PixelData &arPixelData, Color aColor)
{
//    mPixelData.CopyFrom(Point(0,0), arPixelData, arPixelData.GetRect(), aColor);
    mpPixelData = &arPixelData;
}

} /* namespace rsp::graphics */
