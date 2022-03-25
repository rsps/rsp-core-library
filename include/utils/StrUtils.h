/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_UTILS_STRUTILS_H_
#define SRC_UTILS_STRUTILS_H_

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <chrono>

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
size_t Split(const std::string &arTxt, std::vector<std::string> &aList, char aDelimiter = ' ', bool aKeepEmpty = false);

/**
 * Trim a string, based on locale settings for whitespace.
 *
 * @see https://stackoverflow.com/a/29892589
 *
 * @param aStr String to trim
 * @return The trimmed string
 */
std::string& Trim(std::string &aStr);

/**
 * Left trim a string, based on locale settings for whitespace.
 *
 * @see https://stackoverflow.com/a/29892589
 *
 * @param aStr String to trim
 * @return The trimmed string
 */
std::string& LeftTrim(std::string &aStr);

/**
 * Right trim a string, based on locale settings for whitespace.
 *
 * @see https://stackoverflow.com/a/29892589
 *
 * @param aStr String to trim
 * @return The trimmed string
 */
std::string& RightTrim(std::string &aStr);

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
bool StartsWith(const std::string &aText, const std::string &aPrefix);

/**
 * Check if string ends with given substring
 *
 * @param aText Text to check for presence of substring.
 * @param aAffix Substring to find at end of text.
 * @return True if Text ends with substring
 */
bool EndsWith(const std::string &aText, const std::string &aAffix);

/**
 * Check if a string contains the given substring
 *
 * @param aText Text to check for presence of substring.
 * @param aMatch Substring to find in text.
 * @return True if text contains the substring
 */
bool Contains(const std::string &aText, const std::string &aMatch);

/**
 * Get a list of strings in arList that starts with aText.
 *
 * @param aText String to find in list
 * @param Reference to list of strings to search in.
 * @return List with matching strings
 */
std::vector<std::string> FindMatches(std::string aText, std::vector<std::string> &arList);

/**
 * Find all characters common from start of all strings in a list.
 *
 * @param arList List of strings to search
 * @return String with common start of all strings
 */
std::string ReduceToCommon(std::vector<std::string> &arList);


/**
 * Find all occurrences of aFrom and replace them with aTo.
 *
 * @param aText
 * @param aFrom
 * @param aTo
 * @return
 */
std::string& ReplaceAll(std::string &aText, const std::string& aFrom, const std::string& aTo);


enum class TimeFormats { Normal, Logging, RFC3339, RFC3339Milli };
/**
 * Format a time_point into a string timestamp with milliseconds.
 *
 * @param aTime
 * @return String
 */
std::string TimeStamp();
std::string TimeStamp(TimeFormats aFormat);
std::string TimeStamp(std::chrono::steady_clock::time_point aTime, TimeFormats aFormat = TimeFormats::Logging);
std::string TimeStamp(std::chrono::system_clock::time_point aTime, TimeFormats aFormat = TimeFormats::Logging);
std::string TimeStamp(std::chrono::milliseconds aMilliSeconds, TimeFormats aFormat = TimeFormats::Logging);


/**
 * Create and format a std::string using printf format specifiers.
 *
 * @tparam Args Variable template arguments, forwarded to sprintf
 * @param format sprintf format string
 * @param args Optional arguments
 * @return std::string formatted with snprintf
 */
std::string Format(const char* apFormat, ...) __attribute__((__format__(__printf__, 1, 0)));

}

#endif /* SRC_UTILS_STRUTILS_H_ */
