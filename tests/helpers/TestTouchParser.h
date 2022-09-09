/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef TESTS_HELPERS_TESTTOUCHPARSER_H_
#define TESTS_HELPERS_TESTTOUCHPARSER_H_

#include <graphics/TouchParser.h>

class TestTouchParser: public rsp::graphics::TouchParser
{
public:
    TestTouchParser(): TouchParser(std::string()) {};
    TestTouchParser(const TestTouchParser&) = delete;
    TestTouchParser& operator=(const TestTouchParser&) = delete;

    TestTouchParser& SetEvents(const rsp::graphics::TouchEvent *apTouchEvents, std::size_t aCount);

    bool Poll(rsp::graphics::TouchEvent &arInput) override;

    void Flush() override;

protected:
    const rsp::graphics::TouchEvent *mpTouchEvents = nullptr;
    std::size_t mEventCount = 0;
    std::size_t mIndex = 0;
};

#endif /* TESTS_HELPERS_TESTTOUCHPARSER_H_ */
