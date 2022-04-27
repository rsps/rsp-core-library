/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/controls/Scene.h>
#include <functional>

namespace rsp::graphics
{

Scene::Scene(const Rect &arRect, const std::string &arName)
    : Control(arRect),
      mName(arName)
{
}

void Scene::ProcessInput(TouchEvent &arInput)
{
    for (TouchArea &area : mTouchables) {
        area.ProcessInput(arInput);
    }
}

TouchArea& Scene::makeTouchArea(const Rect &arRect)
{
    return mTouchables.emplace_back().SetArea(arRect);
}

} // namespace rsp::graphics
