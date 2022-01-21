/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
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

}

#endif // RSPCOREEXCEPTIONS_H
