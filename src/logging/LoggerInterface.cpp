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
#include <logging/Logger.h>

namespace rsp::logging {

std::shared_ptr<LoggerInterface> LoggerInterface::mpDefaultInstance = nullptr;


std::shared_ptr<LoggerInterface> LoggerInterface::GetDefault()
{
    if (!mpDefaultInstance) {
        mpDefaultInstance = std::make_shared<Logger>(true);
    }

    return mpDefaultInstance;
}

void LoggerInterface::DestroyDefault()
{
    mpDefaultInstance.reset();
}

} /* namespace rsp::logging */
