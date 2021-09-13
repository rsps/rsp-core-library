/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "Pen.h"

#include "Canvas.h"

Pen::Pen(int aSize, Colour aColour)
    : size(aSize), colour(aColour) {
}
void Pen::Draw(Canvas &aCanvas, const Point &aPoint) const {
    aCanvas.SetPixel(aPoint, colour);
}