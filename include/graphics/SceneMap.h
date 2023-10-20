/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_GRAPHICS_SCENE_MAP_H
#define RSP_CORE_LIB_GRAPHICS_SCENE_MAP_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <exceptions/CoreException.h>
#include <graphics/Scene.h>
#include <messaging/Notifier.h>
#include <logging/LogChannel.h>
#include <magic_enum.hpp>

namespace rsp::graphics {

class SceneNotFound : public rsp::exceptions::CoreException
{
public:
    explicit SceneNotFound(const std::string &arName) : rsp::exceptions::CoreException("Scene " + arName + " does not exist") {};
};

class ActiveSceneNotSet : public rsp::exceptions::CoreException
{
public:
    ActiveSceneNotSet() : rsp::exceptions::CoreException("No scene has been set active") {};
};

class SceneMap
{
public:
    using SceneCreator = std::function<std::unique_ptr<Scene>()>;
    using SceneNotify = rsp::messaging::Notifier<Scene&>;

    SceneMap() : mLogger("Gfx") {};

    template <typename T>
    void AddFactory()
    {
        mLogger.Info() << "Adding scene factory: " << rsp::utils::NameOf<T>() << " with id: " << rsp::utils::ID<T>();
        mScenes[rsp::utils::ID<T>()] = [this]() {
            mLogger.Info() << "Creating scene: " << rsp::utils::NameOf<T>();
            return std::make_unique<T>();
        };
    }

    template<typename T, typename E, typename = typename std::enable_if<std::is_enum<E>::value, E>::type>
    void AddFactory(E e)
    {
        std::string_view sv = magic_enum::enum_name(e);
        mLogger.Info() << "Adding scene factory: " << rsp::utils::NameOf<T>() << " with id: " << sv;
        mScenes[uint32_t(e)] = [id=uint32_t(e), sv, this]() {
            mLogger.Info() << "Creating scene: " << rsp::utils::NameOf<T>();
            auto result = std::make_unique<T>();
            result->SetId(id);
            result->SetName(sv.data());
            return result;
        };
    }

    SceneCreator GetFactory(std::uint32_t aId);

    bool HasActiveScene() { return (mpActiveScene != nullptr); }

    void SetActiveScene(std::uint32_t aId);
    template <class T>
    void SetActiveScene() { SetActiveScene(rsp::utils::ID<T>()); }

    Scene& ActiveScene();
    template<class T>
    T& ActiveSceneAs() { return dynamic_cast<T&>(ActiveScene()); }


    SceneNotify& GetAfterCreate() { return mOnCreated; }
    SceneNotify& GetBeforeDestroy() { return mOnDestroy; }

protected:
    std::map<std::uint32_t, SceneCreator> mScenes{};
    std::unique_ptr<Scene> mpActiveScene = nullptr;
    SceneNotify mOnCreated{};
    SceneNotify mOnDestroy{};
    uint32_t mCurrentSceneId = 0;
    rsp::logging::LogChannel mLogger;
};

} /* namespace rsp::graphics */

#endif // RSP_CORE_LIB_GRAPHICS_SCENE_MAP_H
