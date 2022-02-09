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

#include <type_traits>
#include <string>
#include <stdexcept>
#include <cmath>
#include <cstdlib>
#include <utils/CoreException.h>

namespace rsp::utils {

class ENullableException : public CoreException {
public:
    explicit ENullableException(const char *aMsg) : CoreException(aMsg) {}
};

class ENullValueError : public ENullableException {
public:
    explicit ENullValueError() : ENullableException("Value is null") {}
};

class ETypeMismatchError : public ENullableException {
public:
    explicit ETypeMismatchError() : ENullableException("Value types are not the same") {}
};


class Nullable
{
public:
    virtual ~Nullable() {};

    virtual bool IsNull() const = 0;
    virtual void Clear() = 0;
};

} /* namespace rsp::utils */

#endif /* RSP_UTILS_NULLABLE_H_ */
