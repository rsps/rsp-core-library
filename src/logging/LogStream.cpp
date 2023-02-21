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

LogStream::LogStream(LoggerInterface *apOwner, LogLevel aLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext)
    : mpOwner(apOwner),
      mLevel(aLevel),
      mChannel(arChannel),
      mContext(arContext)
{
}

LogStream::LogStream(LogStream &&arOther)
    : mpOwner(std::move(arOther.mpOwner)),
      mLevel(std::move(arOther.mLevel)),
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

LogStream& LogStream::operator=(LogStream &&arOther)
{
    if (&arOther != this) {
        mpOwner = std::move(arOther.mpOwner);
        mLevel = std::move(arOther.mLevel);
        mChannel = std::move(arOther.mChannel);
        mContext = std::move(arOther.mContext);
        mBuffer = std::move(arOther.mBuffer);
    }
    return *this;
}

void LogStream::flush()
{
    if (mBuffer.rdbuf()->in_avail() > 0) {
        ownerWrite(mBuffer.str());
        mBuffer.clear();
    }
}

void LogStream::ownerWrite(const std::string &arMsg)
{
    mpOwner->write(*this, arMsg, mChannel, mContext);
}

LogLevel LogStream::GetLevel() const
{
    return mLevel;
}

void LogStream::SetLevel(LogLevel aLevel)
{
    mLevel = aLevel;
}

LogStream& LogStream::SetChannel(const std::string &arChannel)
{
    mChannel = arChannel;
    return *this;
}

LogStream& LogStream::SetContext(rsp::utils::DynamicData &arContext)
{
    mContext = arContext;
    return *this;
}

LogStream& LogStream::operator<<(std::ostream&(*apFunc)(std::ostream&))
{
    mBuffer << apFunc;
    return *this;
}

LogStream& LogStream::operator <<(LogLevel aLevel)
{
    SetLevel(aLevel);
    return *this;
}

} /* namespace rsp::logging */

