/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <application/Console.h>
#include <string>
#include <sstream>
#include <utils/AnsiEscapeCodes.h>

using namespace rsp::utils::AnsiEscapeCodes;

namespace rsp::application {

std::string Console::mTtyDeviceFile = "/dev/tty1";
bool Console::mUseColors = false;

static std::string textColorToEscapeString(TextColor aColor)
{
    std::string result;
    if (!Console::GetUseColors()) {
        return result;
    }

    switch(aColor) {
        default:
        case TextColor::Info:
            result = ec::fg::LightCyan;
            break;

        case TextColor::Input:
            result = ec::fg::LightWhite;
            break;

        case TextColor::InfoHighLight:
            result = ec::fg::Green;
            break;

        case TextColor::Error:
            result = ec::fg::LightRed;
            break;

        case TextColor::Debug:
            result = ec::fg::LightGreen;
            break;
    }

    return result;
}

std::ostream& operator <<(std::ostream &os, TextColor aLogColor)
{
    os << textColorToEscapeString(aLogColor);

    return os;
}


ConsoleStream::ConsoleStream(Console *apConsole, TextColor aColor)
    : mpConsole(apConsole)
{
    *this << textColorToEscapeString(aColor);

    mColor = aColor;
}

ConsoleStream::ConsoleStream(ConsoleStream &&aFrom)
    : std::stringstream(static_cast<std::stringstream&&>(aFrom))
{
    mpConsole = std::move(aFrom.mpConsole);
    mColor = std::move(aFrom.mColor);
}

ConsoleStream::~ConsoleStream()
{
    if (rdbuf()->in_avail() > 0) {
        mpConsole->write(str(), mColor);
    }
}


Console& Console::Get()
{
    static Console instance;

    return instance;
}

Console::Console()
    : mPrintToDisplay(false),
      mLcdDisplay(mTtyDeviceFile)
//      mCoutBuf(this, std::cout, TextColor::Info),
//      mCerrBuf(this, std::cerr, TextColor::Error)
{
    if (mLcdDisplay.is_open()) {
        mLcdDisplay << ec::TputClear << ec::ResetAll << ec::DisableScreenSaver << ec::HideCursor << std::flush;
    }
    else {
        std::cerr << mTtyDeviceFile << " could not be opened" << std::endl;
    }
}

Console::~Console()
{
    std::cout << ec::ConsoleDefault;
    std::cerr << ec::ConsoleDefault;
    if (mPrintToDisplay) {
        mLcdDisplay << ec::ConsoleDefault << std::endl;
    }
}

void Console::write(const std::string &arMsg, TextColor aColor)
{
    if (aColor == TextColor::Error) {
        std::cerr << arMsg << std::flush;
    }
    else {
        std::cout << arMsg << std::flush;
    }

    if (mPrintToDisplay) {
        mLcdDisplay << arMsg;
        mLcdDisplay.flush();
    }
}

//void Console::writeToDisplay(const std::string &arMsg)
//{
//    if (mPrintToDisplay) {
//        mDisplay << arMsg;
//        mDisplay.flush();
//    }
//}

void ConsoleLogStreams::Error(const std::string &arMsg)
{
    Console::Error() << arMsg;
}

void ConsoleLogStreams::Info(const std::string &arMsg)
{
    Console::Info() << arMsg;
}


} /* namespace rsp::application */

