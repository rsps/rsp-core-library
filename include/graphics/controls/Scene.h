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

namespace rsp::graphics
{

class Scene : public Control, public rsp::messaging::Publisher
{
  public:
    Scene(const Rect &aRect, rsp::messaging::BrokerBase &arBroker);
    virtual ~Scene() {}

    /**
     * \brief Processes input in all touchable areas within Scene
     * \param arInput Reference to the input being processed
     */
    void ProcessInput(Input &arInput);

    /**
     * \brief Render all children within Scene
     * \param arCanvas Reference to the Canvas children will be rendered on
     */
    void Render(Canvas &arCanvas) override;

    /**
     * \brief Register all touchable areas's click callback to the given current Broker
     */
    void BindElementsToBroker();

  protected:
    std::vector<TouchArea *> mTouchables{};
};

} // namespace rsp::graphics
#endif // SCENE_H
