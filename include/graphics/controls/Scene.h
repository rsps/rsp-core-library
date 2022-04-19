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
#include "Input.h"
#include "TouchArea.h"
#include <vector>

namespace rsp::graphics
{

class Scene : public Control
{
  public:
    Scene(const Rect &arRect, const std::string &arName);
    virtual ~Scene() {}

    /**
     * \brief Get the name of the specific scene.
     *
     * \return string with name of scene
     */
    const std::string& GetName() { return mName; }

    /**
     * \brief Processes input in all touchable areas within Scene
     * \param arInput Reference to the input being processed
     */
    void ProcessInput(Input &arInput);

  protected:
    std::string mName;
    std::vector<TouchArea> mTouchables{};

    TouchArea& makeTouchArea(const Rect &arRect);
};

} // namespace rsp::graphics

#endif // SCENE_H
