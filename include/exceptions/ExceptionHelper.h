/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2020 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef SRC_UTILS_EXCEPTIONHELPER_H_
#define SRC_UTILS_EXCEPTIONHELPER_H_

#include <system_error>
#include <exception>
#include <stdexcept>

#include <string>
#include <iostream>
#include <sstream>
#include <type_traits>

namespace rsp::exceptions {

/**
 * @see https://stackoverflow.com/questions/16945787/how-is-it-possible-to-overload-the-throw-function-while-writing-a-custom-excepti
 */
template<typename BaseException>
class backtraced_exception: public BaseException
{
private:
    std::string mBacktrace{ };

public:
    template<typename... Args>
    backtraced_exception(const char *aFilename, int aLineNum, Args &&... args)
            : BaseException(std::forward<Args>(args)...)
    {
        std::stringstream ss;
        ss << "From '" << aFilename << ":" << aLineNum << "'->\n"
                << BaseException::what();
        mBacktrace = ss.str();
    }

    backtraced_exception(const std::exception &e, const char *aFilename, int aLineNum)
            : BaseException(static_cast<const BaseException&>(e))
    {
        std::stringstream ss;
        ss << "From '" << aFilename << ":" << aLineNum << "'->\n"
                << e.what();
        mBacktrace = ss.str();
    }

    virtual ~backtraced_exception() noexcept
    {
    }

    virtual const char* what() const noexcept
    {
        return mBacktrace.c_str();
    }
};

#define THROW_WITH_BACKTRACE(EXCEPTION) throw rsp::exceptions::backtraced_exception< EXCEPTION >(__FILE__, __LINE__)
#define THROW_WITH_BACKTRACE1(EXCEPTION, ARG1) throw rsp::exceptions::backtraced_exception< EXCEPTION >(__FILE__, __LINE__, ARG1)
#define THROW_WITH_BACKTRACE2(EXCEPTION, ARG1, ARG2) throw rsp::exceptions::backtraced_exception< EXCEPTION >(__FILE__, __LINE__, ARG1, ARG2)
#define THROW_WITH_BACKTRACE3(EXCEPTION, ARG1, ARG2, ARG3) throw rsp::exceptions::backtraced_exception< EXCEPTION >(__FILE__, __LINE__, ARG1, ARG2, ARG3)

#define RETHROW_WITH_BACKTRACE(aMsg, aOriginalException) THROW_WITH_BACKTRACE1(std::decay< decltype(aOriginalException) >::type, (std::string(aMsg) + " <- " + aOriginalException.what()).c_str())


#define THROW_SYSTEM(aMsg) THROW_WITH_BACKTRACE3(std::system_error, errno, std::generic_category(), aMsg)

#define THROW_RUNTIME(aMsg) THROW_WITH_BACKTRACE1(std::runtime_error, aMsg)
#define RETHROW_RUNTIME(aMsg, aOriginalException) THROW_WITH_BACKTRACE1(std::runtime_error, std::string(aMsg) + " <- " + aOriginalException.what())


} // namespace rsp

#endif /* SRC_UTILS_EXCEPTIONHELPER_H_ */
