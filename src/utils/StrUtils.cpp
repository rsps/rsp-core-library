/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <ctime>
#include <chrono>
#include <algorithm>
#include <locale>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstdarg>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <utils/ClockCast.h>
#include <utils/StrUtils.h>

namespace rsp::utils::StrUtils {

size_t Split(const std::string &arTxt, std::vector<std::string> &aList, char aDelimiter, bool aKeepEmpty)
{
    size_t pos = arTxt.find( aDelimiter );
    size_t initialPos = 0;
    aList.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        std::string s = arTxt.substr( initialPos, pos - initialPos );
        if (aKeepEmpty || (s.length() > 0)) {
            aList.push_back( s );
        }
        initialPos = pos + 1;

        pos = arTxt.find( aDelimiter, initialPos );
    }

    // Add the last one
    if (arTxt.length() > initialPos) {
        std::string s = arTxt.substr( initialPos, std::min( pos, arTxt.size() ) - initialPos + 1 );
        aList.push_back( s );
    }

    return aList.size();
}

std::string TrimCopy(std::string const& aStr)
{
   auto s = aStr;
   return LeftTrim(RightTrim(s));
}

std::string& ToLower(std::string &arStr)
{
    std::transform(arStr.begin(), arStr.end(), arStr.begin(),
        [](int c) { return std::tolower(c); });
    return arStr;
}

std::string& ToUpper(std::string &arStr)
{
    std::transform(arStr.begin(), arStr.end(), arStr.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return arStr;
}


std::string GetHomeDir()
{
    const char *homedir;

    if ((homedir = getenv("HOME")) == nullptr) {
        struct passwd *pwd = getpwuid(getuid());
        homedir = pwd->pw_dir;
    }

    return homedir;
}

std::string GetConfigDir()
{
    const char *configdir;

    if ((configdir = getenv("XDG_CONFIG_HOME")) != nullptr) {
        return configdir;
    }

    return GetHomeDir();
}

bool StartsWith(const std::string &aText, const std::string &aPrefix)
{
    return (aPrefix == aText.substr(0, aPrefix.length()));
}

bool EndsWith(const std::string &aText, const std::string &aAffix)
{
    return (aAffix == aText.substr(aText.length() - aAffix.length(), aAffix.length()));
}

bool Contains(const std::string &aText, const std::string &aMatch)
{
    return (aText.find(aMatch, 0) != aText.npos);
}


std::vector<std::string> FindMatches(std::string aText, std::vector<std::string> &arList)
{
    std::vector<std::string> found;
    for (std::string &s : arList) {
        if(aText == s.substr(0, aText.length())) {
            found.push_back(s);
        }
    }

    return found;
}

std::string ReduceToCommon(std::vector<std::string> &arList)
{
    std::string match;

    for (std::string &s : arList) {
        if (match.empty()) {
            match = s;
        }
        else {
            while (match != s.substr(0, match.length())) {
                match.pop_back();
                if (match.empty()) {
                    break;
                }
            }
        }

        if (match.empty()) {
            break;
        }
    }

    return match;
}

std::string& ReplaceAll(std::string &arText, const std::string& aFrom, const std::string& aTo)
{
    size_t start_pos = 0;
    while((start_pos = arText.find(aFrom, start_pos)) != std::string::npos) {
        arText.replace(start_pos, aFrom.length(), aTo);
        start_pos += aTo.length(); // Handles case where 'to' is a substring of 'from'
    }

    return arText;
}


//__attribute__((__format__(__printf__, 1, 0)))
std::string Format(const char* apFormat, ...)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    // Pass on the varargs on to 'vfprintf'.
    va_list arglist;

    va_start(arglist, apFormat);
    std::size_t size = static_cast<std::size_t>(vsnprintf(nullptr, 0, apFormat, arglist)) + 1; // Extra space for '\0'
    if (size > 255) {
        errno = ENOMEM;
        THROW_SYSTEM("StrUtils::format buffer to small");
    }
    char buffer[256];
    va_start(arglist, apFormat); // Reset arglist ptr.
    vsnprintf(buffer, 255, apFormat, arglist);

    va_end(arglist);
#pragma GCC diagnostic pop
    return std::string(buffer, size - 1); // We don't want the '\0' inside
}

} /* namespace StrUtils */

