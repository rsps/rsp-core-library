/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_JSON_JSONEXCEPTIONS_H_
#define INCLUDE_JSON_JSONEXCEPTIONS_H_

#include <utils/CoreException.h>

namespace rsp::json {

/**
 * \class EJsonException
 * \brief Base class for all exceptions thrown by the json module.
 */
class EJsonException : public rsp::utils::CoreException {
public:
    explicit EJsonException(const std::string &aMsg) : rsp::utils::CoreException(aMsg) {}
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

class EMemberNotExisting: public EJsonException {
public:
    explicit EMemberNotExisting(const std::string &arName) : EJsonException("Json object does not have a member named: " + arName) {}
};


} /* namespace rsp::json */

#endif /* INCLUDE_JSON_JSONEXCEPTIONS_H_ */
