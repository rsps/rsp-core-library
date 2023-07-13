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
    const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
    const char* cFontName = "Exo 2";

    CHECK_NOTHROW(Font::RegisterFont(cFontFile));
    CHECK_NOTHROW(Font::SetDefaultFont(cFontName));

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

