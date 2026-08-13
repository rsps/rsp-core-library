/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_STR_UTILS_H
#define RSP_CORE_LIB_UTILS_STR_UTILS_H

#include <algorithm>
#include <string_view>
#include <string>
#include <vector>

namespace rsp::utils::StrUtils {

/**
 * Split a string by given delimiter
 *
 * @param aTxt String to split.
 * @param aList Reference to list to hold elements.
 * @param aDelimiter Character to use as delimiter.
 * @param aKeepEmpty Set if empty elements should be included in list.
 * @return Number of elements
 */
size_t Split(const std::string& arTxt, std::vector<std::string>& aList, char aDelimiter = ' ', bool aKeepEmpty = false);

/**
 * Left trim a string, based on locale settings for whitespace.
 *
 * @see https://stackoverflow.com/a/29892589
 *
 * @param aStr String to trim
 * @return The trimmed string
 */
inline std::string& LeftTrim(std::string& aStr)
{
    aStr.erase(aStr.begin(), std::find_if(aStr.begin(), aStr.end(), [](int c) { return !std::isspace(c); }));
    return aStr;
}

/**
 * Right trim a string, based on locale settings for whitespace.
 *
 * @see https://stackoverflow.com/a/29892589
 *
 * @param aStr String to trim
 * @return The trimmed string
 */
inline std::string& RightTrim(std::string& aStr)
{
    aStr.erase(std::find_if(aStr.rbegin(), aStr.rend(), [](int c) { return !std::isspace(c); }).base(), aStr.end());
    return aStr;
}

/**
 * Trim a string, based on locale settings for whitespace.
 *
 * @see https://stackoverflow.com/a/29892589
 *
 * @param aStr String to trim
 * @return The trimmed string
 */
inline std::string& Trim(std::string& aStr)
{
    return LeftTrim(RightTrim(aStr));
}

/**
 * Trim a string, based on locale settings for whitespace.
 *
 * @see https://stackoverflow.com/a/29892589
 *
 * @param aStr String to trim
 * @return Copy of the trimmed string
 */
std::string TrimCopy(std::string const& aStr);

/**
 * \brief Convert all characters in string to lower case
 *
 * \param arStr Source string
 * \return source string with lower case characters
 */
std::string& ToLower(std::string& arStr);
std::string ToLower(const std::string& arStr);

/**
 * \brief Convert all characters in string to upper case
 *
 * \param arStr Source string
 * \return source string with upper case characters
 */
std::string& ToUpper(std::string& arStr);
std::string ToUpper(const std::string& arStr);

/**
 * Get the user current home directory.
 *
 * @see https://stackoverflow.com/a/26696759
 *
 * @return string with home directory path.
 */
std::string GetHomeDir();

/**
 * Get the user current configuration directory.
 *
 * @see https://stackoverflow.com/a/26696759
 *
 * @return string with data directory path.
 */
std::string GetConfigDir();

/**
 * Check if string starts with given substring.
 *
 * @param aText Text to check for presence of substring.
 * @param aPrefix Substring to find at start of text.
 * @return True if Text starts with substring
 */
bool StartsWith(const std::string& aText, const std::string& aPrefix);

/**
 * Check if string ends with given substring
 *
 * @param aText Text to check for presence of substring.
 * @param aAffix Substring to find at end of text.
 * @return True if Text ends with substring
 */
bool EndsWith(const std::string& aText, const std::string& aAffix);

/**
 * Check if a string contains the given substring
 *
 * @param aText Text to check for presence of substring.
 * @param aMatch Substring to find in text.
 * @return True if text contains the substring
 */
bool Contains(const std::string& aText, const std::string& aMatch);

/**
 * Get a list of strings in arList that starts with arText.
 *
 * @param arText String to find in list
 * @param Reference to list of strings to search in.
 * @return List with matching strings
 */
std::vector<std::string> FindMatches(const std::string& arText, std::vector<std::string>& arList);

/**
 * Find all characters common from start of all strings in a list.
 *
 * @param arList List of strings to search
 * @return String with common start of all strings
 */
std::string ReduceToCommon(std::vector<std::string>& arList);


/**
 * Find all occurrences of aFrom and replace them with aTo.
 *
 * @param aText
 * @param aFrom
 * @param aTo
 * @return
 */
std::string& ReplaceAll(std::string& aText, const std::string& aFrom, const std::string& aTo);


/**
 * Create and format a std::string using printf format specifiers.
 *
 * @tparam Args Variable template arguments, forwarded to sprintf
 * @param format sprintf format string
 * @param args Optional arguments
 * @return std::string formatted with snprintf
 */
std::string Format(const char* apFormat, ...) __attribute__((__format__(__printf__, 1, 0)));

/**
 * \brief Convert a string to double. Always uses '.' as decimal point.
 * \param arString
 * \return double
 */
double ToDouble(const std::string& arString);

/**
 * \brief Convert a double to string with the given precision. Always uses '.' as decimal point.
 * \param aValue double
 * \param aDigits Number of decimals in result.
 * \param aFixed Force decimal point notation
 * \return string
 */
std::string ToString(double aValue, int aDigits = -1, bool aFixed = false);

/**
 * \brief Convert a float to string with the given precision. Always uses '.' as decimal point.
 * \param aValue float
 * \param aDigits Number of decimals in result.
 * \param aFixed Force decimal point notation
 * \return string
 */
std::string ToString(float aValue, int aDigits = -1, bool aFixed = false);


constexpr std::string Utf32ToUtf8(char32_t aCodePoint)
{
    std::string result;
    result.resize_and_overwrite(4, [aCodePoint](char* apBuf, std::size_t) -> std::size_t {
        if (aCodePoint < 0x80) {
            apBuf[0] = static_cast<char>(aCodePoint);
            return 1;
        }
        if (aCodePoint < 0x800) {
            apBuf[0] = static_cast<char>(0xC0 | (aCodePoint >> 6));
            apBuf[1] = static_cast<char>(0x80 | (aCodePoint & 0x3F));
            return 2;
        }
        if (aCodePoint < 0x10000) {
            apBuf[0] = static_cast<char>(0xE0 | (aCodePoint >> 12));
            apBuf[1] = static_cast<char>(0x80 | ((aCodePoint >> 6) & 0x3F));
            apBuf[2] = static_cast<char>(0x80 | (aCodePoint & 0x3F));
            return 3;
        }
        if (aCodePoint <= 0x10FFFF) {
            apBuf[0] = static_cast<char>(0xF0 | (aCodePoint >> 18));
            apBuf[1] = static_cast<char>(0x80 | ((aCodePoint >> 12) & 0x3F));
            apBuf[2] = static_cast<char>(0x80 | ((aCodePoint >> 6) & 0x3F));
            apBuf[3] = static_cast<char>(0x80 | (aCodePoint & 0x3F));
            return 4;
        }
        return 0; // invalid code point
    });
    return result;
}

constexpr std::string Utf32ToUtf8(std::u32string_view aUtf32)
{
    std::string result;
    result.reserve(aUtf32.size());
    for (char32_t cp : aUtf32) {
        result += Utf32ToUtf8(cp);
    }
    return result;
}

constexpr std::u32string Utf8ToUtf32(std::string_view aUtf8)
{
    std::u32string result;
    result.reserve(aUtf8.size());
    for (size_t i = 0; i < aUtf8.size();) {
        char32_t cp = 0;
        auto c = static_cast<unsigned char>(aUtf8[i]);
        if (c < 0x80) {
            cp = c;
            i += 1;
        }
        else if ((c >> 5) == 0x06) {
            cp = char32_t(c & 0x1F) << 6;
            cp |= char32_t(static_cast<unsigned char>(aUtf8[i + 1]) & 0x3F);
            i += 2;
        }
        else if ((c >> 4) == 0x0E) {
            cp = char32_t(c & 0x0F) << 12;
            cp |= char32_t(static_cast<unsigned char>(aUtf8[i + 1]) & 0x3F) << 6;
            cp |= char32_t(static_cast<unsigned char>(aUtf8[i + 2]) & 0x3F);
            i += 3;
        }
        else if ((c >> 3) == 0x1E) {
            cp = char32_t(c & 0x07) << 18;
            cp |= char32_t(static_cast<unsigned char>(aUtf8[i + 1]) & 0x3F) << 12;
            cp |= char32_t(static_cast<unsigned char>(aUtf8[i + 2]) & 0x3F) << 6;
            cp |= char32_t(static_cast<unsigned char>(aUtf8[i + 3]) & 0x3F);
            i += 4;
        }
        else {
            i += 1; // skip invalid byte
            continue;
        }
        result.push_back(cp);
    }
    return result;
}

} // namespace rsp::utils::StrUtils

#endif // RSP_CORE_LIB_UTILS_STR_UTILS_H
