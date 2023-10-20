/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_LOGGING_LOGGER_H
#define RSP_CORE_LIB_LOGGING_LOGGER_H

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <sstream>
#include <mutex>
#include <iostream>
#include <utils/DynamicData.h>
#include "LoggerInterface.h"
#include "SetLevel.h"

namespace rsp::logging {

// Usage: make CXXFLAGS="-DDEBUG_LOG"
// Usage: cmake -DCMAKE_CXX_FLAGS="-DDEBUG_LOG" ..

#ifdef DEBUG_LOG
#include <iostream>

#define LOG(a) { std::cout << a << std::endl; }
#define DLOG(a) { std::cerr << a << std::endl; }

#else /* DEBUG_LOG */

#define LOG(a)
#define DLOG(a)

#endif /* DEBUG_LOG */

constexpr const char* stem(std::string_view path)
{
    return path.substr(path.find_last_of('/') + 1).data();
}

#define DUMP(a, b) { std::cout << rsp::logging::stem(__FILE__) << ":" << __LINE__ << " " << __FUNCTION__ << "(" << a << ") -> " << b << std::endl; }


/**
 * \class Logger
 *
 * \brief The default application logging instance type.
 */
class Logger : public LoggerInterface
{
public:
    explicit Logger(bool aCaptureClog = false);
    Logger(const Logger&) = delete;
    ~Logger() override;

    LogStream Emergency() override;
    LogStream Alert() override;
    LogStream Critical() override;
    LogStream Error() override;
    LogStream Warning() override;
    LogStream Notice() override;
    LogStream Info() override;
    LogStream Debug() override;

    Logger& operator= (const Logger&) = delete;

protected:
    // Use shared_ptr to use compilers default move operations.
    // It is instantiated with "do nothing" de-allocator in Logger constructor initialization.
    std::shared_ptr<std::streambuf> mpClogBackup;

    std::shared_ptr<std::streambuf> makeCLogStream(bool aCaptureLog);
};

} /* namespace logging */

#endif // RSP_CORE_LIB_LOGGING_LOGGER_H
