/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <logging/LogStream.h>
#include <logging/LoggerInterface.h>

namespace rsp::logging {

LogStream::LogStream(LoggerInterface *apLogger, LogLevel aLevel)
    : mpLogger(apLogger),
      mLevel(aLevel)
{
    mBuffer.imbue(std::locale{"C"});
}

LogStream::LogStream(const LogStream &arOther)
    : mpLogger(arOther.mpLogger),
      mLevel(arOther.mLevel),
      mChannel(arOther.mChannel),
      mContext(arOther.mContext)
{
    mBuffer.imbue(std::locale{"C"});
}

LogStream::LogStream(LogStream &&arOther) noexcept
    : mpLogger(arOther.mpLogger),
      mLevel(arOther.mLevel),
      mChannel(std::move(arOther.mChannel)),
      mContext(std::move(arOther.mContext)),
      mBuffer(std::move(arOther.mBuffer))
{
}

LogStream::~LogStream()
{
    try {
        flush();
    }
    catch (...) {
    }
}

LogStream& LogStream::operator=(const LogStream &arOther)
{
    if (&arOther != this) {
        mpLogger = arOther.mpLogger;
        mLevel   = arOther.mLevel;
        mChannel = arOther.mChannel;
        mContext = arOther.mContext;
    }
    return *this;
}

LogStream& LogStream::operator=(LogStream &&arOther) noexcept
{
    if (&arOther != this) {
        mpLogger = arOther.mpLogger;
        mLevel = arOther.mLevel;
        mChannel = std::move(arOther.mChannel);
        mContext = std::move(arOther.mContext);
        mBuffer = std::move(arOther.mBuffer);
    }
    return *this;
}

void LogStream::flush()
{
    if (mBuffer.rdbuf()->in_avail() > 0) {
        writeToLogger(mBuffer.str());
        mBuffer.clear();
    }
}

void LogStream::writeToLogger(const std::string &arMsg)
{
    mpLogger->write(*this, arMsg, mChannel, mContext);
}

LogLevel LogStream::GetLevel() const
{
    return mLevel;
}

LogStream& LogStream::SetLevel(LogLevel aLevel)
{
    mLevel = aLevel;
    return *this;
}

LogStream& LogStream::SetChannel(const std::string &arChannel)
{
    mChannel = arChannel;
    return *this;
}

LogStream& LogStream::SetContext(const rsp::utils::DynamicData &arContext)
{
    mContext = arContext;
    return *this;
}

LogStream& LogStream::operator<<(std::ostream&(*apFunc)(std::ostream&))
{
    mBuffer << apFunc;
    return *this;
}

LogStream& LogStream::operator <<(const class SetLevel &arLevel)
{
    return SetLevel(arLevel.mValue);
}

LogStream &LogStream::operator<<(const class SetContext &arContext)
{
    return SetContext(arContext.mValue);
}

LogStream &LogStream::operator<<(const class SetChannel &arChannel)
{
    return SetChannel(arChannel.mValue);
}

} /* namespace rsp::logging */
