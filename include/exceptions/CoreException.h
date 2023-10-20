/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021-2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_EXCEPTIONS_CORE_EXCEPTION_H
#define RSP_CORE_LIB_EXCEPTIONS_CORE_EXCEPTION_H

#include <string_view>
#include <exceptions/ExceptionHelper.h>
#include <system_error>

namespace rsp::exceptions {


class CoreException: public std::runtime_error
{
public:
    explicit CoreException(const char *apMsg)
        : std::runtime_error(std::string(apMsg))
    {
    }
    explicit CoreException(const std::string &arMsg)
        : std::runtime_error(arMsg)
    {
    }
};

class NotImplementedException: public CoreException
{
public:
    explicit NotImplementedException(const char *aMsg = "Not Implemented")
        : CoreException(aMsg)
    {
    }
    explicit NotImplementedException(const std::string &arMsg)
        : CoreException(arMsg)
    {
    }
};

class NotSetException: public CoreException
{
public:
    explicit NotSetException(const char *aMsg)
        : CoreException(aMsg)
    {
    }
};

class DecimalConversionError : public CoreException
{
public:
    explicit DecimalConversionError(const std::string &arMsg)
        : CoreException(arMsg)
    {
    }
};

class AssertException: public CoreException
{
public:
    explicit AssertException(const char *aMsg)
        : CoreException(aMsg)
    {
    }
};

#ifndef NDEBUG
#define ASSERT(a)                  \
    if (!(a)) {                       \
        THROW_WITH_BACKTRACE1(rsp::exceptions::AssertException, #a); \
    }
#else
#define ASSERT(a)
#endif

class ApplicationException: public CoreException
{
public:
    explicit ApplicationException(const char *aMsg)
        : CoreException(aMsg)
    {
    }
    explicit ApplicationException(const std::string &arMsg)
        : CoreException(arMsg)
    {
    }
};

class ESingletonViolation: public ApplicationException
{
public:
    explicit ESingletonViolation(std::string_view aClass)
        : ApplicationException(std::string("Singleton<") + std::string(aClass) + std::string(">() object already exist"))
    {
    }
};

class ENoInstance: public ApplicationException
{
public:
    explicit ENoInstance(std::string_view aClass)
        : ApplicationException(std::string("Singleton<") + std::string(aClass) + std::string(">() object has not been created"))
    {
    }
};

class ETerminate: public ApplicationException
{
public:
    explicit ETerminate(int aExitCode)
        : ApplicationException(std::string("Terminating with code: ") + std::to_string(aExitCode)),
          mCode(aExitCode)
    {
    }

    [[nodiscard]] int GetCode() const { return mCode; }

protected:
    int mCode = 0;
};

class EMissingArgument: public std::invalid_argument
{
public:
    explicit EMissingArgument()
        : std::invalid_argument("Missing argument")
    {
    }
};

class EBadCast: public CoreException
{
public:
    explicit EBadCast(std::string_view aFrom, std::string_view aTo)
        : EBadCast(std::string(aFrom), std::string(aTo))
    {
    }
    explicit EBadCast(const std::string &arFrom, const std::string &arTo)
        : CoreException(std::string("Cannot cast ") + arFrom + std::string(" to ") + arTo)
    {
    }
};

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_EXCEPTIONS_CORE_EXCEPTION_H
