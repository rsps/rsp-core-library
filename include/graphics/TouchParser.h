/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef INPUTCREATOR_H
#define INPUTCREATOR_H

#include "graphics/controls/Input.h"
#include "posix/FileIO.h"
#include <fstream>
#include <iostream>

namespace rsp::graphics
{

/**
 * \brief Defines the values given on each line by touch driver
 *
 */
struct RawTouchEvent {
    uint32_t stime; /*!< Seconds since Epoch */
    int32_t mtime;  /*!< Signed count of microseconds */
    uint16_t type;
    uint16_t code;
    int32_t value;
} __attribute__((packed));

/**
 * \brief Streaming operator to stream InputLine
 */
std::ostream &operator<<(std::ostream &os, const RawTouchEvent &arTouchEvent);

class InputCreator
{
public:
    InputCreator(const std::string &arPath = "/dev/input/event1");
    ~InputCreator();

    /**
     * \brief Parse input from touch driver
     * \param Reference to the touch event object to be populated
     * \return bool True if event is ready
     */
    bool Poll(Input &arInput);

protected:
    rsp::posix::FileIO mTouchDevice{};
    RawTouchEvent mRawTouchEvent{};

    InputType readType();
    void readBody(Input &arInput);
    void readRawTouchEvent();
};

} // namespace rsp::graphics
#endif // INPUTCREATOR_H
