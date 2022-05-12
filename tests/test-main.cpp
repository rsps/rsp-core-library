/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include <TestHelpers.h>

int main(int argc, const char** argv)
{
    TestHelpers::ParseArguments(argv);
    doctest::Context context(argc, argv);
    int test_result = context.run(); // run queries, or run tests unless --no-run

    if(context.shouldExit()) // honor query flags and --exit
        return test_result;

    return test_result;
}
