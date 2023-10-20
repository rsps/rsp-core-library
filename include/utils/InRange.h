/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_IN_RANGE_H
#define RSP_CORE_LIB_UTILS_IN_RANGE_H

#include <cmath>

namespace rsp::utils {

/**
 * \fn bool IsInRange(const T&, const T&, const T&)
 * \brief Template function to check if a value is within given range boundaries.
 *
 *  The class only uses the < operator, so any type implementing the < operator can be given as values.
 *
 * \tparam T
 * \param value
 * \param low
 * \param high
 * \return True if value is within boundaries
 */
template <typename T>
inline bool IsInRange(const T& value, const T& low, const T& high) {
    return !(value < low) && !(high < value);
}


/**
 * \fn bool IsEqual(const T&, const T&, const T&)
 * \brief Template function to compare floating point values with an epsilon.
 *
 * \tparam T
 * \param v1
 * \param v2
 * \param epsilon
 * \return True if values are equal.
 */
template <typename T>
inline bool IsEqual(const T& v1, const T& v2, const T& epsilon) {
    return (std::fabs(v1 - v2) < epsilon);
}

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_IN_RANGE_H
