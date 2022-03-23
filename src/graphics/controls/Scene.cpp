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

using namespace rsp::messaging;

namespace rsp::graphics
{

Scene::Scene(const Rect &arRect, Broker<ClickTopics> &arBroker)
    : Control(arRect),
      Publisher(arBroker)
{
}

void Scene::ProcessInput(Input &arInput)
{
    for (TouchArea *area : mTouchables) {
        area->ProcessInput(arInput);
    }
}

void Scene::Render(Canvas &arCanvas)
{
    for (Control* child : mChildren) {
        child->Render(arCanvas);
    }
}

void Scene::BindElementsToBroker()
{
    for (TouchArea* touchable : mTouchables) {
        touchable->RegisterOnClicked([&](ClickTopics aTopic, ClickedEvent &aEvent) noexcept { PublishToBroker(aTopic, aEvent); });
    }
}

} // namespace rsp::graphics
