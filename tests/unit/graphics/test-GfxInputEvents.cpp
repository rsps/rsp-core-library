/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#ifndef USE_GFX_SDL

#include <doctest.h>
#include <graphics/GfxInputEvents.h>
#include <graphics/SW/TouchParser.h>
#include <vector>

using namespace rsp::graphics;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("GfxInputEvents")
{
    // Arrange
    std::vector<TouchTypes> inputs = {
        TouchTypes::Press,
        TouchTypes::Lift,
        TouchTypes::Press,
        TouchTypes::Press,
        TouchTypes::Lift,
        TouchTypes::Lift,
        TouchTypes::Press,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Drag,
        TouchTypes::Lift};
    uint counter = 0;
    sw::TouchParser::mpDevicePath = "testImages/touchTest.bin";

    sw::TouchParser tp;

    // Act
    GfxEvent event;

    while (tp.Poll(event)) {
        // Assert
        CHECK(event->CastTo<TouchEvent>().mType == inputs[counter]);
        counter++;
    }
}

TEST_SUITE_END();

#endif // USE_GFX_SDL
