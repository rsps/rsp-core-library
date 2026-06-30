/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Kim Lindberg Schwaner
 */

#include "TestHelpers.h"
#include <doctest.h>

// Test that TestHelpers::ValidateJson() actually validates JSON correctness
TEST_CASE("TestHelpers::ValidateJson")
{
    std::string_view valid_json = R"({"key":"value"})";
    CHECK(TestHelpers::ValidateJson(valid_json));

    valid_json = R"({"key":"value", "array":[1,2,3]})";
    CHECK(TestHelpers::ValidateJson(valid_json));

    std::string_view invalid_json = R"({"key":"value")"; // missing }
    CHECK_FALSE(TestHelpers::ValidateJson(invalid_json));

    invalid_json = R"({"key":value})"; // value not in quotes
    CHECK_FALSE(TestHelpers::ValidateJson(invalid_json));
}
