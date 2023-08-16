/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/Scene.h>

namespace rsp::graphics
{

Rect Scene::mScreenSize{0, 0, 480, 800};

Scene::~Scene()
{
    mLogger.Debug() << "Scene::Destructor called for " << GetName();
}


void Scene::SetScreenSize(GuiUnit_t aWidth, GuiUnit_t aHeight)
{
    mScreenSize = Rect(0, 0, aWidth, aHeight);
}

} // namespace rsp::graphics
