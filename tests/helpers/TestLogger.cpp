/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <memory>
#include <logging/ConsoleLogWriter.h>
#include "TestLogger.h"


rsp::logging::LogLevel TestLogger::mLogLevel = rsp::logging::LogLevel::Notice;

TestLogger::TestLogger()
{
    mConsoleLogWriter = MakeLogWriter<rsp::logging::ConsoleLogWriter>(mLogLevel);
}

TestLogger::~TestLogger()
{
    rsp::logging::LoggerInterface::DestroyDefault();
}
