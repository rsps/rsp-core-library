/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_LOGGING_LOGSTREAM_H_
#define INCLUDE_LOGGING_LOGSTREAM_H_

#include <ostream>
#include <sstream>
#include <utils/DynamicData.h>
#include "LogTypes.h"

namespace rsp::logging {

class LoggerInterface;

/**
 * \class LogStream
 *
 * \brief The LogStream with template streaming operator declaration
 *
 * Because of the template streaming operator and the scoped factory use of this class
 * in LoggerInterface, it impossible to implement a good interface for this class.
 */
class LogStream
{
public:
    LogStream(LoggerInterface *apOwner, LogLevel aLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext);
    LogStream(const LogStream &arOther) = delete;
    LogStream(LogStream &&arOther); /* No copy, move is OK */
    virtual ~LogStream();

    LogStream& operator=(const LogStream &arOther) = delete;
    LogStream& operator=(LogStream &&arOther);

    LogLevel GetLevel() const;
    void SetLevel(LogLevel aLevel);

    LogStream& SetChannel(const std::string &arChannel);
    LogStream& SetContext(rsp::utils::DynamicData& arContext);

    template< class type>
    LogStream& operator<<(const type &arValue) {
        mBuffer << arValue;
        return *this;
    }

    LogStream& operator<<(std::ostream&(*apFunc)(std::ostream&));
    LogStream& operator<<(LogLevel aLevel);

protected:
    LoggerInterface *mpOwner;
    LogLevel mLevel;
    std::string mChannel;
    rsp::utils::DynamicData mContext;
    std::stringstream mBuffer{};

    void flush();
    void ownerWrite(const std::string &arMsg);
};


} /* namespace rsp::logging */

#endif /* INCLUDE_LOGGING_LOGSTREAM_H_ */
