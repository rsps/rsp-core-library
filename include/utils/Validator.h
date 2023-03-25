/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_VALIDATOR_H_
#define INCLUDE_UTILS_VALIDATOR_H_

#include <exceptions/CoreException.h>
#include <string>
#include <sstream>
#include "InRange.h"

namespace rsp::utils {

/**
 * \brief Base class for validator exceptions.
 */
class ValidatorException : public exceptions::CoreException
{
public:
    using CoreException::CoreException;
};

class ENotEmpty : public ValidatorException
{
public:
    ENotEmpty() : ValidatorException("Value is not empty") {}
};

class ENotInRange : public ValidatorException
{
public:
    ENotInRange() : ValidatorException("Value is not in range") {}

    template <typename T>
    ENotInRange(T aLow, T aHigh, T aValue)
        :ValidatorException("Value is not in range: " + format(aLow, aHigh, aValue))
    {

    }

    template <typename T>
    std::string format(T aLow, T aHigh, T aValue)
    {
        std::stringstream ss;
        ss << "(" << aLow << " < " << aValue << " < " << aHigh << ")";
        return ss.str();
    }
};

/**
 * \brief Simple class with static methods that can be used for data validation.
 *
 * Every validator function throws an exception if given value is not valid.
 */
class Validator
{
public:
    /**
     * \brief Test if a string is not empty.
     * \param apString
     */
    static void NotEmpty(const char* apString);
    static void NotEmpty(const std::string& arString);

    /**
     * \brief Test is a given value is within the given range.
     * \tparam T Type of the value to test
     * \param aLow Low limit
     * \param aHigh High limit
     * \param aValue Value to test
     */
    template <typename T>
    static void InRange(T aLow, T aHigh, T aValue)
    {
        if (!IsInRange(aValue, aLow, aHigh)) {
            THROW_WITH_BACKTRACE3(ENotInRange, aLow, aHigh, aValue);
        }
    }
};

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_VALIDATOR_H_ */
