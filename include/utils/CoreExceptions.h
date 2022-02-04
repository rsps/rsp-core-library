/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#ifndef RSPCOREEXCEPTIONS_H
#define RSPCOREEXCEPTIONS_H

#include <exception>
#include <stdexcept>
#include <system_error>
#include "ExceptionHelper.h"

namespace rsp::utils
{

class CoreException : public std::runtime_error
{
public:
    explicit CoreException(const char *aMsg)
        : std::runtime_error("")
    {
        mMsg = aMsg;
    }

    const char *what() const noexcept override
    {
        return mMsg.c_str();
    }

  protected:
    std::string mMsg;
};

class NotImplementedException : public CoreException
{
public:
    explicit NotImplementedException(const char *aMsg)
        : CoreException(aMsg)
    {
    }
};

class AssertException : public CoreException
{
public:
    explicit AssertException(const char *aMsg)
        : CoreException(aMsg)
    {
    }
};

#ifndef NDEBUG
#define ASSERT(a)                  \
    if (a) {                       \
        throw rsp::utils::AssertException(#a); \
    }
#else
#define ASSERT(a)
#endif


class NotsetException : public CoreException
{
public:
    explicit NotsetException(const char *aMsg)
    : CoreException(aMsg)
    {
    }
};


class ApplicationException : public CoreException {
public:
    explicit ApplicationException(const char* aMsg) : CoreException(aMsg) {}
};

class ESingletonViolation: public ApplicationException {
public:
    explicit ESingletonViolation() : ApplicationException("Application already exist") {}
};

class ENoInstance: public ApplicationException {
public:
    explicit ENoInstance() : ApplicationException("Application has not been created") {}
};

class EMissingArgument : public std::invalid_argument {
public:
    explicit EMissingArgument() : std::invalid_argument("Missing argument") {}
};

}

#endif // RSPCOREEXCEPTIONS_H
