/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "BmpLoader.h"
#include <graphics/raster/ImgLoader.h>
#include "PngLoader.h"
#include <memory>

namespace rsp::graphics {

std::shared_ptr<ImgLoader> ImgLoader::GetRasterLoader(const std::string aFileType)
{
    if (aFileType == ".bmp") {
        return std::make_shared<BmpLoader>();
    }
#ifdef USE_PNG
    else if (aFileType == ".png") {
        return std::make_shared<PngLoader>();
    }
#endif
    else {
        THROW_WITH_BACKTRACE1(EUnsupportedFileformat, std::string("Raster loader not found for file type: ") + aFileType);
    }
}

void ImgLoader::initAfterLoad(GuiUnit_t aWidth, GuiUnit_t aHeight, ColorDepth aDepth)
{
    mPixelData.initAfterLoad(aWidth, aHeight, aDepth);
}

}


