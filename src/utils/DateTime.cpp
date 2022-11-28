/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <string>
#include <sstream>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <utils/DateTime.h>

using namespace std::literals::chrono_literals;
using namespace std::chrono;

namespace rsp::utils {

DateTime::DateTime()
    : mTp(std::chrono::system_clock::now())
{
}

DateTime::DateTime(int aYear, unsigned aMonth, unsigned aDayOfMonth, int aHour, int aMinute, int aSecond, int aMilliSecond)
{
    Date dt{year(aYear), month(aMonth), day(aDayOfMonth)};
    Time tm{seconds((aHour * 3600) + (aMinute * 60) + aSecond)};
    sys_days sd = dt;
    mTp = sd + tm.to_duration() + milliseconds(aMilliSecond);
}

DateTime::DateTime(std::chrono::system_clock::duration aDuration)
    : mTp(aDuration)
{
}

DateTime::DateTime(std::chrono::system_clock::time_point aTimePoint)
    : mTp(aTimePoint)
{
}

DateTime::DateTime(std::filesystem::file_time_type aFileTime)
    : mTp(std::chrono::file_clock::to_sys(aFileTime))
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
            FromString(arTimeString, "%Y-%m-%dT%T");
            break;

        case Formats::RFC3339Milli:
            FromString(arTimeString, "%Y-%m-%dT%T.");
            break;

        case Formats::ISO8601:
            FromString(arTimeString, "%Y-%m-%d %T%z");
            break;

        case Formats::ISO8601UTC:
            FromString(arTimeString, "%Y-%m-%d %T");
            break;

        case Formats::Logging:
            FromString(arTimeString, "%Y-%m-%d %T.");
            break;

        case Formats::HTTP:
            FromString(arTimeString, "%a, %d %b %Y %H:%M:%S");
            break;

        default:
            break;
    }
}

DateTime::DateTime(std::time_t aSeconds)
    : mTp(seconds(aSeconds))
{
}

DateTime::DateTime(std::tm &arTm)
    : mTp(system_clock::from_time_t(std::mktime(&arTm)))
{
    mTp -= getTimezoneOffset(arTm);
}

int64_t DateTime::SecondsBetween(const DateTime &arOther) const
{
    system_clock::time_point tp = arOther;
    return duration_cast<seconds>(tp - mTp).count();
}

int64_t DateTime::MilliSecondsBetween(const DateTime &arOther) const
{
    system_clock::time_point tp = arOther;
    return duration_cast<milliseconds>(tp - mTp).count();
}

int64_t DateTime::MicroSecondsBetween(const DateTime &arOther) const
{
    system_clock::time_point tp = arOther;
    return duration_cast<microseconds>(tp - mTp).count();
}

int64_t DateTime::NanoSecondsBetween(const DateTime &arOther) const
{
    system_clock::time_point tp = arOther;
    return duration_cast<nanoseconds>(tp - mTp).count();
}

DateTime::operator std::chrono::system_clock::duration() const
{
    return mTp.time_since_epoch();
}

DateTime::operator std::chrono::system_clock::time_point() const
{
    return mTp;
}

DateTime::operator std::filesystem::file_time_type() const
{
    return std::chrono::file_clock::from_sys(mTp);
}

DateTime::operator std::time_t() const
{
    return system_clock::to_time_t(mTp);
}

DateTime::operator std::tm() const
{
    std::tm result;
    auto t = system_clock::to_time_t(mTp );
    gmtime_r(&t, &result);
//    t -= getTimezoneOffset(result).count();
//    gmtime_r(&t, &result);
    return result;


}

std::string DateTime::ToString(const char *apFormat) const
{
    std::tm tm = *this;
    std::stringstream ss;
    ss << std::put_time(&tm, apFormat);

    if (std::string(apFormat).back() == '.') {
        encodeFractions(ss, mTp);
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
    return ToString("%FT%T.") + "Z";
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

    mTp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    mTp += decodeFractions(msecs) - getTimezoneOffset(tm);

    return *this;
}

DateTime DateTime::operator +(const DateTime &arOther) const
{
    return DateTime(mTp.time_since_epoch() + arOther.mTp.time_since_epoch());
}

DateTime DateTime::operator -(const DateTime &arOther) const
{
    return DateTime(mTp - arOther.mTp);
}

DateTime& DateTime::operator +=(const DateTime &arOther)
{
    mTp += arOther.mTp.time_since_epoch();
    return *this;
}

DateTime& DateTime::operator -=(const DateTime &arOther)
{
    mTp -= arOther.mTp.time_since_epoch();
    return *this;
}

DateTime DateTime::operator+(const std::chrono::system_clock::duration &arDuration) const
{
    return DateTime(mTp + arDuration);
}

DateTime DateTime::operator-(const std::chrono::system_clock::duration &arDuration) const
{
    return DateTime(mTp - arDuration);
}

DateTime& DateTime::operator +=(const std::chrono::system_clock::duration &arDuration)
{
    mTp += arDuration;
    return *this;
}

DateTime& DateTime::operator -=(const std::chrono::system_clock::duration &arDuration)
{
    mTp -= arDuration;
    return *this;
}


bool DateTime::operator <(const DateTime &arOther) const
{
    return mTp < arOther.mTp;
}

bool DateTime::operator >(const DateTime &arOther) const
{
    return mTp > arOther.mTp;
}

bool DateTime::operator <=(const DateTime &arOther) const
{
    return mTp <= arOther.mTp;
}

bool DateTime::operator >=(const DateTime &arOther) const
{
    return mTp >= arOther.mTp;
}

bool DateTime::operator ==(const DateTime &arOther) const
{
    return mTp == arOther.mTp;
}

bool DateTime::operator !=(const DateTime &arOther) const
{
    return mTp != arOther.mTp;
}

std::chrono::seconds DateTime::getTimezoneOffset(std::tm &arTm) const
{
    std::time_t t = std::mktime(&arTm);
    std::tm gm_tm;
    gmtime_r(&t, &gm_tm);
    gm_tm.tm_isdst = false;
    std::time_t tz_offset = (std::mktime(&gm_tm) - t);
    return seconds(tz_offset);
}

DateTime::Date DateTime::GetDate() const
{
    return Date(time_point_cast<days>(mTp));
}

DateTime::Time DateTime::GetTime() const
{
    sys_days sd = time_point_cast<days>(mTp);
    return Time(duration_cast<milliseconds>(mTp - sd));
}

std::ostream& operator <<(std::ostream &os, const DateTime::Date &arDate)
{
    os << std::setfill('0') << std::setw(4) << static_cast<int>(arDate.year()) << "-"
        << std::setw(2) << static_cast<uint32_t>(arDate.month()) << "-"
        << std::setw(2) << static_cast<uint32_t>(arDate.day());
    return os;
}

std::ostream& operator <<(std::ostream &os, const DateTime::Time &arTime)
{
    milliseconds msecs = duration_cast<milliseconds>(arTime.subseconds());

    os << std::setfill('0') << std::setw(2) << arTime.hours().count() << ":"
        << std::setw(2) << arTime.minutes().count() << ":"
        << std::setw(2) << arTime.seconds().count() << "."
        << std::setw(3) << msecs.count();
    return os;
}

std::ostream& operator <<(std::ostream &os, const DateTime &arDateTime)
{
    os << arDateTime.GetDate() << " " << arDateTime.GetTime();
    return os;
}

std::chrono::system_clock::duration DateTime::decodeFractions(uint64_t aFractions) const
{
    if (aFractions >= 1000000) {
        return nanoseconds(aFractions);
    }
    if (aFractions >= 1000) {
        return microseconds(aFractions);
    }
    return milliseconds(aFractions);
}

std::ostream& DateTime::encodeFractions(std::ostream& os, std::chrono::system_clock::time_point aTp) const
{
    time_point<system_clock, milliseconds> msd = time_point_cast<milliseconds>(aTp);
    unsigned msecs = msd.time_since_epoch().count() % 1000;

    os << std::setfill('0') << std::setw(3) << msecs;
    return os;
}

} /* namespace rsp::utils */
