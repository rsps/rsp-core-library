/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef TESTS_HELPERS_TESTENGINE_H_
#define TESTS_HELPERS_TESTENGINE_H_

#include <graphics/GfxEngine.h>
#include <graphics/GfxEvents.h>
#include <messaging/EventBroker.h>
#include <scenes/Scenes.h>

class TestEngine : public rsp::graphics::GfxEngine<Scenes>, public rsp::messaging::SubscriberInterface
{
public:
    TestEngine() : GfxEngine<Scenes>(1000) {
        mBroker.Subscribe(*this);
    }

    void Run() {
        while(!mTerminated) {
            Iterate();
        }
    }

    void Terminate() {
        mTerminated = true;
    }

    bool ProcessEvent(rsp::messaging::Event &arEvent) override
    {
        if (arEvent.IsType<rsp::graphics::QuitEvent>()) {
            Terminate();
        }
        return false;
    }

protected:
    bool mTerminated = false;
};


#endif /* TESTS_HELPERS_TESTENGINE_H_ */
