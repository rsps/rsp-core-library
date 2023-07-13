/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef TESTS_HELPERS_TESTLOGGER_H_
#define TESTS_HELPERS_TESTLOGGER_H_

#include <logging/Logger.h>


class TestLogger : public rsp::logging::Logger
{
public:
    TestLogger();
    ~TestLogger();

    static rsp::logging::LogLevel mLogLevel;
};

#endif /* TESTS_HELPERS_TESTLOGGER_H_ */
