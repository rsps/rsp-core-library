/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_LOGGING_LOG_CHANNEL_H
#define RSP_CORE_LIB_LOGGING_LOG_CHANNEL_H

#include <string_view>
#include <logging/LoggerInterface.h>
#include <utils/ConstTypeInfo.h>
#include <utils/DynamicData.h>


namespace rsp::logging {

class LogChannel: public LoggerInterface
{
public:
    explicit LogChannel(std::string_view aChannel);

    LogChannel(const LogChannel &arOther);
    LogChannel& operator=(const LogChannel &arOther);

    rsp::logging::LogStream Warning() override;
    rsp::logging::LogStream Notice() override;
    rsp::logging::LogStream Error() override;
    rsp::logging::LogStream Alert() override;
    rsp::logging::LogStream Critical() override;
    rsp::logging::LogStream Emergency() override;
    rsp::logging::LogStream Info() override;
    rsp::logging::LogStream Debug() override;

    [[nodiscard]] size_t GetWritersCount() const override;

protected:
    Handle_t addLogWriter(std::shared_ptr<LogWriterInterface> aWriter) override;
    void write(const LogStream &arStream, const std::string &arMsg, const std::string &arChannel, const utils::DynamicData &arContext) override;

protected:
    std::string mChannel;
};

template <class T>
class NamedLogger
{
public:
    NamedLogger() : NamedLogger(rsp::utils::NameOf<T>()) {}
    explicit NamedLogger(std::string_view aName) : mLogger(aName) {}
    virtual ~NamedLogger() = default;

    NamedLogger(const NamedLogger<T>& arOther) : mLogger(arOther.mLogger) {}
    NamedLogger(NamedLogger<T>&& arOther) noexcept : mLogger(std::move(arOther.mLogger)) {}

protected:
    LogChannel mLogger;
};

} /* namespace rsp::logging */

#endif // RSP_CORE_LIB_LOGGING_LOG_CHANNEL_H
