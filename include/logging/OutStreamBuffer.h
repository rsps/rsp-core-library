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

    void Lock() { mMutex.lock(); }

protected:
    std::mutex mMutex{};

    int overflow(int c) override;
    int sync() override;
};


} /* namespace rsp::logging */

#endif /* INCLUDE_LOGGING_OUTSTREAMBUFFER_H_ */
