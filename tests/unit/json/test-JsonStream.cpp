/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 */

#include <rsp/json/JsonStream.h>

#include <doctest.h>

#include <array>
#include <cstring>
#include <string>

using namespace rsp::json;

TEST_SUITE_BEGIN("Json");

TEST_CASE("JsonStream SetBuffer")
{
    constexpr char cSentinel = '#';
    const std::string cExpected(R"({"a":1,"b":"text"})");

    SUBCASE("Output is written to the supplied buffer")
    {
        std::array<char, 64> buffer;
        buffer.fill(cSentinel);

        JsonStream stream;
        stream.SetBuffer(buffer.data(), buffer.size());
        stream << OBegin() << Key("a") << Raw("1") << Comma() << Key("b") << "text" << OEnd();

        REQUIRE_EQ(stream.GetSize(), cExpected.size());
        CHECK_EQ(std::string(buffer.data(), stream.GetSize()), cExpected);
        CHECK_EQ(buffer[stream.GetSize()], cSentinel);
    }

    SUBCASE("Size accounting")
    {
        std::array<char, 64> buffer{};

        JsonStream stream;
        stream.SetBuffer(buffer.data(), buffer.size());
        CHECK_EQ(stream.GetMaxSize(), buffer.size());
        CHECK_EQ(stream.GetSize(), 0u);
        CHECK_EQ(stream.GetAvailable(), buffer.size());

        stream << OBegin() << Key("a") << Raw("1") << OEnd();
        CHECK_EQ(stream.GetSize(), 7u); // {"a":1}
        CHECK_EQ(stream.GetAvailable(), buffer.size() - 7u);
    }
}

TEST_SUITE_END();
