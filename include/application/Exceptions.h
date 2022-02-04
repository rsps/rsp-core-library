/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_APPLICATION_EXCEPTIONS_H_
#define INCLUDE_APPLICATION_EXCEPTIONS_H_

#include <exception>
#include <stdexcept>
#include <utils/ExceptionHelper.h>

namespace rsp::application {

class ApplicationException : public std::runtime_error {
public:
    explicit ApplicationException(const char* aMsg) : std::runtime_error(aMsg) {}
};

class ESingletonViolation: public ApplicationException {
public:
    explicit ESingletonViolation() : ApplicationException("Application already exist") {}
};

class ENoInstance: public ApplicationException {
public:
    explicit ENoInstance() : ApplicationException("Application has not been created") {}
};

class EMissingArgument : public std::invalid_argument {
public:
    explicit EMissingArgument() : std::invalid_argument("Missing argument") {}
};

} /* namespace rsp::application */

#endif /* INCLUDE_APPLICATION_EXCEPTIONS_H_ */
