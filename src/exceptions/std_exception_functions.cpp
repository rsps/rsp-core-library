/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <exceptions/TracedException.h>
#include <new>
#include <typeinfo>

/**
 * \brief Override all standard exception functions with backtraced alternatives.
 */

using namespace rsp::exceptions;

namespace std {

void
__throw_bad_exception(void)
{
    throw TracedException<bad_exception>();
}

// Helper for exception objects in <new>
void
__throw_bad_alloc(void)
{
    throw TracedException<bad_alloc>();
}

void
__throw_bad_array_new_length(void)
{
    throw TracedException<bad_array_new_length>();
}

// Helper for exception objects in <typeinfo>
void
__throw_bad_cast(void)
{
    throw TracedException<bad_cast>();
}

void
__throw_bad_typeid(void)
{
    throw TracedException<bad_typeid>();
}

// Helpers for exception objects in <stdexcept>
void
__throw_logic_error(const char* apMsg)
{
    throw LogicError(apMsg);
}

void
__throw_domain_error(const char* apMsg)
{
    throw DomainError(apMsg);
}

void
__throw_invalid_argument(const char* apMsg)
{
    throw InvalidArgument(apMsg);
}

void
__throw_length_error(const char* apMsg)
{
    throw LengthError(apMsg);
}

void
__throw_out_of_range(const char* apMsg)
{
    throw OutOfRange(apMsg);
}

//void
//__throw_out_of_range_fmt(const char*, ...)
//__attribute__((__format__(__gnu_printf__, 1, 2)))
//{
//    throw OutOfRange(apMsg);
//}

void
__throw_runtime_error(const char* apMsg)
{
    throw RuntimeError(apMsg);
}


void
__throw_range_error(const char* apMsg)
{
    throw RangeError(apMsg);
}


void
__throw_overflow_error(const char* apMsg)
{
    throw OverflowError(apMsg);
}


void
__throw_underflow_error(const char* apMsg)
{
    throw UnderflowError(apMsg);
}


// Helpers for exception objects in <ios>
//void
//__throw_ios_failure(const char* apMsg);
//
//void
//__throw_ios_failure(const char* apMsg, int);

// Helpers for exception objects in <system_error>
void
__throw_system_error(int __v)
{

    throw SystemError(__v, std::generic_category());
}

// Helpers for exception objects in <future>
//void
//__throw_future_error(int);

// Helpers for exception objects in <functional>
//void
//__throw_bad_function_call();


} /* namespace std */

