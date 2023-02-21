/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_LOGGING_OUTSTREAMBUFFER_H_
#define INCLUDE_LOGGING_OUTSTREAMBUFFER_H_

#include <iostream>
#include <mutex>
#include "LoggerInterface.h"
#include "LogStream.h"

namespace rsp::logging {

/**
 * \class OutStreamBuf
 *
 * \brief An ostreambuf implementation allowing for std::ostream operations.
 */
class OutStreamBuffer : public std::streambuf, public LogStream
{
public:
    OutStreamBuffer(LoggerInterface *apOwner, LogLevel aLevel);

    void Lock() { mMutex.lock(); }

protected:
    std::mutex mMutex{};

    int overflow(int c) override;
    int sync() override;
};


} /* namespace rsp::logging */

#endif /* INCLUDE_LOGGING_OUTSTREAMBUFFER_H_ */
