/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_CONTROLS_SCENEMAP_H_
#define INCLUDE_GRAPHICS_CONTROLS_SCENEMAP_H_

#include <map>
#include <string>
#include <utils/CoreException.h>
#include "Scene.h"

namespace rsp::graphics {

class SceneNotFound : public rsp::utils::CoreException
{
public:
    SceneNotFound(const std::string &arName) : rsp::utils::CoreException("Scene " + arName + " does not exist") {};
};

class ActiveSceneNotSet : public rsp::utils::CoreException
{
public:
    ActiveSceneNotSet() : rsp::utils::CoreException("No scene has been set activate") {};
};


class SceneMap
{
public:
    SceneMap();
    virtual ~SceneMap();

    template<class T>
    Scene& MakeScene() {
        return add(new T());
    }

    Scene& operator[](const std::string &arName);

    void SetActiveScene(const std::string &arName);
    Scene& ActiveScene();

protected:
    std::unordered_map<std::string, Scene *> mScenes{};
    Scene *mpActiveScene = nullptr;

    Scene& add(Scene *apScene);
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_CONTROLS_SCENEMAP_H_ */
