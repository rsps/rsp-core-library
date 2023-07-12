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

#ifndef INCLUDE_LOGGING_LOGGERINTERFACE_H_
#define INCLUDE_LOGGING_LOGGERINTERFACE_H_

namespace rsp::logging {

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
    virtual ~LoggerInterface() {}

    static void SetDefault(LoggerInterface* apLogger);
    static LoggerInterface& GetDefault();

    virtual LogStream Emergency() = 0;
    virtual LogStream Alert() = 0;
    virtual LogStream Critical() = 0;
    virtual LogStream Error() = 0;
    virtual LogStream Warning() = 0;
    virtual LogStream Notice() = 0;
    virtual LogStream Info() = 0;
    virtual LogStream Debug() = 0;

    bool HasWriters() const;

    typedef uintptr_t Handle_t;
    Handle_t AddLogWriter(std::shared_ptr<LogWriterInterface> aWriter);

    void RemoveLogWriter(Handle_t aHandle);

    LoggerInterface& SetChannel(const std::string &arChannel) { mChannel = arChannel; return *this; }
    LoggerInterface& SetContext(rsp::utils::DynamicData &arContext) { mContext = arContext; return *this; }

    virtual void write(const LogStream &arStream, const std::string &arMsg,
                       const std::string &arChannel, const rsp::utils::DynamicData &arContext);
protected:
    static std::shared_ptr<LoggerInterface> mpDefaultInstance;
    std::recursive_mutex mMutex{};
    std::vector<std::shared_ptr<LogWriterInterface>> mWriters{};
    std::string mChannel{};
    rsp::utils::DynamicData mContext{};
};


} /* namespace rsp::logging */

#endif /* INCLUDE_LOGGING_LOGGERINTERFACE_H_ */
