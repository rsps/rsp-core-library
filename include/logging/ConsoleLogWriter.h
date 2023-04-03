/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_LOGGING_CONSOLELOGWRITER_H_
#define SRC_LOGGING_CONSOLELOGWRITER_H_

#include <logging/LogWriterInterface.h>
#include <array>
#include <magic_enum.hpp>

namespace rsp::logging {

/**
 * \class ConsoleLogStreamInterface
 * \brief Helper interface for console streams.
 */
class ConsoleLogStreamsInterface
{
public:
    virtual ~ConsoleLogStreamsInterface() {};

    virtual void Info(const std::string &arMsg) = 0;
    virtual void Error(const std::string &arMsg) = 0;
};


/**
 * \class ConsoleLogWriter
 * \brief A log writer for console output.
 *
 * The writer supports ANSI terminal colors.
 */
class ConsoleLogWriter: public LogWriterInterface {
public:
	using ConsoleColors_t = std::array<const std::string, std::size_t(magic_enum::enum_count<LogLevel>())>;

    ConsoleLogWriter(std::string aAcceptLevel, ConsoleLogStreamsInterface *apConsole = nullptr, const ConsoleColors_t *apColors = nullptr);
    ConsoleLogWriter(LogLevel aAcceptLevel, ConsoleLogStreamsInterface *apConsole = nullptr, const ConsoleColors_t *apColors = nullptr);
    ConsoleLogWriter(const ConsoleLogWriter&) = delete;
    ~ConsoleLogWriter();

    void Write(const std::string &arMsg, LogLevel aCurrentLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext) override;

    ConsoleLogWriter& operator= (const ConsoleLogWriter&) = delete;

protected:
    ConsoleLogStreamsInterface* mpConsole;
    const ConsoleColors_t *mpColors;
};

} /* namespace logging */

#endif /* SRC_LOGGING_CONSOLELOGWRITER_H_ */
