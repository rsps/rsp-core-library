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

#include <string>
#include <sstream>
#include "CoreException.h"
#include "InRange.h"

namespace rsp::utils {

class ValidatorException : public CoreException
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
        :ValidatorException("Value is not in range: (")
    {
        std::stringstream ss;
        ss << aLow << " < " << aValue << " < " << aHigh << ")";
        mMsg += ss.str();
    }
};


class Validator
{
public:
    static void NotEmpty(const char* apString);
    static void NotEmpty(const std::string& arString);

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
