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
#include <graphics/GfxEvents.h>
#include <graphics/SW/TouchParser.h>
#include <vector>

using namespace rsp::graphics;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("GfxEvents")
{
    // Arrange
    std::vector<EventTypes> inputs = {
        EventTypes::Press,
        EventTypes::Lift,
        EventTypes::Press,
        EventTypes::Press,
        EventTypes::Lift,
        EventTypes::Lift,
        EventTypes::Press,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Drag,
        EventTypes::Lift};
    uint counter = 0;
    TouchParser tp("testImages/touchTest.bin");

    // Act
    GfxEvent event;

    while (tp.Poll(event)) {
        // Assert
        CHECK(event.mType == inputs[counter]);
        counter++;
    }
}

TEST_SUITE_END();
