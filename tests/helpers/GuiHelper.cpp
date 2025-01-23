/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <graphics/Renderer.h>
#include <posix/FileSystem.h>
#include <scenes/Scenes.h>
#include <utils/Timer.h>
#include "TestHelpers.h"
#include "GuiHelper.h"

using namespace rsp::graphics;
using namespace rsp::utils;


GuiHelper::GuiHelper(bool aRegisterTestTouchParser)
    : mLogger{},
      mTouchParser(aRegisterTestTouchParser),
      mPixMap{},
      mTimerQueue{}
{
//    CHECK_NOTHROW(Font::RegisterFont("fonts/Exo 2/Exo2-VariableFont_wght.ttf"));
//    CHECK_NOTHROW(Font::SetDefaultFont("Exo 2"));
    CHECK_NOTHROW(Font::RegisterFont("fonts/Noto Sans/NotoSans-VariableFont_wdth,wght.ttf"));
    CHECK_NOTHROW(Font::RegisterFont("fonts/Noto Sans/NotoSansSC-VariableFont_wght.ttf"));
    CHECK_NOTHROW(Font::SetDefaultFont("Noto Sans SC"));

    // Make framebuffer
#ifdef USE_GFX_SW
    std::filesystem::path p;
    CHECK_NOTHROW(p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"}));
    Renderer::SetDevicePath(p.string());
#endif

    CHECK_NOTHROW(Renderer::Init(480, 800));
    auto& renderer = Renderer::Get();

    // Set default scene size to screen size
    CHECK_NOTHROW(Scene::SetScreenSize(renderer.GetWidth(), renderer.GetHeight()));

    mPixMap.Boot();

    // Make scenes
    CHECK_NOTHROW(SecondScene scn2);
    CHECK_NOTHROW(InputScene scn3);
    CHECK_NOTHROW(Scenes dummy_scenes);
}

GuiHelper::~GuiHelper()
{
}
