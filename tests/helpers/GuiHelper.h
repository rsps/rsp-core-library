/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef TESTS_HELPERS_GUIHELPER_H_
#define TESTS_HELPERS_GUIHELPER_H_

#include "TestHelpers.h"
#include "TestPixmap.h"
#include "TestTouchParser.h"

class GuiHelper
{
public:
    TestLogger mLogger;
    TestTouchParser mTouchParser;
    TestPixmap mPixMap;

    GuiHelper(bool aRegisterTestTouchParser = true);
    virtual ~GuiHelper();
};

#endif /* TESTS_HELPERS_GUIHELPER_H_ */
