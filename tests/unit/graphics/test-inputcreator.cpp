/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <doctest.h>
#include <graphics/TouchParser.h>
#include <vector>

using namespace rsp::graphics;

TEST_CASE("InputCreator Test")
{
    // Arrange
    std::vector<TouchEvent::Types> inputs = {
        TouchEvent::Types::Press,
        TouchEvent::Types::Lift,
        TouchEvent::Types::Press,
        TouchEvent::Types::Press,
        TouchEvent::Types::Lift,
        TouchEvent::Types::Lift,
        TouchEvent::Types::Press,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Drag,
        TouchEvent::Types::Lift};
    uint counter = 0;
    TouchParser tp("testImages/touchTest.bin");

    // Act
    TouchEvent event;

    while (tp.Poll(event)) {
        // Assert
        CHECK(event.mType == inputs[counter]);
        counter++;
    }
}
