/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef TESTS_HELPERS_TESTPIXMAP_H_
#define TESTS_HELPERS_TESTPIXMAP_H_

#include <graphics/GfxCache.h>

class TestPixmap: public rsp::graphics::GfxCache
{
public:
    TestPixmap();
    ~TestPixmap();
};

#endif /* TESTS_HELPERS_TESTPIXMAP_H_ */
