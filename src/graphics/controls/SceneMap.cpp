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

namespace rsp::graphics {

SceneMap::SceneCreator SceneMap::operator [](const std::string &arName)
{
    try {
        return mScenes.at(arName);
    }
    catch (const std::out_of_range &e) {
        THROW_WITH_BACKTRACE1(SceneNotFound, arName);
    }
}

Scene& SceneMap::ActiveScene()
{
    if (!mpActiveScene) {
        THROW_WITH_BACKTRACE(ActiveSceneNotSet);
    }

    return *mpActiveScene;
}

void SceneMap::SetActiveScene(const std::string &arName)
{
    if (mpActiveScene) {
        mOnDestroy(*mpActiveScene);
        delete mpActiveScene;
        mpActiveScene = nullptr;
    }

    mpActiveScene = (operator[](arName))();
    mOnCreated(*mpActiveScene);
}

} /* namespace rsp::graphics */
