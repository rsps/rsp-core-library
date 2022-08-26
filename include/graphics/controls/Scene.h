/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#ifndef SCENE_H
#define SCENE_H

#include <graphics/controls/TouchControl.h>
#include <graphics/TouchEvent.h>
#include <utils/ConstTypeInfo.h>
#include "Control.h"
#include <vector>
#include <string_view>

namespace rsp::graphics {

class Scene: public Control
{
public:
    static void SetScreenSize(int aWidth, int aHeight);

    Scene(const std::string &arName, std::uint32_t aId)
        : Control(mScreenSize), mName(arName), mId(aId)
    {
    }
    Scene(const Rect &arRect, const std::string &arName, std::uint32_t aId)
        : Control(arRect), mName(arName), mId(aId)
    {
    }

    virtual ~Scene()
    {
    }

    template<class T>
    T& GetAs()
    {
        return *static_cast<T*>(this);
    }

    /**
     * \brief Activation function, called after scene has been fully constructed
     */
    virtual void Init()
    {
    }

    /**
     * \brief De-activation function, called before scene is destroyed, while it is still a fully valid object.
     */
    virtual void DeInit()
    {
    }

    /**
     * \brief Get the name of the specific scene.
     *
     * \return string with name of scene
     */
    const std::string& GetName()
    {
        return mName;
    }

    std::uint32_t GetId()
    {
        return mId;
    }

    /**
     * \brief Processes input in all touchable areas within Scene
     * \param arInput Reference to the input being processed
     */
    void ProcessInput(TouchEvent &arInput);

protected:
    static Rect mScreenSize;
    std::string mName;
    std::uint32_t mId;
    std::vector<TouchControl*> mTouchables { };

    void removeTouchable(TouchControl *apTouchControl) override;
    void addTouchable(TouchControl *apTouchControl) override;
};

template<class T>
class SceneBase: public Scene
{
public:
    SceneBase()
        : Scene(std::string(NAME), ID)
    {
    }

    SceneBase(const Rect &arRect)
        : Scene(arRect, std::string(NAME), ID)
    {
    }

    static constexpr std::uint32_t ID = rsp::utils::crc32::HashOf<T>();

    static constexpr std::string_view NAME = rsp::utils::NameOf<T>();
};

} // namespace rsp::graphics

#endif // SCENE_H
