/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_UTILS_ROUNDING_H_
#define RSP_UTILS_ROUNDING_H_

#include <cmath>

// Precision: 1 (all integers)
#define ROUND1(a) (std::lround(static_cast<float>(a)))

// Precision: 2 (Round to nearest even number)
#define ROUND2(a) (std::lround(static_cast<float>(a) * 0.5) * 2)

// Precision: 0.01
#define ROUND001(a) (ROUND1((a) * 100.0f) * 0.01f)

// Precision: 0.1
#define ROUND01(a) (ROUND1((a) * 10.0f) * 0.1f)

// Precision: 0.2
#define ROUND02(a) (ROUND2((a) * 10.0f) * 0.1f)


#endif /* RSP_UTILS_ROUNDING_H_ */
