/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/Singleton.h>
#include "GfxEvent.h"
#include "Point.h"

#ifndef INCLUDE_GRAPHICS_GFXEVENTS_H_
#define INCLUDE_GRAPHICS_GFXEVENTS_H_

namespace rsp::graphics {

/**
 * \brief Interface for GFX events parser
 */
class GfxInputEvents
{
public:
    static GfxInputEvents& Get();

    virtual ~GfxInputEvents() {}

    /**
     * \brief Parse input from event driver
     * \param Reference to the event object to be populated
     * \return bool True if the event is successfully filled
     */
    virtual bool Poll(GfxEvent &arInput) = 0;

    /**
     * \brief Flush the input buffer for remaining input events
     */
    virtual void Flush() = 0;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_GFXEVENTS_H_ */
