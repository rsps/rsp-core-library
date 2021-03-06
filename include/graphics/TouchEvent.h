/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <graphics/primitives/Point.h>

namespace rsp::graphics
{

class TouchEvent
{
  public:
    /**
     * \brief Enum type specifying the different types of touch events
     *
     */
    enum class Types {
        None,
        Press,
        Drag,
        Lift
    };

    Types mType = Types::None;
    Point mPoint{};  /* Value of the absolute X coordinate from touch */
};

std::ostream &operator<<(std::ostream &os, const TouchEvent &arTouchEvent);

} // namespace rsp::graphics

#endif // INPUT_H
