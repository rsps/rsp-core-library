/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_NULLABLE_H
#define RSP_CORE_LIB_UTILS_NULLABLE_H

#include <exceptions/CoreException.h>

namespace rsp::utils {

/**
 * \class ENullableException
 * \brief Base class for some helpful Nullable exceptions.
 */
class ENullableException : public exceptions::CoreException {
public:
    using exceptions::CoreException::CoreException;
};

/**
 * \class ENullValueError
 * \brief Exception thrown on attempt to use a null value.
 */
class ENullValueError : public ENullableException {
public:
    explicit ENullValueError(const std::string_view &arOrigin) : ENullableException(std::string("Value is null in ") + std::string(arOrigin)) {}
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
    virtual ~Nullable() = default;

    [[nodiscard]] virtual bool IsNull() const = 0;
    virtual void Clear() = 0;
};

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_NULLABLE_H
