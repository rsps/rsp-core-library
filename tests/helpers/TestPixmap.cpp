/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#include <TestPixmap.h>
#include "pixmap/GfxResources.h"
#include <graphics/Keyboard.h>

TestPixmap::TestPixmap()
{
    SetInstance(this);
}

TestPixmap::~TestPixmap()
{
    SetInstance(nullptr);
}

void TestPixmap::Boot()
{
    MakePixelData(cLowerCase   , uint32_t(rsp::graphics::Keyboard::TextureId::LowerCase));
    MakePixelData(cErase       , uint32_t(rsp::graphics::Keyboard::TextureId::Erase));
    MakePixelData(cBigSpecial  , uint32_t(rsp::graphics::Keyboard::TextureId::BigSpecial));
    MakePixelData(cSpace       , uint32_t(rsp::graphics::Keyboard::TextureId::Space));
    MakePixelData(cKey         , uint32_t(rsp::graphics::Keyboard::TextureId::Key));
    MakePixelData(cSmallSpecial, uint32_t(rsp::graphics::Keyboard::TextureId::SmallSpecial));
    MakePixelData(cUpperCase   , uint32_t(rsp::graphics::Keyboard::TextureId::UpperCase));
}

