/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_EXCEPTIONS_BACK_TRACE_H
#define RSP_CORE_LIB_EXCEPTIONS_BACK_TRACE_H

#include <string>
#include <ostream>
#include <vector>

namespace rsp::exceptions {

#if !defined(NO_BACKTRACE_IN_EXCEPTIONS) && defined(_GLIBCXX_HAVE_STACKTRACE)
#   define BACKTRACE_IN_EXCEPTIONS 1
#endif


class StackEntry
{
public:
    StackEntry(std::string aFile, std::string aFunc, const unsigned long aLine) noexcept
        : mFileName(std::move(aFile)),
          mFunction(std::move(aFunc)),
          mLineNumber(aLine)
    {
    }

protected:
    friend std::ostream& operator<<(std::ostream& o, const StackEntry& arEntry);

    std::string mFileName;
    std::string mFunction;
    unsigned long mLineNumber;
};

std::ostream& operator<<(std::ostream& o, const StackEntry& arEntry);

/**
 * \brief Using the GNU extension has severe limits. Hopefully it will get better with C++23 std::stacktrace
 */
class BackTrace
{
public:
    explicit BackTrace(size_t aEntriesToDiscard);
    virtual ~BackTrace() noexcept = default;

protected:
#if !defined(BACKTRACE_IN_EXCEPTIONS) && defined(__x86_64__)
    static std::string demangle(const std::string& arMangled);
#endif

    friend std::ostream& operator<<(std::ostream& o, const BackTrace& arBackTrace);
    std::vector<StackEntry> mStackEntries{};
};

std::ostream& operator<<(std::ostream& o, const BackTrace& arBackTrace);

} /* namespace rsp::exceptions */

#endif // RSP_CORE_LIB_EXCEPTIONS_BACK_TRACE_H
