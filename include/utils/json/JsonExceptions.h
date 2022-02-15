/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_UTILS_JSON_JSONEXCEPTIONS_H_
#define INCLUDE_UTILS_JSON_JSONEXCEPTIONS_H_

#include <utils/CoreException.h>

namespace rsp::utils::json {

class EJsonException : public rsp::utils::CoreException {
public:
    explicit EJsonException(const char *apMsg) : rsp::utils::CoreException(apMsg) {}
    EJsonException(const std::string &aMsg) : rsp::utils::CoreException(aMsg) {}
};

class EJsonParseError : public EJsonException {
public:
    explicit EJsonParseError() : EJsonException("Json Parse Error") {}
};

class EJsonFormatError : public EJsonException {
public:
    explicit EJsonFormatError() : EJsonException("Json Format Error") {}
};

class EJsonNumberError : public EJsonException {
public:
    explicit EJsonNumberError() : EJsonException("Json Number Error") {}
};

class EJsonTypeError : public EJsonException {
public:
    explicit EJsonTypeError(const std::string &aMsg) : EJsonException(aMsg) {}
};

class EJsonUnicodeError : public EJsonException {
public:
    explicit EJsonUnicodeError() : EJsonException("This Json implementation does not support unicode values greater than U+00FF") {}
};

} /* namespace rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSONEXCEPTIONS_H_ */
