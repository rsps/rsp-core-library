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

namespace rsp::logging {

OutStreamBuffer::OutStreamBuffer(LoggerInterface& arLogger, LogLevel aLevel)
    : std::streambuf(),
      LogStream(arLogger, aLevel)
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
    if (mLock.IsLocked()) {
        DEBUG("OutStreamBuffer mutex was not locked!!! " << std::this_thread::get_id())
    }

    // Remove one ending newline, writeToLogger enforces a newline on every write
    std::string result = mBuffer.str();
    if (result[result.length()-1] == '\n') {
        result.pop_back();
    }

    if (result.length() > 0) {
        DEBUG("Message: (" << result.length() << ") " << result)
        writeToLogger(result);
    }
    mBuffer.str(std::string());

    mLock.Unlock();
    DEBUG("Unlocked by " << std::this_thread::get_id())
    return 0;
}

} /* namespace rsp::logging */
