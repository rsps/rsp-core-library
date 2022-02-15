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
Scene::Scene(Rect &aRect)
    : Control(aRect)
{
}

Scene::~Scene()
{
}

void Scene::ProcessInput(Input &aInput)
{
    for (TouchArea *area : mTouchables) {
        area->ProcessInput(aInput);
    }
}

void Scene::Render(Canvas &aCanvas)
{
    // std::cout << "Rendering" << std::endl;
    for (auto child : mChildren) {
        child->Render(aCanvas);
    }
}

/*void Scene::Invalidate()
{
    for (auto child : mChildren) {
        child->Invalidate();
    }
}*/

void Scene::AddChildren(Image &aImg)
{
    mChildren.push_back(&aImg);
}

void Scene::AddArea(TouchArea &aArea)
{
    mTouchables.push_back(&aArea);
}
} // namespace rsp::graphics