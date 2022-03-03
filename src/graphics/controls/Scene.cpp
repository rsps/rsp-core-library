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

Scene::Scene(const Rect &aRect, BrokerBase &arBroker)
    : Control(aRect), Publisher(arBroker)
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
    for (auto child : mChildren) {
        child->Render(aCanvas);
    }
}

void Scene::BindElementsToBroker()
{
    for (auto touchable : mTouchables) {
        touchable->RegisterOnClicked(std::bind(&BrokerBase::doPublish, &mrBroker, std::placeholders::_1, std::placeholders::_2));
    }
}

} // namespace rsp::graphics
