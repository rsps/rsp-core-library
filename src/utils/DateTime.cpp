/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <sstream>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <utils/DateTime.h>

using namespace std::literals::chrono_literals;

namespace rsp::utils {

DateTime::DateTime(int aYear, int aMonth, int aDayOfMonth, int aHour, int aMinute, int aSecond)
{
    Date dt(aYear, aMonth, aDayOfMonth);
    Time tm(aHour, aMinute, aSecond);
    mJulianDays = dt;
    mNanoSecondsOfDay = tm;
}

DateTime::DateTime(std::chrono::system_clock::duration aDuration)
{
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(aDuration);
    mJulianDays = (ns.count() / cNanoSecondsPerDay);
    mNanoSecondsOfDay = (ns.count() % cNanoSecondsPerDay);
}

DateTime::DateTime(std::chrono::system_clock::time_point aTimePoint)
{
    auto tp = std::chrono::time_point_cast<std::chrono::nanoseconds>(aTimePoint);
    mJulianDays = (tp.time_since_epoch().count() / cNanoSecondsPerDay);
    mNanoSecondsOfDay = (tp.time_since_epoch().count() % cNanoSecondsPerDay);
}

DateTime::DateTime(std::filesystem::file_time_type aFileTime)
    : DateTime(std::chrono::file_clock::to_sys(aFileTime))
{
    std::chrono::year y{2022y};

}

DateTime::DateTime(const std::string &arTimeString, const char *apFormat)
{
    FromString(arTimeString, apFormat);
}

DateTime::DateTime(const std::string &arTimeString, Formats aFormat)
{
    switch (aFormat) {
        case Formats::RFC3339:
            FromString(arTimeString, "%FT%T");
            break;

        case Formats::RFC3339Milli:
            FromString(arTimeString, "%FT%T.");
            break;

        case Formats::ISO8601:
            FromString(arTimeString, "%F %T%z");
            break;

        case Formats::ISO8601UTC:
            FromString(arTimeString, "%F %T");
            break;

        case Formats::Logging:
            FromString(arTimeString, "%F %T.");
            break;

        case Formats::HTTP:
            FromString(arTimeString, "%a, %d %b %Y %H:%M:%S");
            break;

        default:
            break;
    }
}

DateTime::DateTime(int64_t aDays, int64_t aNanoSeconds)
{
    mJulianDays = aDays;
    mNanoSecondsOfDay = aNanoSeconds;
    if (mNanoSecondsOfDay > cNanoSecondsPerDay) {
        mJulianDays += (mNanoSecondsOfDay / cNanoSecondsPerDay);
        mNanoSecondsOfDay %= cNanoSecondsPerDay;
    }
}

DateTime::DateTime(int64_t aSeconds)
{
    mJulianDays = (aSeconds * cNanoSecondsPerSecond) / cNanoSecondsPerDay;
    mNanoSecondsOfDay = (aSeconds * cNanoSecondsPerSecond) % cNanoSecondsPerDay;
}

bool DateTime::operator ==(const DateTime &arOther) const
{
    return (mJulianDays == arOther.mJulianDays) && (mNanoSecondsOfDay == arOther.mNanoSecondsOfDay);
}

bool DateTime::operator <(const DateTime &arOther) const
{
    if (mJulianDays < arOther.mJulianDays) {
        return true;
    }
    else if ((mJulianDays == arOther.mJulianDays) && (mNanoSecondsOfDay < arOther.mNanoSecondsOfDay)) {
        return true;
    }
    return false;
}

bool DateTime::operator <=(const DateTime &arOther) const
{
    if (mJulianDays < arOther.mJulianDays) {
        return true;
    }
    else if ((mJulianDays == arOther.mJulianDays) && (mNanoSecondsOfDay <= arOther.mNanoSecondsOfDay)) {
        return true;
    }
    return false;
}

bool DateTime::operator >(const DateTime &arOther) const
{
    if (mJulianDays > arOther.mJulianDays) {
        return true;
    }
    else if ((mJulianDays == arOther.mJulianDays) && (mNanoSecondsOfDay > arOther.mNanoSecondsOfDay)) {
        return true;
    }
    return false;
}

bool DateTime::operator >=(const DateTime &arOther) const
{
    if (mJulianDays > arOther.mJulianDays) {
        return true;
    }
    else if ((mJulianDays == arOther.mJulianDays) && (mNanoSecondsOfDay >= arOther.mNanoSecondsOfDay)) {
        return true;
    }
    return false;
}

DateTime DateTime::operator +(const DateTime &arOther) const
{
    DateTime result = *this;
    result += arOther;
    return result;
}

DateTime DateTime::operator -(const DateTime &arOther) const
{
    DateTime result = *this;
    result -= arOther;
    return result;
}

DateTime& DateTime::operator +=(const DateTime &arOther)
{
    mJulianDays += arOther.mJulianDays;
    mNanoSecondsOfDay += arOther.mNanoSecondsOfDay;
    if (mNanoSecondsOfDay > cNanoSecondsPerDay) {
        mJulianDays += (mNanoSecondsOfDay / cNanoSecondsPerDay);
        mNanoSecondsOfDay %= cNanoSecondsPerDay;
    }
    return *this;
}

DateTime& DateTime::operator -=(const DateTime &arOther)
{
    mJulianDays -= arOther.mJulianDays;
    mNanoSecondsOfDay -= arOther.mNanoSecondsOfDay;
    if (mNanoSecondsOfDay < 0) {
        mJulianDays += (mNanoSecondsOfDay / cNanoSecondsPerDay);
        mNanoSecondsOfDay = -1;
        mNanoSecondsOfDay %= cNanoSecondsPerDay;
    }
    return *this;
}

int64_t DateTime::SecondsBetween(const DateTime &arOther) const
{
    int64_t result = (mJulianDays - arOther.mJulianDays) * cSecondsPerDay;
    result += (mNanoSecondsOfDay - arOther.mNanoSecondsOfDay) / cNanoSecondsPerSecond;
    return result;
}

int64_t DateTime::MilliSecondsBetween(const DateTime &arOther) const
{
    int64_t result = (mJulianDays - arOther.mJulianDays) * cMilliSecondsPerDay;
    result += (mNanoSecondsOfDay - arOther.mNanoSecondsOfDay) / (cNanoSecondsPerSecond * cMilliSecondsPerSecond);
    return result;
}

int64_t DateTime::MicroSecondsBetween(const DateTime &arOther) const
{
    int64_t result = (mJulianDays - arOther.mJulianDays) * cMicroSecondsPerDay;
    result += (mNanoSecondsOfDay - arOther.mNanoSecondsOfDay) / (cNanoSecondsPerSecond * cMicroSecondsPerSecond);
    return result;
}

int64_t DateTime::NanoSecondsBetween(const DateTime &arOther) const
{
    int64_t result = (mJulianDays - arOther.mJulianDays) * cNanoSecondsPerDay;
    result += (mNanoSecondsOfDay - arOther.mNanoSecondsOfDay);
    return result;
}

DateTime::operator std::chrono::system_clock::duration() const
{
    return std::chrono::nanoseconds((mJulianDays * cNanoSecondsPerDay) + mNanoSecondsOfDay);
}

DateTime::operator std::chrono::system_clock::time_point() const
{
    std::chrono::system_clock::time_point dt{std::chrono::nanoseconds((mJulianDays * cNanoSecondsPerDay) + mNanoSecondsOfDay)};
    return dt;
}

DateTime::operator std::filesystem::file_time_type() const
{
    std::chrono::system_clock::time_point sctp{std::chrono::nanoseconds((mJulianDays * cNanoSecondsPerDay) + mNanoSecondsOfDay)};
    return std::chrono::file_clock::from_sys(sctp);
}

DateTime::operator std::tm() const
{
    auto d = GetDate();
    auto t = GetTime();
    std::tm tm{};
    tm.tm_sec = t.Seconds;
    tm.tm_min = t.Minutes;
    tm.tm_hour = t.Hours;
    tm.tm_mday = d.DayOfMonth;
    tm.tm_mon = d.Month - 1;
    tm.tm_year = d.Year - 1900;
    return tm;
}

std::string DateTime::ToString(const char *apFormat) const
{
    std::tm tm = *this;
    std::stringstream ss;
    ss << std::put_time(&tm, apFormat);

    if (std::string(apFormat).back() == '.') {
        unsigned int msecs = (mNanoSecondsOfDay % cNanoSecondsPerSecond) / (cNanoSecondsPerSecond / cMilliSecondsPerSecond);
        ss << std::setfill('0') << std::setw(3) << msecs;
    }

    return ss.str();
}

std::string DateTime::ToString(Formats aFormat) const
{
    switch (aFormat) {
        case Formats::RFC3339:
            return ToRFC3339();
        case Formats::RFC3339Milli:
            return ToRFC3339Milli();
        case Formats::ISO8601:
            return ToISO8601();
        case Formats::ISO8601UTC:
            return ToISO8601UTC();
        case Formats::Logging:
            return ToLogging();
        case Formats::HTTP:
            return ToHTTP();
        default:
            break;
    }
    return ToISO8601UTC();
}

std::string DateTime::ToRFC3339() const
{
    return ToString("%FT%TZ");
}

std::string DateTime::ToRFC3339Milli() const
{
    return ToString("%FT%T.");
}

std::string DateTime::ToISO8601() const
{
    return ToString("%F %T%z");
}

std::string DateTime::ToISO8601UTC() const
{
    return ToString("%F %T");
}

std::string DateTime::ToLogging() const
{
    return ToString("%F %T.");
}

std::string DateTime::ToHTTP() const
{
    return ToString("%a, %d %b %Y %H:%M:%S");
}

DateTime& DateTime::FromString(const std::string &arTimeString, const char *apFormat)
{
    std::tm tm = {};
    unsigned int msecs = 0;
    std::stringstream ss(arTimeString);
    ss >> std::get_time(&tm, apFormat);
    tm.tm_isdst = 0;
    if (std::string(apFormat).back() == '.') {
        ss >> msecs;
    }

    auto sctp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    sctp += std::chrono::milliseconds(msecs) - getTimezoneOffset(tm);

    auto tp = std::chrono::time_point_cast<std::chrono::nanoseconds>(sctp);
    mJulianDays = (tp.time_since_epoch().count() / cNanoSecondsPerDay);
    mNanoSecondsOfDay = (tp.time_since_epoch().count() % cNanoSecondsPerDay);

    return *this;
}

std::chrono::seconds DateTime::getTimezoneOffset(std::tm &arTm) const
{
    std::time_t t = std::mktime(&arTm);
    std::tm gm_tm;
    gmtime_r(&t, &gm_tm);
    gm_tm.tm_isdst = false;
    std::time_t tz_offset = (std::mktime(&gm_tm) - t);

    return std::chrono::seconds(tz_offset);
}

DateTime::Date DateTime::GetDate() const
{
    return Date(mJulianDays);
}

DateTime::Time DateTime::GetTime() const
{
    return Time(mNanoSecondsOfDay);
}

DateTime::Date::Date(int64_t aJulianDays)
{
    auto l = aJulianDays + 68569;
    auto n = ( 4 * l ) / 146097;
    l = l - ( 146097 * n + 3 ) / 4;
    auto i = ( 4000 * ( l + 1 ) ) / 1461001;
    l = l - ( 1461 * i ) / 4 + 31;
    auto j = ( 80 * l ) / 2447;
    auto d = l - ( 2447 * j ) / 80;
    l = j / 11;
    auto m = j + 2 - ( 12 * l );
    auto y = 100 * ( n - 49 ) + i + l;

    Year = y;
    Month = m;
    DayOfMonth = d;
}

int64_t DateTime::Date::ToJulianDays() const
{
    auto m = (Month - 14) / 12;
    auto jd = (1461 * (Year + 4800 + m)) / 4
        + (367 * (Month - 2 - 12 * m)) / 12
        - (3 * ((Year + 4900 + m) / 100)) / 4
        + DayOfMonth - 32075;
/*
    uint32_t a  = (14 - static_cast<uint32_t>(Month)) / 12;
    uint32_t y1 = static_cast<uint32_t>(Year) + 4800 - a;
    uint32_t m1 = static_cast<uint32_t>(Month) + (12 * a) - 3;
    int64_t jd = static_cast<int64_t>(DayOfMonth) + (153 * m1 + 2) / 5 + y1 * 365 + y1 / 4 - y1 / 100 + y1 / 400 - 32045;
*/
    return jd;
}

DateTime::Time::Time(int64_t aNanoSeconds)
{
    Hours = aNanoSeconds / cNanoSecondsPerHour;
    Minutes = (aNanoSeconds % cNanoSecondsPerHour) / cNanoSecondsPerMinute;
    Seconds = (aNanoSeconds % cNanoSecondsPerMinute) / cNanoSecondsPerSecond;
}

int64_t DateTime::Time::ToNanoSeconds() const
{
    return (Hours * cNanoSecondsPerHour) + (Minutes * cNanoSecondsPerMinute) + (Seconds * cNanoSecondsPerSecond);
}

std::ostream& operator <<(std::ostream &os, const DateTime::Date &arDate)
{
    os << static_cast<int>(arDate.Year) << "-" << static_cast<uint32_t>(arDate.Month) << "-" << static_cast<uint32_t>(arDate.DayOfMonth);
    return os;
}

std::ostream& operator <<(std::ostream &os, const DateTime::Time &arTime)
{
    os << std::setfill('0') << std::setw(2) << static_cast<uint32_t>(arTime.Hours) << ":"
        << std::setw(2) << static_cast<uint32_t>(arTime.Minutes) << ":"
        << std::setw(2) << static_cast<uint32_t>(arTime.Seconds);
    return os;
}

std::ostream& operator <<(std::ostream &os, const DateTime &arDateTime)
{
    os << arDateTime.GetDate() << " " << arDateTime.GetTime();
    return os;
}

} /* namespace rsp::utils */
