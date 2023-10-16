/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2020 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_EXCEPTIONS_EXCEPTIONHELPER_H_
#define INCLUDE_EXCEPTIONS_EXCEPTIONHELPER_H_

#include <system_error>
#include <exception>
#include <stdexcept>

#include <string>
#include <iostream>
#include <sstream>
#include <type_traits>
#include "BackTrace.h"

namespace rsp::exceptions {

/**
 * @see https://stackoverflow.com/questions/16945787/how-is-it-possible-to-overload-the-throw-function-while-writing-a-custom-excepti
 */
template<typename BaseException>
class BackTracedException: public BaseException
{
private:
    std::string mWhat{};

public:
    template<typename... Args>
    BackTracedException(const char *aFilename, int aLineNum, Args &&... args)
            : BaseException(std::forward<Args>(args)...)
    {
        BackTrace bt(1);
        std::stringstream ss;
        ss << "From '" << aFilename << ":" << aLineNum << "'->\n"
                << BaseException::what() << "\n" << bt;

        mWhat = ss.str();
    }

    BackTracedException(const std::exception &e, const char *aFilename, int aLineNum)
            : BaseException(static_cast<const BaseException&>(e))
    {
        BackTrace bt(1);
        std::stringstream ss;
        ss << "From '" << aFilename << ":" << aLineNum << "'->\n"
                << e.what() << "\n" << bt;
        mWhat = ss.str();
    }

/*
    BackTracedException() noexcept
    {
    }
*/

    [[nodiscard]] virtual const char* what() const noexcept
    {
        return mWhat.c_str();
    }
};

#define THROW_WITH_BACKTRACE(EXCEPTION) throw rsp::exceptions::BackTracedException< EXCEPTION >(__FILE__, __LINE__)
#define THROW_WITH_BACKTRACE1(EXCEPTION, ARG1) throw rsp::exceptions::BackTracedException< EXCEPTION >(__FILE__, __LINE__, ARG1)
#define THROW_WITH_BACKTRACE2(EXCEPTION, ARG1, ARG2) throw rsp::exceptions::BackTracedException< EXCEPTION >(__FILE__, __LINE__, ARG1, ARG2)
#define THROW_WITH_BACKTRACE3(EXCEPTION, ARG1, ARG2, ARG3) throw rsp::exceptions::BackTracedException< EXCEPTION >(__FILE__, __LINE__, ARG1, ARG2, ARG3)

// The CATCH macro does not work in all cases.
//#define CATCH_WITH_BACKTRACE(EXCEPTION, EXCEPT_NAME) catch(const rsp::BackTracedException< EXCEPTION >& EXCEPT_NAME)

//#define RETHROW_WITH_BACKTRACE(EXCEPT_NAME) throw rsp::BackTracedException< std::decay< decltype(EXCEPT_NAME) >::type >(EXCEPT_NAME, __FILE__, __LINE__)
#define RETHROW_WITH_BACKTRACE(aMsg, aOriginalException) THROW_WITH_BACKTRACE1(std::decay< decltype(aOriginalException) >::type, (std::string(aMsg) + " <- " + aOriginalException.what()).c_str())


#define THROW_SYSTEM(aMsg) THROW_WITH_BACKTRACE3(std::system_error, errno, std::generic_category(), aMsg)

#define THROW_RUNTIME(aMsg) THROW_WITH_BACKTRACE1(std::runtime_error, aMsg)
#define RETHROW_RUNTIME(aMsg, aOriginalException) THROW_WITH_BACKTRACE1(std::runtime_error, std::string(aMsg) + " <- " + aOriginalException.what())

} // namespace rsp::exceptions

#endif /* INCLUDE_EXCEPTIONS_EXCEPTIONHELPER_H_ */
