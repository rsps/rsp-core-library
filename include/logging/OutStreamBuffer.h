/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_LOGGING_OUT_STREAM_BUFFER_H
#define RSP_CORE_LIB_LOGGING_OUT_STREAM_BUFFER_H

#include <iostream>
#include <mutex>
#include <thread>
#include "LoggerInterface.h"
#include "LogStream.h"
#include <utils/ThreadGuard.h>

namespace rsp::logging {

#define DEBUG(a)
//#define DEBUG(a) DLOG("OutStreamBuffer - " << a)

/**
 * \class OutStreamBuf
 *
 * \brief A std::streambuf implementation of LogStream can be used to replace std::cout/cerr/clog streambuf's.
 *
 * This is not intended to be used directly, instead simply instantiate a Logger with the aCaptureClog argument.
 */
class OutStreamBuffer : public std::streambuf, public LogStream
{
public:
    OutStreamBuffer(LoggerInterface *apOwner, LogLevel aLevel);

    void Lock() { mLock.Lock(); }

protected:
    rsp::utils::ThreadGuard mLock{};

    int overflow(int c) override;
    int sync() override;
};


} /* namespace rsp::logging */

#endif // RSP_CORE_LIB_LOGGING_OUT_STREAM_BUFFER_H
