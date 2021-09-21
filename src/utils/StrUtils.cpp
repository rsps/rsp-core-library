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
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <utils/StrUtils.h>
#include <utils/clock_cast.h>

namespace rsp::StrUtils {

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

std::string& Trim(std::string &aStr)
{
    return LeftTrim(RightTrim(aStr));
}

std::string& LeftTrim(std::string &aStr)
{
    auto it2 = std::find_if(aStr.begin(), aStr.end(),
        [](char ch) {
            return !std::isspace<char>(ch , std::locale::classic() );
        }
    );

    aStr.erase(aStr.begin(), it2);
    return aStr;
}

std::string& RightTrim(std::string &aStr)
{
    auto it1 = std::find_if(aStr.rbegin(), aStr.rend(),
        [](char ch) {
            return !std::isspace<char>(ch , std::locale::classic() );
        }
    );
    aStr.erase(it1.base(), aStr.end());

    return aStr;
}

std::string TrimCopy(std::string const& aStr)
{
   auto s = aStr;
   return LeftTrim(RightTrim(s));
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
    for (auto s : arList) {
        if(aText == s.substr(0, aText.length())) {
            found.push_back(s);
        }
    }

    return found;
}

std::string ReduceToCommon(std::vector<std::string> &arList)
{
    std::string match;

    for (auto &s : arList) {
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

std::string& ReplaceAll(std::string &aText, const std::string& aFrom, const std::string& aTo)
{
    size_t start_pos = 0;
    while((start_pos = aText.find(aFrom, start_pos)) != std::string::npos) {
        aText.replace(start_pos, aFrom.length(), aTo);
        start_pos += aTo.length(); // Handles case where 'to' is a substring of 'from'
    }

    return aText;
}

std::string TimeStamp()
{
    return TimeStamp(std::chrono::steady_clock::now(), TimeFormats::Logging);
}

std::string TimeStamp(TimeFormats aFormat)
{
    return TimeStamp(std::chrono::steady_clock::now(), aFormat);
}

std::string TimeStamp(std::chrono::system_clock::time_point aTime, TimeFormats aFormat)
{
    auto tse = aTime.time_since_epoch();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tse);
    auto now_s = std::chrono::duration_cast<std::chrono::seconds>(tse);
    auto msecs = now_ms - now_s;
    auto t = std::chrono::system_clock::to_time_t(aTime);

    std::stringstream ss;

//    2006-01-02T15:04:05.999999Z07:00

    switch (aFormat) {
        case TimeFormats::Logging:
            ss << std::put_time(std::gmtime(&t), "%F %T.") << std::setfill('0') << std::setw(3) << msecs.count();
            break;

        case TimeFormats::RFC3339:
            ss << std::put_time(std::gmtime(&t), "%FT%T");
            break;

        case TimeFormats::RFC3339Milli:
            ss << std::put_time(std::gmtime(&t), "%FT%T.") << std::setfill('0') << std::setw(3) << msecs.count() << "Z";
            break;

        default:
            ss << std::put_time(std::gmtime(&t), "%F %T");
            break;
    }

    return ss.str();
}

std::string TimeStamp(std::chrono::steady_clock::time_point aTime, TimeFormats aFormat)
{
    return TimeStamp(rsp::clock_cast<std::chrono::system_clock::time_point>(aTime), aFormat);
}

std::string TimeStamp(std::chrono:: milliseconds aMilliSeconds, TimeFormats aFormat)
{
    std::chrono::time_point<std::chrono::system_clock> dur(aMilliSeconds);
    return TimeStamp(dur, aFormat);
}



} /* namespace StrUtils */

