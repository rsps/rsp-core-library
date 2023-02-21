/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <logging/OutStreamBuffer.h>
#include <logging/Logger.h>

//#define DEBUG(a) DLOG(a)
#define DEBUG(a)

namespace rsp::logging {

OutStreamBuffer::OutStreamBuffer(LoggerInterface *apOwner, LogLevel aLevel)
    : std::streambuf(),
      LogStream(apOwner, aLevel, std::string(), rsp::utils::DynamicData())
{
}

int OutStreamBuffer::overflow(int c)
{
    if (c != EOF) {
        mBuffer << static_cast<char>(c);
    }
    else {
        mBuffer << '#';
        sync();
    }

    return c;
}

int OutStreamBuffer::sync()
{
    if (mMutex.try_lock()) {
        DEBUG("OutStreamBuffer mutex was not locked!!! " << std::this_thread::get_id());
    }

    std::size_t l = std::size_t(mBuffer.rdbuf()->in_avail());
    if (l > 0) {
        DEBUG("Message: (" << l << ") " << mBuffer);
        std::string result = mBuffer.str();
        if (result[l-1] == '\n') {
            result.pop_back();
        }
        ownerWrite(result);
        mBuffer.clear();
    }
    mMutex.unlock();
    DEBUG("Unlocked by " << std::this_thread::get_id());
    return 0;
}


} /* namespace rsp::logging */

