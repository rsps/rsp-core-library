/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_JSON_JSON_EXCEPTIONS_H
#define RSP_CORE_LIB_JSON_JSON_EXCEPTIONS_H

#include <exceptions/CoreException.h>

namespace rsp::json {

/**
 * \class EJsonException
 * \brief Base class for all exceptions thrown by the json module.
 */
class EJsonException : public exceptions::CoreException {
public:
    explicit EJsonException(const std::string &aMsg) : CoreException(aMsg) {}
};

class EJsonParseError : public EJsonException {
public:
    explicit EJsonParseError(const std::string &aMsg) : EJsonException("Json Parse Error: " + aMsg) {}
};

class EJsonFormatError : public EJsonException {
public:
    explicit EJsonFormatError(const std::string &aMsg) : EJsonException("Json Format Error: " + aMsg) {}
};

class EJsonNumberError : public EJsonException {
public:
    explicit EJsonNumberError(const std::string &aMsg) : EJsonException("Json Number Error: " + aMsg) {}
};

class EJsonTypeError : public EJsonException {
public:
    explicit EJsonTypeError(const std::string &aMsg) : EJsonException("Json Type Error: " + aMsg) {}
};


} /* namespace rsp::json */

#endif // RSP_CORE_LIB_JSON_JSON_EXCEPTIONS_H
