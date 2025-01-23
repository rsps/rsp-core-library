/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_OPTIONAL_PTR_H
#define RSP_CORE_LIB_UTILS_OPTIONAL_PTR_H

namespace rsp::utils {

/**
 * \class OptionalPtr
 * \brief Helper to support optional pointers or references
 *
 * \tparam T
 */
template <class T>
class OptionalPtr
{
public:
    OptionalPtr(const T* apValue = nullptr) : mPtr(apValue) {} // NOLINT, Conversion constructor
    OptionalPtr(const T &arValue) : mPtr(&arValue) {} // NOLINT, Conversion constructor
    explicit operator bool() const { return (mPtr); }
    const T& operator->() const { return *mPtr; }
    const T& operator*() const { return *mPtr; }
    const T& Get() const { return *mPtr; }
protected:
    const T* mPtr;
};

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_OPTIONAL_PTR_H
