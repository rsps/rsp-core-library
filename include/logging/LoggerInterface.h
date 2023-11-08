/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <algorithm>
#include <memory>
#include <mutex>
#include "LogStream.h"
#include "LogWriterInterface.h"

#ifndef RSP_CORE_LIB_LOGGING_LOGGER_INTERFACE_H
#define RSP_CORE_LIB_LOGGING_LOGGER_INTERFACE_H

namespace rsp::logging {

class LogStream;
class LogChannel;

/**
 * \class Abstract LoggerInterface
 *
 * \brief This is the interface for logging functionality.
 *
 * The logging design is a public logger interface with multiple writers attached to it.
 * A writer can be limited in which log level should trigger its output.
 */
class LoggerInterface
{
public:
    using Handle_t = std::shared_ptr<LogWriterInterface>;

    virtual ~LoggerInterface() = default;

    /**
     * \brief Set the default logger instance for the application.
     *
     * Every application should set the instance very early
     * during execution.
     *
     * \param apLogger
     */
    static void DestroyDefault();

    /**
     * \brief Get the default logger instance.
     *
     * This is used by many modules to get a logger instance.
     *
     * \return shared pointer to LoggerInterface instance
     */
    static LoggerInterface& GetDefault();

    /**
     * \brief Log level stream creators
     * \return LogStream
     */
    virtual LogStream Emergency() = 0;
    virtual LogStream Alert() = 0;
    virtual LogStream Critical() = 0;
    virtual LogStream Error() = 0;
    virtual LogStream Warning() = 0;
    virtual LogStream Notice() = 0;
    virtual LogStream Info() = 0;
    virtual LogStream Debug() = 0;

    /**
     * \brief Get the number of registered log writers.
     *
     * \return size_t
     */
    [[nodiscard]] virtual size_t GetWritersCount() const = 0;


    template<class T, class ... Args>
    Handle_t MakeLogWriter(Args ...args)
    {
        return addLogWriter(std::make_shared<T>(args...));
    }

protected:
    static std::shared_ptr<LoggerInterface> mpDefaultInstance;

    friend class LogStream;
    friend class LogChannel;

    /**
     * \brief Add a log writer to the logger.
     * \param aWriter
     * \return Handle_t Handle to the registration
     */
    virtual Handle_t addLogWriter(std::shared_ptr<LogWriterInterface> aWriter) = 0;

    /**
     * \brief Delegates a log message to the registered writers.
     * \param arStream
     * \param arMsg
     * \param arChannel
     * \param arContext
     */
    virtual void write(const LogStream &arStream, const std::string &arMsg,
                       const std::string &arChannel, const rsp::utils::DynamicData &arContext) = 0;
};


} /* namespace rsp::logging */

#endif // RSP_CORE_LIB_LOGGING_LOGGER_INTERFACE_H
