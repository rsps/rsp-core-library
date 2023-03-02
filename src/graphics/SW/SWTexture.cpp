/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#include "SWTexture.h"

namespace rsp::graphics {

void SWTexture::Fill(Color aColor)
{
    mPixelData.Fill(aColor);
}

SWTexture::SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, PixelData::ColorDepth aDepth)
    : mPixelData(aWidth, aHeight, aDepth)
{
}

void SWTexture::Update(const PixelData &arPixelData)
{
    if (mPixelData.GetColorDepth() == arPixelData.GetColorDepth()) {
        mPixelData = arPixelData;
    }
    else {
        mPixelData = arPixelData.ChangeColorDepth(mPixelData.GetColorDepth());
    }
}

} /* namespace rsp::graphics */
