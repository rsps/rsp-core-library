/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/InputCreator.h"
#include <doctest.h>
#include <vector>

using namespace rsp::graphics;

TEST_CASE("InputCreator Test")
{
    // Arrange
    std::vector<InputType> inputs = {
        InputType::Press,
        InputType::Lift,
        InputType::Press,
        InputType::Press,
        InputType::Lift,
        InputType::Lift,
        InputType::Press,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Drag,
        InputType::Lift};
    uint counter = 0;
    InputCreator inputCreator("testImages/touchTest.bin");

    // Act
    while (inputCreator.HasNewInputs()) {
        // Assert
        CHECK(inputCreator.GetInput().type == inputs[counter]);
        counter++;
    }
}