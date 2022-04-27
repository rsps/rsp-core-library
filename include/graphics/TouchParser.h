/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef TOUCHPARSER_H
#define TOUCHPARSER_H

#include <graphics/TouchEvent.h>
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
 * \brief Streaming operator to stream RawTouchEvent
 */
std::ostream &operator<<(std::ostream &os, const RawTouchEvent &arTouchEvent);


/**
 * \class TouchParser
 * \brief Read and parse input events from kernel device.
 *
 * A polling interface is implemented, since the device handle will always
 * flag readable to select or epoll. Reading the value could then return
 * 0 bytes if empty, so this could be done in one read operation.
 */
class TouchParser
{
public:
    TouchParser(const std::string &arPath = "/dev/input/event1");
    ~TouchParser();

    /**
     * \brief Parse input from touch driver
     * \param Reference to the touch event object to be populated
     * \return bool True if the event is successfully filled
     */
    bool Poll(TouchEvent &arInput);

    /**
     * \brief Flush the input buffer for remaining raw touch events
     */
    void Flush();

protected:
    rsp::posix::FileIO mTouchDevice{};
    RawTouchEvent mRawTouchEvent{};

    TouchEvent::Types readType();
    void readBody(TouchEvent &arInput);
    void readRawTouchEvent();
};

} // namespace rsp::graphics
#endif // TOUCHPARSER_H
