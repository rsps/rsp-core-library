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
#include <functional>
#include <utils/CoreException.h>
#include <utils/Function.h>
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
    using SceneCreator = std::function<Scene*()>;
    using SceneNotify = rsp::utils::Function<void(Scene &)>;

    SceneMap() {};
    SceneMap(const SceneMap&) = default;

    #define AddFactory(T) \
        std::cout << "Creating scene: " << #T << std::endl; \
        mScenes[T::ID] = []() { return new T(); }

    SceneMap& operator=(const SceneMap&) = default;

    SceneCreator operator[](std::uint32_t aId);

    void SetActiveScene(std::uint32_t aId);
    Scene& ActiveScene();

    template<class T>
    T& ActiveSceneAs() { return ActiveScene().GetAs<T>(); }


    SceneNotify& GetAfterCreate() { return mOnCreated; }
    SceneNotify& GetBeforeDestroy() { return mOnDestroy; }

protected:
    std::map<std::uint32_t, SceneCreator> mScenes{};
    Scene *mpActiveScene = nullptr;
    SceneNotify mOnCreated{};
    SceneNotify mOnDestroy{};
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_CONTROLS_SCENEMAP_H_ */
