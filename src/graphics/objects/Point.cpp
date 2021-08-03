/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "Point.h"

Point::Point()
    : x(0), y(0) {
}
Point::Point(int aX, int aY)
    : x(aX), y(aY) {
}
Point::Point(const Point &aPoint)
    : x(aPoint.x), y(aPoint.y) {
}
