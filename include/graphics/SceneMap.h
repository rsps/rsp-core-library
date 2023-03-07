/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_SCENEMAP_H_
#define INCLUDE_GRAPHICS_SCENEMAP_H_

#include <exceptions/CoreException.h>
#include <graphics/Scene.h>
#include <map>
#include <string>
#include <functional>
#include <utils/Function.h>
#include <logging/Logger.h>

namespace rsp::graphics {

class SceneNotFound : public rsp::exceptions::CoreException
{
public:
    SceneNotFound(const std::string &arName) : rsp::exceptions::CoreException("Scene " + arName + " does not exist") {};
};

class ActiveSceneNotSet : public rsp::exceptions::CoreException
{
public:
    ActiveSceneNotSet() : rsp::exceptions::CoreException("No scene has been set activate") {};
};

class SceneMap
{
public:
    using SceneCreator = std::function<Scene*()>;
    using SceneNotify = rsp::utils::Function<void(Scene*)>;

    SceneMap() {};
    SceneMap(const SceneMap&) = default;

    #define AddFactory(T) \
        GFXLOG("Adding scene factory: " << T::NAME << " with id: " << T::ID); \
        mScenes[rsp::utils::ID<T>()] = []() { \
            Scene* result = new T(); \
            GFXLOG("Created scene: " << result->GetName()); \
            return result; \
        }

    SceneMap& operator=(const SceneMap&) = default;

    SceneCreator operator[](std::uint32_t aId);

    bool HasActiveScene() { return (mpActiveScene != nullptr); }

    void SetActiveScene(std::uint32_t aId);
    template <class T>
    void SetActiveScene() { SetActiveScene(rsp::utils::ID<T>()); }

    Scene& ActiveScene();
    template<class T>
    T& ActiveScene() { return dynamic_cast<T&>(ActiveScene()); }


    SceneNotify& GetAfterCreate() { return mOnCreated; }
    SceneNotify& GetBeforeDestroy() { return mOnDestroy; }

protected:
    std::map<std::uint32_t, SceneCreator> mScenes{};
    Scene *mpActiveScene = nullptr;
    SceneNotify mOnCreated{};
    SceneNotify mOnDestroy{};
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_SCENEMAP_H_ */
