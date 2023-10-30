/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_LOGGING_LOG_STREAM_H
#define RSP_CORE_LIB_LOGGING_LOG_STREAM_H

#include <ostream>
#include <sstream>
#include <utils/DynamicData.h>
#include "LogTypes.h"
#include "SetLevel.h"
#include "SetContext.h"
#include "SetChannel.h"

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
    LogStream(LoggerInterface *apOwner, LogLevel aLevel);
    LogStream(const LogStream &arOther);
    LogStream(LogStream &&arOther) noexcept; /* No copy, move is OK */
    virtual ~LogStream();

    LogStream& operator=(const LogStream &arOther);
    LogStream& operator=(LogStream &&arOther) noexcept;

    /**
     * \brief Get the current stream acceptance log level
     *
     * \return LogLevel
     */
    LogLevel GetLevel() const;

    /**
     * \brief Set the current stream acceptance log level
     *
     * \param aLevel
     * \return self
     */
    LogStream& SetLevel(LogLevel aLevel);

    /**
     * \brief Set the current stream channel
     *
     * \param arChannel
     * \return self
     */
    LogStream& SetChannel(const std::string &arChannel);

    /**
     * \brief Set the current stream context
     *
     * \param arContext
     * \return self
     */
    LogStream& SetContext(const rsp::utils::DynamicData &arContext);

    /**
     * \brief Template to declare streaming operators for individual types
     *
     * \tparam type
     * \param arValue
     * \return self
     */
    template< class T>
    LogStream& operator<<(const T& arValue) {
        mBuffer << arValue;
        return *this;
    }

    /**
     * \brief Streaming operator specialization  for functions
     *
     * \param apFunc
     * \return self
     */
    LogStream& operator<<(std::ostream&(*apFunc)(std::ostream&));

    /**
     * \brief Streaming operator specialization for SetLevel objects
     * \param arLevel
     * \return self
     */
    LogStream& operator<<(const class SetLevel &arLevel);

    /**
     * \brief Streaming operator specialization for SetContext objects
     * \param arContext
     * \return self
     */
    LogStream& operator<<(const class SetContext& arContext);

    /**
     * \brief Streaming operator specialization for SetChannel objects
     * \param arChannel
     * \return self
     */
    LogStream& operator<<(const class SetChannel& arChannel);

protected:
    LoggerInterface *mpLogger;
    LogLevel mLevel;
    std::string mChannel{};
    rsp::utils::DynamicData mContext{};
    std::stringstream mBuffer{};

    void flush();
    void writeToLogger(const std::string &arMsg);
};

} /* namespace rsp::logging */

#endif // RSP_CORE_LIB_LOGGING_LOG_STREAM_H
