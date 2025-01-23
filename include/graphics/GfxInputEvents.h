/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_GRAPHICS_GFX_INPUT_EVENTS_H
#define RSP_CORE_LIB_GRAPHICS_GFX_INPUT_EVENTS_H

#include <utils/Singleton.h>
#include "GfxEvents.h"
#include "Point.h"
#include <utils/Timer.h>

namespace rsp::graphics {

/**
 * \brief Interface for GFX events parser
 */
class GfxInputEvents : public rsp::utils::Singleton<GfxInputEvents>
{
public:
    explicit GfxInputEvents(bool aSelfRegister = true)
    {
        if (aSelfRegister) {
            SetInstance(this);
        }
    }

    GfxInputEvents(GfxInputEvents &&) = default;

    ~GfxInputEvents() override
    {
        if (&GetInstance() == this) {
            SetInstance(nullptr);
        }
    }

    GfxInputEvents& operator=(GfxInputEvents &&) = default;

    /**
     * \brief Parse input from event driver
     * \param Reference to the event object to be populated
     * \return bool True if the event is successfully filled
     */
    virtual bool Poll(GfxEvent &arEvent) = 0;

    /**
     * \brief Flush the input buffer for remaining input events
     */
    virtual void Flush() = 0;
};

} /* namespace rsp::graphics */

#endif // RSP_CORE_LIB_GRAPHICS_GFX_INPUT_EVENTS_H
