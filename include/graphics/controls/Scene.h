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
#include "Control.h"
#include <vector>

namespace rsp::graphics {

class Scene: public Control
{
public:
    static void SetScreenSize(int aWidth, int aHeight);

    Scene(const rsp::utils::TypeInfo &arInfo)
        : Control(arInfo)
    {
        SetArea(mScreenSize);
    }
    Scene(const Rect &arRect, const rsp::utils::TypeInfo &arInfo)
        : Control(arInfo)
    {
        SetArea(arRect);
    }

    virtual ~Scene()
    {
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
     * \brief Processes input in all touchable areas within Scene
     * \param arInput Reference to the input being processed
     */
    void ProcessInput(TouchEvent &arInput);

protected:
    static Rect mScreenSize;
    std::vector<TouchControl*> mTouchables { };

    void removeTouchable(TouchControl *apTouchControl) override;
    void addTouchable(TouchControl *apTouchControl) override;
};

template<class T>
class SceneBase: public Scene
{
public:
    SceneBase()
        : Scene(rsp::utils::MakeTypeInfo<T>())
    {
    }

    SceneBase(const Rect &arRect)
        : Scene(arRect, rsp::utils::MakeTypeInfo<T>())
    {
    }

    static constexpr std::uint32_t ID = rsp::utils::crc32::HashOf<T>();

    static constexpr std::string_view NAME = rsp::utils::NameOf<T>();
};

} // namespace rsp::graphics

#endif // SCENE_H
