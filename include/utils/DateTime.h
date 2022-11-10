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
#include <ostream>
#include <filesystem>

namespace rsp::utils {

/**
 * \class DateTime
 * \brief A class with generic date time helper functions.
 */
class DateTime
{
public:
    using Date = std::chrono::year_month_day;
    using Time = std::chrono::hh_mm_ss<std::chrono::milliseconds>;

    enum class Formats {
       RFC3339,
       RFC3339Milli,
       ISO8601,
       ISO8601UTC,
       Logging,
       HTTP
    };

    DateTime() {};
    DateTime(int aYear, int aMonth, int aDayOfMonth, int aHour, int aMinute, int aSecond, int aMilliSecond=0);
    DateTime(std::chrono::system_clock::duration aDuration);
    DateTime(std::chrono::system_clock::time_point aTimePoint);
    DateTime(std::filesystem::file_time_type aFileTime);
    DateTime(const std::string &arTimeString, const char *apFormat);
    DateTime(const std::string &arTimeString, Formats aFormat);
    DateTime(std::time_t aSeconds);
    DateTime(std::tm &arTm);
    virtual ~DateTime() {};

    DateTime(const DateTime &other) = default;
    DateTime(DateTime &&other) = default;
    DateTime& operator=(const DateTime &other) = default;
    DateTime& operator=(DateTime &&other) = default;

    int64_t SecondsBetween(const DateTime &arOther) const;
    int64_t MilliSecondsBetween(const DateTime &arOther) const;
    int64_t MicroSecondsBetween(const DateTime &arOther) const;
    int64_t NanoSecondsBetween(const DateTime &arOther) const;

    operator std::chrono::system_clock::duration() const;
    operator std::chrono::system_clock::time_point() const;
    operator std::filesystem::file_time_type() const;
    operator std::time_t() const;
    operator std::tm() const;

    std::string ToString(const char *apFormat) const;
    std::string ToString(Formats aFormat) const;
    std::string ToRFC3339() const;
    std::string ToRFC3339Milli() const;
    std::string ToISO8601() const;
    std::string ToISO8601UTC() const;
    std::string ToLogging() const;
    std::string ToHTTP() const;

    DateTime& FromString(const std::string &arTimeString, const char *apFormat);

    Date GetDate() const;
    Time GetTime() const;

protected:
    std::chrono::system_clock::time_point mTp{};

private:
    std::chrono::seconds getTimezoneOffset(std::tm &arTm) const;
};

std::ostream& operator<< (std::ostream& os, const DateTime::Date &arDate);
std::ostream& operator<< (std::ostream& os, const DateTime::Time &arTime);
std::ostream& operator<< (std::ostream& os, const DateTime &arDateTime);

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_DATETIME_H_ */
