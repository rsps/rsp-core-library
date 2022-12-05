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
#include <time.h>

namespace rsp::utils {

constexpr timespec timepointToTimespec(
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp)
{
    using namespace std::chrono;
    auto secs = time_point_cast<seconds>(tp);
    auto ns = time_point_cast<nanoseconds>(tp) -
             time_point_cast<nanoseconds>(secs);

    return timespec{secs.time_since_epoch().count(), ns.count()};
}

/**
 * \class DateTime
 * \brief A class with generic date time helper functions.
 *        The internal time is stored in a system_clock::time_point object,
 *        with its implementation default resolution.
 *        The system_clock does not count leap seconds. If that turns out to be a problem,
 *        convert content to utc_clock.
 */
class DateTime
{
public:
    /**
     * Date alias used in this object
     */
    using Date = std::chrono::year_month_day;
    /**
     * Time alias used in this object.
     */
    using Time = std::chrono::hh_mm_ss<std::chrono::milliseconds>;

    /**
     * \brief Some built-in known formats.
     */
    enum class Formats {
       RFC3339,     /**< RFC3339 */
       RFC3339Milli,/**< RFC3339Milli */
       ISO8601,     /**< ISO8601 */
       ISO8601UTC,  /**< ISO8601UTC */
       Logging,     /**< Logging */
       HTTP         /**< HTTP */
    };

    /**
     * \brief Static factory function.
     * \return DateTime object initialized to current time and date.
     */
    static DateTime Now() { return DateTime(); }

    /**
     * \brief Default constructor, initializes to current time
     */
    DateTime();
    /**
     * \brief Construct from date and time parameters
     * \param aYear
     * \param aMonth
     * \param aDayOfMonth
     * \param aHour
     * \param aMinute
     * \param aSecond
     * \param aMilliSecond
     */
    DateTime(int aYear, unsigned aMonth, unsigned aDayOfMonth, int aHour, int aMinute, int aSecond, int aMilliSecond=0);
    /**
     * \brief Construct from a duration. The time is set to the duration since Unix epoch.
     * \param aDuration
     */
    DateTime(std::chrono::system_clock::duration aDuration);
    /**
     * \brief Construct from af system_clock time_point.
     * \param aTimePoint
     */
    DateTime(std::chrono::system_clock::time_point aTimePoint);
    /**
     * \brief Construct from a file_time_type (file_clock)
     * \param aFileTime
     */
    DateTime(std::filesystem::file_time_type aFileTime);
    /**
     * \brief Construct from the given string, using the given format.
     * \param arTimeString
     * \param apFormat Same format as for std::get_time, if last format character is '.', then fractional seconds are parsed also.
     */
    DateTime(const std::string &arTimeString, const char *apFormat);
    /**
     * \brief Construct from the given string, using a built-in format.
     * \param arTimeString
     * \param aFormat \see DateTime::Formats
     */
    DateTime(const std::string &arTimeString, Formats aFormat);
    /**
     * \brief Construct from seconds since Unix epoch
     * \param aSeconds
     */
    DateTime(std::time_t aSeconds);
    /**
     * \brief Construct from old C-style time struct.
     * \param arTm
     */
    DateTime(std::tm &arTm);
    /**
     * \brief Construct from posix timespec struct
     * \param arTimeSpec
     */
    DateTime(timespec &arTimeSpec);
    /**
     * \brief Destructor
     */
    virtual ~DateTime() {};

    /**
     * \brief Copy and move contructors
     * \param other
     */
    DateTime(const DateTime &other) = default;
    DateTime(DateTime &&other) = default;
    /**
     * \brief Assignment operators
     * \param other
     * \return self
     */
    DateTime& operator=(const DateTime &other) = default;
    DateTime& operator=(DateTime &&other) = default;

    /**
     * \brief Various useful arithmetic operators
     * \param arOther
     * \return DateTime or self
     */
    DateTime operator+(const DateTime &arOther) const;
    DateTime operator-(const DateTime &arOther) const;
    DateTime& operator+=(const DateTime &arOther);
    DateTime& operator-=(const DateTime &arOther);
    DateTime operator+(const std::chrono::system_clock::duration &arDuration) const;
    DateTime operator-(const std::chrono::system_clock::duration &arDuration) const;
    DateTime& operator+=(const std::chrono::system_clock::duration &arDuration);
    DateTime& operator-=(const std::chrono::system_clock::duration &arDuration);
    /**
     * \brief Various useful comparison operators
     * \param arOther
     * \return bool
     */
    bool operator<(const DateTime &arOther) const;
    bool operator>(const DateTime &arOther) const;
    bool operator<=(const DateTime &arOther) const;
    bool operator>=(const DateTime &arOther) const;
    bool operator==(const DateTime &arOther) const;
    bool operator!=(const DateTime &arOther) const;

    /**
     * \brief Get the count of seconds between two DateTime objects.
     * \param arOther
     * \return int64_t
     */
    int64_t SecondsBetween(const DateTime &arOther) const;
    /**
     * \brief Get the count of milliseconds between two DateTime objects.
     * \param arOther
     * \return int64_t
     */
    int64_t MilliSecondsBetween(const DateTime &arOther) const;
    /**
     * \brief Get the count of microseconds between two DateTime objects.
     * \param arOther
     * \return int64_t
     */
    int64_t MicroSecondsBetween(const DateTime &arOther) const;
    /**
     * \brief Get the count of nanoseconds between two DateTime objects.
     * \param arOther
     * \return int64_t
     */
    int64_t NanoSecondsBetween(const DateTime &arOther) const;

    /**
     * \brief Implicit cast operators
     */
    operator std::chrono::system_clock::duration() const;
    operator std::chrono::system_clock::time_point() const;
    operator std::filesystem::file_time_type() const;
    operator std::time_t() const;
    operator std::tm() const;

    timespec GetTimeSpec() const;

    /**
     * \brief Format the timestamp to a string using the given format.
     * \see std::put_time for format. Extension: If final character is a '.', a 3 digit millisecond part is appended.
     * \param apFormat
     * \return string
     */
    std::string ToString(const char *apFormat) const;
    /**
     * \brief Format timestamp to one of the known formats. \see DateTime::Formats
     * \param aFormat
     * \return string
     */
    std::string ToString(Formats aFormat) const;
    std::string ToRFC3339() const;
    std::string ToRFC3339Milli() const;
    std::string ToISO8601() const;
    std::string ToISO8601UTC() const;
    std::string ToLogging() const;
    std::string ToHTTP() const;

    /**
     * \brief Decode the given string, using the given format. \see std::get_time for supported format.
     * \param arTimeString
     * \param apFormat
     * \return self
     */
    DateTime& FromString(const std::string &arTimeString, const char *apFormat);

    /**
     * \brief Get the Date part of this object.
     * \return Date
     */
    Date GetDate() const;
    /**
     * \brief Get the time par of this object.
     * \return Time with millisecond resolution.
     */
    Time GetTime() const;

protected:
    std::chrono::system_clock::time_point mTp{};

    std::chrono::system_clock::duration decodeFractions(uint64_t aFractions) const;
    std::ostream& encodeFractions(std::ostream& os, std::chrono::system_clock::time_point aTp) const;

private:
    std::chrono::seconds getTimezoneOffset(std::tm &arTm) const;
};

/**
 * \brief Various streaming operators for the DateTime object.
 * \param os output stream
 * \param Date/Time/DateTime
 * \return output stream
 */
std::ostream& operator<< (std::ostream& os, const DateTime::Date &arDate);
std::ostream& operator<< (std::ostream& os, const DateTime::Time &arTime);
std::ostream& operator<< (std::ostream& os, const DateTime &arDateTime);

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_DATETIME_H_ */
