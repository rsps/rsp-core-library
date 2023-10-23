/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef TESTS_HELPERS_TEST_HELPERS_H_
#define TESTS_HELPERS_TEST_HELPERS_H_

#include <string>
#include <utils/StrUtils.h>
#include "TestLogger.h"

#define CHECK_HEX(a, b) CHECK_MESSAGE((a == b), rsp::utils::StrUtils::Format( "%8.8X != %8.8X" , a, b))

class TestHelpers
{
public:
    static std::uint8_t TamperWithFile(const std::string& arFileName, std::uint32_t aOffset, std::uint8_t aValue);

    static void ParseArguments(const char ** apArgv);

    static std::string ToHex(const std::string &arString);
    static std::string ToHex(std::uint32_t aValue);
//    static std::string ToHex(const std::uint8_t *apData, std::uint32_t aSize);
    static std::string ToHex(const uint8_t *apData, std::uint32_t aSize, std::uint32_t aSizeOf);

    static bool ValidateJson(const std::string &arJson);
    static bool ValidateJsonFile(const std::string &arJsonFile);
};

template <class T>
std::string ToHex(const T* apData, std::uint32_t aSize)
{
    return TestHelpers::ToHex(reinterpret_cast<const uint8_t*>(apData), aSize, sizeof(T));
}

#endif /* TESTS_HELPERS_TEST_HELPERS_H_ */
