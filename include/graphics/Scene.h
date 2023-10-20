/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#ifndef RSP_CORE_LIB_GRAPHICS_SCENE_H
#define RSP_CORE_LIB_GRAPHICS_SCENE_H

#include "Control.h"
#include <vector>

namespace rsp::graphics {

class Scene: public Control
{
public:
    static void SetScreenSize(GuiUnit_t aWidth, GuiUnit_t aHeight);

    explicit Scene(const Rect &arRect = mScreenSize)
    {
        SetArea(arRect);
    }

    ~Scene() override;

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
    SceneBase()
    {
        initTypeInfo<T>();
    }

    explicit SceneBase(const Rect &arRect)
        : Scene(arRect)
    {
        initTypeInfo<T>();
    }
};

} // namespace rsp::graphics

#endif // RSP_CORE_LIB_GRAPHICS_SCENE_H
