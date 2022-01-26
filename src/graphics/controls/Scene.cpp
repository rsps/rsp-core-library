/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/controls/Scene.h"

namespace rsp::graphics
{

void Scene::ProcessEvent(Event &aEvent)
{
    for (TouchArea *area : mTouchables) {
        if (area->IsHit(Point(aEvent.x, aEvent.y))) {
            area->ProcessEvent(aEvent);
        }
    }
}

void Scene::AddArea(TouchArea *aArea)
{
    mTouchables.push_back(aArea);
}
} // namespace rsp::graphics