/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/primitives/raster/ImgLoader.h>
#include <memory>
#include "BmpLoader.h"
#include "PngLoader.h"

namespace rsp::graphics {

std::shared_ptr<ImgLoader> ImgLoader::GetRasterLoader(const std::string aFileType)
{
    if (aFileType == ".bmp") {
        return std::make_shared<BmpLoader>();
    }
    else if (aFileType == ".png") {
        return std::make_shared<PngLoader>();
    }
    else {
        THROW_WITH_BACKTRACE1(EUnsupportedFileformat, std::string("Raster loader not found for file type: ") + aFileType);
    }
}

void ImgLoader::initAfterLoad(unsigned int aWidth, unsigned int aHeight, PixelData::ColorDepth aDepth)
{
    mPixelData.initAfterLoad(aWidth, aHeight, aDepth);
}

}


