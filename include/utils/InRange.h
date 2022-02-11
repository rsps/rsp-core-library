/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_INRANGE_H_
#define INCLUDE_UTILS_INRANGE_H_

namespace rsp::utils {

/**
 * \fn bool IsInRange(const T&, const T&, const T&)
 * \brief Template class to check if a value is within given range boundaries.
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
bool IsInRange(const T& value, const T& low, const T& high) {
    return !(value < low) && !(high < value);
}

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_INRANGE_H_ */
