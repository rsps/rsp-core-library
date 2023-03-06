/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_UTILS_NULLABLE_H_
#define RSP_UTILS_NULLABLE_H_

#include <exceptions/CoreException.h>
#include <type_traits>
#include <string>
#include <stdexcept>
#include <cmath>
#include <cstdlib>

namespace rsp::utils {

/**
 * \class ENullableException
 * \brief Base class for some helpful Nullable excetpions.
 */
class ENullableException : public exceptions::CoreException {
public:
    explicit ENullableException(const char *aMsg) : CoreException(aMsg) {}
};

/**
 * \class ENullValueError
 * \brief Exception thrown on attempt to use a nulled value.
 */
class ENullValueError : public ENullableException {
public:
    explicit ENullValueError() : ENullableException("Value is null") {}
};

/**
 * \class ETypeMismatchError
 * \brief Exception thrown if trying to assign a wrong type to a Nullable variable.
 *
 */
class ETypeMismatchError : public ENullableException {
public:
    explicit ETypeMismatchError() : ENullableException("Value types are not the same") {}
};


/**
 * \class Nullable
 * \brief Interface for Nullable types.
 *
 */
class Nullable
{
public:
    virtual ~Nullable() {};

    virtual bool IsNull() const = 0;
    virtual void Clear() = 0;
};

} /* namespace rsp::utils */

#endif /* RSP_UTILS_NULLABLE_H_ */
