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
#include "messaging/eventTypes/ClickedEvent.h"
#include <functional>

using namespace rsp::messaging;

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
        touchable->RegisterOnClicked([&](ClickTopic aTopic, ClickedEvent &aEvent) noexcept { PublishToBroker<ClickTopic>(aTopic, aEvent); });
    }
}

} // namespace rsp::graphics
