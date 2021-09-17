/*
 * ExceptionHelper.h
 *
 *  Created on: 5. mar. 2020
 *      Author: steffen
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

namespace rsp {

/**
 * @see https://stackoverflow.com/questions/16945787/how-is-it-possible-to-overload-the-throw-function-while-writing-a-custom-excepti
 */
template<typename BaseException>
class backtraced_exception: public BaseException
{
private:
    std::string mBacktrace;

public:
    template<typename... Args>
    backtraced_exception(const char *aFilename, int aLineNum, Args &&... args)
            : BaseException(std::forward<Args>(args)...)
    {
        std::stringstream ss;
        ss << "From '" << aFilename << ":" << aLineNum << "'->\n"
                << BaseException::what();
        mBacktrace = ss.str();
    };

    backtraced_exception(const std::exception &e, const char *aFilename, int aLineNum)
            : BaseException(static_cast<const BaseException&>(e))
    {
        std::stringstream ss;
        ss << "From '" << aFilename << ":" << aLineNum << "'->\n"
                << e.what();
        mBacktrace = ss.str();
    };

    virtual ~backtraced_exception() noexcept
    {
    };

    virtual const char* what() const noexcept
    {
        return mBacktrace.c_str();
    };
};

#define THROW_WITH_BACKTRACE(EXCEPTION) throw rsp::backtraced_exception< EXCEPTION >(__FILE__, __LINE__)
#define THROW_WITH_BACKTRACE1(EXCEPTION, ARG1) throw rsp::backtraced_exception< EXCEPTION >(__FILE__, __LINE__, ARG1)
#define THROW_WITH_BACKTRACE2(EXCEPTION, ARG1, ARG2) throw rsp::backtraced_exception< EXCEPTION >(__FILE__, __LINE__, ARG1, ARG2)
#define THROW_WITH_BACKTRACE3(EXCEPTION, ARG1, ARG2, ARG3) throw rsp::backtraced_exception< EXCEPTION >(__FILE__, __LINE__, ARG1, ARG2, ARG3)

// The CATCH macro does not work in all cases.
//#define CATCH_WITH_BACKTRACE(EXCEPTION, EXCEPT_NAME) catch(const rsp::backtraced_exception< EXCEPTION >& EXCEPT_NAME)

//#define RETHROW_WITH_BACKTRACE(EXCEPT_NAME) throw rsp::backtraced_exception< std::decay< decltype(EXCEPT_NAME) >::type >(EXCEPT_NAME, __FILE__, __LINE__)
#define RETHROW_WITH_BACKTRACE(aMsg, aOriginalException) THROW_WITH_BACKTRACE1(std::decay< decltype(aOriginalException) >::type, (std::string(aMsg) + " <- " + aOriginalException.what()).c_str())


#define THROW_SYSTEM(aMsg) THROW_WITH_BACKTRACE3(std::system_error, errno, std::generic_category(), aMsg)

#define THROW_RUNTIME(aMsg) THROW_WITH_BACKTRACE1(std::runtime_error, aMsg)
#define RETHROW_RUNTIME(aMsg, aOriginalException) THROW_WITH_BACKTRACE1(std::runtime_error, std::string(aMsg) + " <- " + aOriginalException.what())


} // namespace rsp

#endif /* SRC_UTILS_EXCEPTIONHELPER_H_ */
