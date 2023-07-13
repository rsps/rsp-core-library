/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_LOGGING_LOGCHANNEL_H_
#define INCLUDE_LOGGING_LOGCHANNEL_H_

#include <string_view>
#include <logging/LoggerInterface.h>
#include <utils/ConstTypeInfo.h>


namespace rsp::logging {

class LogChannel: public LoggerInterface
{
public:
    LogChannel(std::string_view aChannel);
    rsp::logging::LogStream Warning() override;
    rsp::logging::LogStream Notice() override;
    rsp::logging::LogStream Error() override;
    rsp::logging::LogStream Alert() override;
    rsp::logging::LogStream Critical() override;
    rsp::logging::LogStream Emergency() override;
    rsp::logging::LogStream Info() override;
    rsp::logging::LogStream Debug() override;

protected:
    LoggerInterface &mrLogger;
    std::string mChannel;
};

template <class T>
class NamedLogger
{
public:
    NamedLogger() : mLogger(rsp::utils::NameOf<T>()) {}
    virtual ~NamedLogger() {};

protected:
    LogChannel mLogger;
};

} /* namespace rsp::logging */

#endif /* INCLUDE_LOGGING_LOGCHANNEL_H_ */
