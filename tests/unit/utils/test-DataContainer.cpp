/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <utils/DataContainer.h>

using namespace rsp::utils;

struct MyData {
    int Integer = 0;
    float Float = 0.0f;
    char String[10];
};

TEST_CASE("Data Container")
{
    DataContainer<MyData> dc;


}


