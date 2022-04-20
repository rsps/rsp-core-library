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

namespace rsp::graphics
{

/**
 * \brief Enum type specifying the different types of input
 *
 */
enum class InputType {
    None,
    Press,
    Drag,
    Lift
};

class Input
{
  public:
    InputType type = InputType::None;
    int x = 0; /*!< Value of the absolute X coordinate from touch */
    int y = 0; /*!< Value of the absolute Y coordinate from touch */
};

} // namespace rsp::graphics

#endif // INPUT_H
