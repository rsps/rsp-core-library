/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_APPLICATION_CONSOLE_H
#define RSP_CORE_LIB_APPLICATION_CONSOLE_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utils/AnsiEscapeCodes.h>
#include <logging/ConsoleLogWriter.h>

namespace rsp::application {

/**
 * \enum TextColor
 * \brief Color definitions for console output.
 */
enum class TextColor {
    Info, InfoHighLight, Input, Error, Debug
};

std::ostream& operator<< (std::ostream& os, TextColor aColor);

class Console;

/**
 * \class ConsoleStream
 * \brief Internal stream used to output console text with colors.
 */
class ConsoleStream : public std::stringstream
{
public:
    ConsoleStream(Console *apConsole, TextColor aColor);
    ConsoleStream(ConsoleStream &&aFrom) noexcept;
    ConsoleStream(const ConsoleStream &) = delete;

    ConsoleStream& operator=(const ConsoleStream &) = delete;

    friend class Console;
    ~ConsoleStream() override;

protected:
    Console *mpConsole = nullptr;
    TextColor mColor{};
};

/**
 * \class ConsoleLogStreams
 * \brief Helper log writer class to direct log messages to a console
 */
class ConsoleLogStreams : public rsp::logging::ConsoleLogStreamsInterface
{
public:
    void Error(const std::string &arMsg) override;
    void Info(const std::string &arMsg) override;
};


/**
 * \class Console
 * \brief Class intended for console output.
 *
 * It always writes to std::cout and std::cerr, but when enabled it will also output to a given character device, like a separate LCD screen.
 *
 */
class Console
{
public:
    ~Console();

    static Console& Get();

    static ConsoleStream Debug() { return {&Get(), TextColor::Debug}; }
    static ConsoleStream Error() { return {&Get(), TextColor::Error}; }
    static ConsoleStream HighLightInfo()  { return {&Get(), TextColor::InfoHighLight}; }
    static ConsoleStream Info()  { return {&Get(), TextColor::Info}; }

    static void SetUseColors(bool aEnable) { mUseColors = aEnable; }
    static bool GetUseColors() { return mUseColors; }

    static void SetTtyDevice(const std::string &arTtyDevice);
    static std::string GetTtyDevice() { return mTtyDeviceFile; }

    static void SetPrintToDisplay(bool aEnable);
    static bool PrintToDisplay() { return Get().mPrintToDisplay; }

protected:
    bool mPrintToDisplay;
    std::ofstream mLcdDisplay;

    static std::string mTtyDeviceFile;
    static bool mUseColors;

    Console();
    void updatePrintToDisplay(const std::string &arTtyDevice, bool aEnable);

    friend class ConsoleStream;
    void write(const std::string &arMsg, TextColor aColor);
};


} /* namespace rsp::application */

#endif // RSP_CORE_LIB_APPLICATION_CONSOLE_H
