/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <exception>
#include <stdexcept>
#include <system_error>
#include <sstream>
#include "BackTrace.h"

#ifndef INCLUDE_EXCEPTIONS_TRACEDEXCEPTION_H_
#define INCLUDE_EXCEPTIONS_TRACEDEXCEPTION_H_

namespace rsp::exceptions {


template<class BaseException>
class TracedException : public BaseException {
public:
    template<typename... Args>
    TracedException(Args &&... args)
        : BaseException(std::forward<Args>(args)...)
    {
        BackTrace bt(2);

        std::stringstream ss;
        ss << BaseException::what() << "\n" << bt;
        mWhat = ss.str();
    }

    const char* what() const noexcept override
    {
        return mWhat.c_str();
    }

protected:
    std::string mWhat{};
};


typedef TracedException<std::domain_error>     DomainError;
typedef TracedException<std::invalid_argument> InvalidArgument;
typedef TracedException<std::length_error>     LengthError;
typedef TracedException<std::logic_error>      LogicError;
typedef TracedException<std::overflow_error>   OverflowError;
typedef TracedException<std::out_of_range>     OutOfRange;
typedef TracedException<std::range_error>      RangeError;
typedef TracedException<std::runtime_error>    RuntimeError;
typedef TracedException<std::system_error>     SystemError;
typedef TracedException<std::underflow_error>  UnderflowError;

} /* namespace rsp::exceptions */

#endif /* INCLUDE_EXCEPTIONS_TRACEDEXCEPTION_H_ */
