/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <logging/LoggerInterface.h>

namespace rsp::logging {

std::shared_ptr<LoggerInterface> LoggerInterface::mpDefaultInstance = nullptr;


LoggerInterface& LoggerInterface::GetDefault()
{
    if (!mpDefaultInstance) {
        THROW_WITH_BACKTRACE1(exceptions::NotSetException, "Logger instance not set.");
    }

    return *mpDefaultInstance;
}

void LoggerInterface::SetDefault(LoggerInterface* apLogger)
{
    if (apLogger) {
        mpDefaultInstance = std::shared_ptr<LoggerInterface>(apLogger, [](LoggerInterface*){});
    }
    else {
        mpDefaultInstance.reset();
    }
}

} /* namespace rsp::logging */
