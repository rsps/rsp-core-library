/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_DATETIME_H_
#define INCLUDE_UTILS_DATETIME_H_

#include <cstdint>
#include <chrono>
#include <filesystem>

namespace rsp::utils {

/**
 * \class DateTime
 * \brief A generic date time class that internally holds a Julian date and the nanoseconds count of the day.
 */
class DateTime
{
public:
    struct Format
    {
        static const char* cRFC3339 = "%FT%TZ";
        static const char* cRFC3339Milli = "%FT%T.";
        static const char* cISO8601 = "%F %T%z";
        static const char* cISO8601UTC = "%F %T";
        static const char* cLogging = "%F %T.";
        static const char* cHTTP = "%a, %d %b %Y %H:%M:%S";
    };

    DateTime() {};
    DateTime(std::chrono::duration aDuration);
    DateTime(std::chrono::time_point aTimePoint);
    DateTime(std::filesystem::file_time_type);
    DateTime(const std::string &arTimeString, const std::string &arFormat = "");
    virtual ~DateTime() {};
    DateTime(const DateTime &other) = default;
    DateTime(DateTime &&other) = default;
    DateTime& operator=(const DateTime &other) = default;
    DateTime& operator=(DateTime &&other) = default;

protected:
    std::int64_t mJulianDays = 0;
    std::int64_t mNanoSecondsOfDay = 0;
};

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_DATETIME_H_ */
