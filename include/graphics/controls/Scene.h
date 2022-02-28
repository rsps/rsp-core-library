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

#include "graphics/controls/Control.h"
#include "graphics/controls/Input.h"
#include "graphics/controls/TouchArea.h"
#include "messaging/Publisher.h"
#include <vector>

using namespace rsp::messaging;

namespace rsp::graphics
{

class Scene : public Control, public Publisher
{
public:
    Scene(const Rect &aRect);
    ~Scene();

    void ProcessInput(Input &aInput);
    void Render(Canvas &aCanvas) override;
    void BindElementsToBroker();

protected:
    std::vector<TouchArea *> mTouchables{};
};

} // namespace rsp::graphics
#endif // SCENE_H
