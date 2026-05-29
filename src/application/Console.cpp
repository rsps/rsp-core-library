/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <rsp/application/Console.h>
#include <iostream>
#include <string>
#include <rsp/json/Json.h>
#include <rsp/utils/AnsiEscapeCodes.h>

using namespace rsp::utils::AnsiEscapeCodes;

namespace rsp::application {

std::string Console::mTtyDeviceFile = "/dev/tty1";
bool Console::mUseColors = false;

static std::string textColorToEscapeString(const TextColor aColor)
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

std::ostream& operator <<(std::ostream &os, const TextColor aLogColor)
{
    os << textColorToEscapeString(aLogColor);
    return os;
}


ConsoleStream::ConsoleStream(Console *apConsole, const TextColor aColor)
    : mpConsole(apConsole),
      mColor(aColor)
{
    mBuffer << textColorToEscapeString(aColor);
}

ConsoleStream::ConsoleStream(ConsoleStream &&arOther) noexcept
    : mpConsole(arOther.mpConsole),
      mColor(arOther.mColor),
      mBuffer(std::move(arOther.mBuffer))
{
}

ConsoleStream::~ConsoleStream()
{
    if (mBuffer.rdbuf()->in_avail() > 0) {
        mpConsole->write(mBuffer.view(), mColor);
    }
}

ConsoleStream& ConsoleStream::operator=(ConsoleStream&& arOther) noexcept
{
    if (this != &arOther) {
        mpConsole = arOther.mpConsole;
        mColor = arOther.mColor;
        mBuffer = std::move(arOther.mBuffer);
    }
    return *this;
}

ConsoleStream& ConsoleStream::operator<<(std::ostream& (* apFunc)(std::ostream&))
{
    mBuffer << apFunc;
    return *this;
}


Console& Console::Get()
{
    static Console instance;

    return instance;
}

Console::Console()
    : mPrintToDisplay(false),
      mLcdDisplay()
{
}

Console::~Console()
{
    std::cout << ec::ConsoleDefault;
    std::cerr << ec::ConsoleDefault;
    if (mPrintToDisplay) {
        mLcdDisplay << ec::ConsoleDefault << std::endl;
    }
}

void Console::write(const std::string_view aMsg, const TextColor aColor)
{
    if (aColor == TextColor::Error) {
        std::cerr << aMsg << std::flush;
    }
    else {
        std::cout << aMsg << std::flush;
    }

    if (mPrintToDisplay) {
        mLcdDisplay << aMsg;
        mLcdDisplay.flush();
    }
}

void Console::SetTtyDevice(const std::string &arTtyDevice)
{
    Get().updatePrintToDisplay(arTtyDevice, Get().mPrintToDisplay);
}

void Console::SetPrintToDisplay(const bool aEnable)
{
    Get().updatePrintToDisplay(mTtyDeviceFile, aEnable);
}

void Console::updatePrintToDisplay(const std::string &arTtyDevice, const bool aEnable)
{
    mTtyDeviceFile = arTtyDevice;
    mPrintToDisplay = aEnable;

    if (mPrintToDisplay && !mTtyDeviceFile.empty()) {
        mLcdDisplay.open(mTtyDeviceFile);
        if (mLcdDisplay.is_open()) {
            mLcdDisplay << ec::TPutClear << ec::ResetAll << ec::DisableScreenSaver << ec::HideCursor << std::flush;
        }
        else {
            std::cerr << mTtyDeviceFile << " could not be opened" << std::endl;
        }
    }
    else if (mLcdDisplay.is_open()) {
        mLcdDisplay.close();
    }
}

void Console::Write(std::string_view aMsg, logging::LogLevel aCurrentLevel, const std::string& arChannel, const utils::DynamicData& arContext, const std::string& arColor)
{
    ConsoleStream out(this, (aCurrentLevel < logging::LogLevel::Warning) ? TextColor::Error : TextColor::Info);

    if (!arColor.empty()) {
        out << arColor;
    }
    if (!arChannel.empty()) {
        out << arChannel << ": ";
    }
    out << aMsg;
    if (!arContext.IsNull()) {
        out << " " << rsp::json::JsonEncoder().Encode(arContext);
    }
    if (!arColor.empty()) {
        out << std::string(utils::AnsiEscapeCodes::ec::ConsoleDefault);
    }
    out << std::endl;
}

} /* namespace rsp::application */
