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
 * \brief A generic date time class that internally holds a Julian date and the nanoseconds count of the day.
 *
 * The Julian date calculation is using the conversion algorithms by Henry F. Fliegel and Thomas C. Van Flandern.
 * Thus Julian day 0 corresponds to -4713-11-24 Gregorian Calendar, which is -4712-01-01 Julian Calendar
 * \see https://www.hermetic.ch/cal_stud/jdn.htm
 */
class DateTime
{
public:
    static constexpr int64_t cNanoSecondsPerSecond = (1000000000L);
    static constexpr int64_t cMicroSecondsPerSecond = (1000000L);
    static constexpr int64_t cMilliSecondsPerSecond = (1000L);
    static constexpr int64_t cNanoSecondsPerMinute = (60*cNanoSecondsPerSecond);
    static constexpr int64_t cNanoSecondsPerHour = (60*60*cNanoSecondsPerSecond);
    static constexpr int64_t cNanoSecondsPerDay = (24*60*60*cNanoSecondsPerSecond);
    static constexpr int64_t cSecondsPerDay = (24*60*60);
    static constexpr int64_t cMilliSecondsPerDay = (24*60*60*cMilliSecondsPerSecond);
    static constexpr int64_t cMicroSecondsPerDay = (24*60*60*cMicroSecondsPerSecond);

    struct Date {
        int16_t Year = 0;
        uint8_t Month = 1; // 1-12
        uint8_t DayOfMonth = 1; // 1-31

        Date() {}
        Date(int16_t aYear, uint8_t aMonth, uint8_t aDay) : Year(aYear), Month(aMonth), DayOfMonth(aDay) {}
        Date(int64_t aJulianDays);
        Date(const Date &arOther) = default;
        Date(Date &&arOther) = default;
        Date& operator=(const Date &arOther) = default;
        Date& operator=(Date &&arOther) = default;
        operator int64_t() const { return ToJulianDays(); }
        int64_t ToJulianDays() const;
    };
    struct Time {
        uint8_t Hours = 0; // 0-23
        uint8_t Minutes = 0; // 0-59
        uint8_t Seconds = 0; // 0-60, Allows for one leap second

        Time() {}
        Time(uint8_t aHours, uint8_t aMinutes, uint8_t aSeconds) : Hours(aHours), Minutes(aMinutes), Seconds(aSeconds) {}
        Time(int64_t aNanoSeconds);
        Time(const Time &arOther) = default;
        Time(Time &&arOther) = default;
        Time& operator=(const Time &arOther) = default;
        Time& operator=(Time &&arOther) = default;
        operator int64_t() const { return ToNanoSeconds(); }
        int64_t ToNanoSeconds() const;
    };
    enum class Formats {
       RFC3339,
       RFC3339Milli,
       ISO8601,
       ISO8601UTC,
       Logging,
       HTTP
    };

    DateTime() {};
    DateTime(int aYear, int aMonth, int aDayOfMonth, int aHour, int aMinute, int aSecond);
    DateTime(std::chrono::system_clock::duration aDuration);
    DateTime(std::chrono::system_clock::time_point aTimePoint);
    DateTime(std::filesystem::file_time_type aFileTime);
    DateTime(const std::string &arTimeString, const char *apFormat);
    DateTime(const std::string &arTimeString, Formats aFormat);
    DateTime(int64_t aDays, int64_t aNanoSeconds);
    DateTime(int64_t aSeconds);
    virtual ~DateTime() {};

    DateTime(const DateTime &other) = default;
    DateTime(DateTime &&other) = default;
    DateTime& operator=(const DateTime &other) = default;
    DateTime& operator=(DateTime &&other) = default;

    DateTime operator+(const DateTime &arOther) const;
    DateTime operator-(const DateTime &arOther) const;
    DateTime& operator+=(const DateTime &arOther);
    DateTime& operator-=(const DateTime &arOther);

    bool operator==(const DateTime &arOther) const;
    bool operator<(const DateTime &arOther) const;
    bool operator<=(const DateTime &arOther) const;
    bool operator>(const DateTime &arOther) const;
    bool operator>=(const DateTime &arOther) const;

    int64_t SecondsBetween(const DateTime &arOther) const;
    int64_t MilliSecondsBetween(const DateTime &arOther) const;
    int64_t MicroSecondsBetween(const DateTime &arOther) const;
    int64_t NanoSecondsBetween(const DateTime &arOther) const;

    operator std::chrono::system_clock::duration() const;
    operator std::chrono::system_clock::time_point() const;
    operator std::filesystem::file_time_type() const;
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
    std::int64_t mJulianDays = 0;
    std::int64_t mNanoSecondsOfDay = 0;

    std::chrono::seconds getTimezoneOffset(std::tm &arTm) const;
};

std::ostream& operator<< (std::ostream& os, const DateTime::Date &arDate);
std::ostream& operator<< (std::ostream& os, const DateTime::Time &arTime);
std::ostream& operator<< (std::ostream& os, const DateTime &arDateTime);

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_DATETIME_H_ */
