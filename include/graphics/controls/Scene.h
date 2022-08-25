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
#include <string_view>

namespace rsp::graphics {

class Scene: public Control
{
public:
    static void SetScreenSize(int aWidth, int aHeight);

    Scene(const std::string &arName)
        : Control(mScreenSize), mName(arName)
    {
    }
    Scene(const Rect &arRect, const std::string &arName)
        : Control(arRect), mName(arName)
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
    ;
    /**
     * \brief De-activation function, called before scene is destroyed, while it is still a fully valid object.
     */
    virtual void DeInit()
    {
    }
    ;

    /**
     * \brief Get the name of the specific scene.
     *
     * \return string with name of scene
     */
    const std::string& GetName()
    {
        return mName;
    }

    /**
     * \brief Processes input in all touchable areas within Scene
     * \param arInput Reference to the input being processed
     */
    void ProcessInput(TouchEvent &arInput);

protected:
    static Rect mScreenSize;
    std::string mName;
    std::vector<TouchControl*> mTouchables { };

    void removeTouchable(TouchControl *apTouchControl) override;
    void addTouchable(TouchControl *apTouchControl) override;
};

template<class T>
class SceneBase: public Scene
{
public:
    SceneBase()
        : Scene(GetName())
    {
    }

    SceneBase(const Rect &arRect)
        : Scene(arRect, GetName())
    {
    }

    static std::string GetName() {
        return std::string(get_name());
    }

private:
    // https://stackoverflow.com/questions/35941045/can-i-obtain-c-type-names-in-a-constexpr-way/35943472#35943472
    constexpr std::string_view get_name()
    {
        char const *p = __PRETTY_FUNCTION__;
        while (*p++ != '=')
            ;
        for (; *p == ' ' ; ++p)
            ;
        char const *p2 = p;
        int count = 1;
        for (; ; ++p2) {
            switch (*p2) {
                case '[':
                    ++count;
                    break;
                case ']':
                    --count;
                    if (!count) {
                        return {p, std::size_t(p2 - p)};
                    }
                    break;
                default:
                    break;
            }
        }
        return {};
    }
};

} // namespace rsp::graphics

#endif // SCENE_H
