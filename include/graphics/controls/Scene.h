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

#include "Control.h"
#include <vector>

namespace rsp::graphics {

class Scene: public Control
{
public:
    static void SetScreenSize(GuiUnit_t aWidth, GuiUnit_t aHeight);

    Scene(const rsp::utils::TypeInfo &arInfo, const Rect &arRect = mScreenSize)
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

protected:
    static Rect mScreenSize;
};

template<class T>
class SceneBase: public Scene
{
public:
    TYPEINFO(T)

    SceneBase()
        : Scene(rsp::utils::MakeTypeInfo<T>())
    {
    }

    SceneBase(const Rect &arRect)
        : Scene(arRect, rsp::utils::MakeTypeInfo<T>())
    {
    }
};

} // namespace rsp::graphics

#endif // SCENE_H
