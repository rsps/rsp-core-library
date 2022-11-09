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

    std::tm tbuf;

//    std::cout << "TimeStamp is: " << asctime(gmtime_r(&t, &tbuf)) << std::endl;;

    std::stringstream ss;

//    2006-01-02T15:04:05.999999Z07:00

    switch (aFormat) {
        case TimeFormats::Logging:
            ss << std::put_time(gmtime_r(&t, &tbuf), "%F %T.") << std::setfill('0') << std::setw(3) << msecs.count();
            break;

        case TimeFormats::RFC3339:
            ss << std::put_time(gmtime_r(&t, &tbuf), "%FT%T");
            break;

        case TimeFormats::RFC3339Milli:
            ss << std::put_time(gmtime_r(&t, &tbuf), "%FT%T.") << std::setfill('0') << std::setw(3) << msecs.count() << "Z";
            break;

        case TimeFormats::HTTP:
            ss << std::put_time(gmtime_r(&t, &tbuf), "%a, %d %b %Y %H:%M:%S %Z"); // << " GMT";
            break;

        default:
            ss << std::put_time(gmtime_r(&t, &tbuf), "%F %T");
            break;
    }

    return ss.str();
}


std::chrono::system_clock::time_point ToTimePoint(const std::string &arTimeString, TimeFormats aFormat)
{

    std::tm tm = {};
    std::stringstream ss(arTimeString);
    ss.imbue(std::locale("C"));
    unsigned int msecs = 0;

    switch (aFormat) {
        case TimeFormats::Logging:
            ss >> std::get_time(&tm, "%F %T.");
            ss >> msecs;
            break;

        case TimeFormats::RFC3339:
            ss >> std::get_time(&tm, "%FT%T");
            break;

        case TimeFormats::RFC3339Milli:
            ss >> std::get_time(&tm, "%FT%T.");
            ss >> msecs;
            break;

        case TimeFormats::HTTP:
            ss >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S %Z");
            break;

        default:
            ss >> std::get_time(&tm, "%F %T");
            break;
    }

    // Find offset to system time at the specific time.
    // Then offset the tm object back to GMT.
    std::time_t t = std::mktime(&tm);
    std::tm gm_tm;
    gmtime_r(&t, &gm_tm);
    gm_tm.tm_isdst = false;
    std::time_t gm_t = std::mktime(&gm_tm);
    std::time_t tz_offset = (gm_t - t);
    t -= tz_offset;
    gmtime_r(&t, &tm); // Re-fill tm from timezone corrected time.
//    std::cout << "HTTP: " << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S %Z") << std::endl;

    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    tp += std::chrono::milliseconds(msecs) - std::chrono::seconds(tz_offset); // Subtract timezone offset from time_point.
    return tp;
}

std::string TimeStamp(std::chrono::steady_clock::time_point aTime, TimeFormats aFormat)
{
    return TimeStamp(rsp::utils::ClockCast<std::chrono::system_clock::time_point>(aTime), aFormat);
}

std::string TimeStamp(std::chrono:: milliseconds aMilliSeconds, TimeFormats aFormat)
{
    std::chrono::time_point<std::chrono::system_clock> dur(aMilliSeconds);
    return TimeStamp(dur, aFormat);
}

std::string TimeStamp(std::filesystem::file_time_type aFiletime, TimeFormats aFormat)
{
    using namespace std::chrono;
    auto tp = time_point_cast<system_clock::duration>(aFiletime - std::filesystem::file_time_type::clock::now() + system_clock::now());
    return TimeStamp(tp, aFormat);
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

