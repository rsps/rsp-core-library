/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/controls/SceneMap.h>
#include <stdexcept>
#include <logging/Logger.h>

using namespace rsp::logging;

namespace rsp::graphics {

SceneMap::SceneCreator SceneMap::operator [](std::uint32_t aId)
{
    try {
        return mScenes.at(aId);
    }
    catch (const std::out_of_range &e) {
        THROW_WITH_BACKTRACE1(SceneNotFound, std::to_string(aId));
    }
}

Scene& SceneMap::ActiveScene()
{
    if (!mpActiveScene) {
        THROW_WITH_BACKTRACE(ActiveSceneNotSet);
    }

    GFXLOG("Get Active Scene: " << mpActiveScene->GetName());

    return *mpActiveScene;
}

void SceneMap::SetActiveScene(std::uint32_t aId)
{
    if (mpActiveScene) {
        mOnDestroy(mpActiveScene);
        mpActiveScene->DeInit();
        delete mpActiveScene;
        mpActiveScene = nullptr;
    }

    mpActiveScene = (operator[](aId))();
    Logger::GetDefault().Info() << "SceneChange: " << mpActiveScene->GetName();
    mpActiveScene->Init();
    mOnCreated(mpActiveScene);
}

} /* namespace rsp::graphics */
