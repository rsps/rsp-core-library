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
#include <iomanip>
#include <utils/DateTime.h>

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


DateTime& DateTime::Add(const DateTime &arOther)
{
    return Add(arOther.mJulianDays, arOther.mNanoSecondsOfDay);
}

DateTime& DateTime::Add(int64_t aSeconds)
{
    return Add(0, aSeconds * cNanoSecondsPerSecond);
}

DateTime& DateTime::Add(int64_t aDays, int64_t aNanoSeconds)
{
    mJulianDays += aDays;
    mNanoSecondsOfDay += aNanoSeconds;
    if (mNanoSecondsOfDay > cNanoSecondsPerDay) {
        mJulianDays += (mNanoSecondsOfDay / cNanoSecondsPerDay);
        mNanoSecondsOfDay %= cNanoSecondsPerDay;
    }
    return *this;
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

int64_t DateTime::SecondsBetween(const DateTime &arOther) const
{
    int64_t result = (mJulianDays - arOther.mJulianDays) * cSecondsPerDay;
    result += (mNanoSecondsOfDay - arOther.mNanoSecondsOfDay) / cNanoSecondsPerSecond;
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
    std::filesystem::file_time_type ft{std::chrono::nanoseconds((mJulianDays * cNanoSecondsPerDay) + mNanoSecondsOfDay)};
    return ft;
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

DateTime::Date DateTime::GetDate() const
{
    return Date(mJulianDays);
}

DateTime& DateTime::FromString(const std::string &arTimeString, const char *apFormat)
{
    std::tm tm = {};
    unsigned int msecs = 0;
    std::stringstream ss(arTimeString);
    ss >> std::get_time(&tm, apFormat);
    if (std::string(apFormat).back() == '.') {
        ss >> msecs;
    }
    auto sctp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    sctp += std::chrono::milliseconds(msecs);

    auto tp = std::chrono::time_point_cast<std::chrono::nanoseconds>(sctp);
    mJulianDays = (tp.time_since_epoch().count() / cNanoSecondsPerDay);
    mNanoSecondsOfDay = (tp.time_since_epoch().count() % cNanoSecondsPerDay);

    return *this;
}

DateTime::Time DateTime::GetTime() const
{
    return Time(mNanoSecondsOfDay);
}

DateTime::Date::Date(int64_t aJulianDays)
{
    uint32_t i, j, d, m, y;
    uint32_t l = aJulianDays + 68569;
    uint32_t n = ( 4 * l ) / 146097;
    l = l - ( 146097 * n + 3 ) / 4;
    i = ( 4000 * ( l + 1 ) ) / 1461001;
    l = l - ( 1461 * i ) / 4 + 31;
    j = ( 80 * l ) / 2447;
    d = l - ( 2447 * j ) / 80;
    l = j / 11;
    m = j + 2 - ( 12 * l );
    y = 100 * ( n - 49 ) + i + l;

    Year = y;
    Month = m;
    DayOfMonth = d;
}

int64_t DateTime::Date::ToJulianDays() const
{
    uint32_t a  = (14 - static_cast<uint32_t>(Month)) / 12;
    uint32_t y1 = static_cast<uint32_t>(Year) + 4800 - a;
    uint32_t m1 = static_cast<uint32_t>(Month) + (12 * a) - 3;
    int64_t jd = static_cast<int64_t>(DayOfMonth) + (153 * m1 + 2) / 5 + y1 * 365 + y1 / 4 - y1 / 100 + y1 / 400 - 32045;
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

} /* namespace rsp::utils */
