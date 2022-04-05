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

SceneMap::SceneMap()
{
}

SceneMap::~SceneMap()
{
    for(auto &pair : mScenes) {
        delete pair.second;
    }
}

Scene& SceneMap::operator [](const std::string &arName)
{
    try {
        return *mScenes.at(arName);
    }
    catch (const std::out_of_range &e) {
        THROW_WITH_BACKTRACE1(SceneNotFound, arName);
    }
}

Scene& SceneMap::FirstScene()
{
    if (mScenes.empty()) {
        THROW_WITH_BACKTRACE1(SceneNotFound, "FirstScene");
    }

    return *(mScenes.begin()->second);
}

Scene& SceneMap::add(Scene *apScene)
{
    mScenes[apScene->GetName()] = apScene;
    return *apScene;
}

} /* namespace rsp::graphics */
