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
#include <functional>

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

void Scene::AddChildren(Control &aCtrl)
{
    mChildren.push_back(&aCtrl);
}

void Scene::AddArea(TouchArea &aArea)
{
    aArea.RegisterOnClicked(std::bind(&Scene::publishToBroker, *this, std::placeholders::_1, std::placeholders::_2));
    mTouchables.push_back(&aArea);
}
} // namespace rsp::graphics