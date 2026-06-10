/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <doctest.h>
#include <rsp/utils/FifoBuffer.h>

using namespace rsp::utils;

TEST_SUITE_BEGIN("Utils");

static bool Compare(std::span<char> aData, std::initializer_list<char> aList)
{
    return (std::memcmp(aData.data(), std::data(aList), aList.size()) == 0);
}

TEST_CASE("FifoBuffer")
{
    FifoBuffer<char, 10> mFifo;

    char c_data[] = {1,2,3,4,5};
    auto data = std::span(c_data);

    CHECK(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 0u);
    CHECK_EQ(mFifo.Free(), 10u);

    CHECK_EQ(mFifo.Write(data), sizeof(c_data));

    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 5u);
    CHECK_EQ(mFifo.Free(), 5u);

    CHECK_EQ(mFifo.Write(data), sizeof(c_data));

    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 10u);
    CHECK_EQ(mFifo.Free(), 0u);

    CHECK_EQ(mFifo.Write(data), 0u);

    char c_out[4]{};
    auto out = std::span(c_out);
    CHECK_EQ(mFifo.Read(out), sizeof(c_out));
    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 6u);
    CHECK_EQ(mFifo.Free(), 4u);
    CHECK(Compare(out, {1, 2, 3, 4}));

    CHECK_EQ(mFifo.Write(data), 4u);
    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 10u);
    CHECK_EQ(mFifo.Free(), 0u);

    CHECK_EQ(mFifo.Read(out), sizeof(c_out));
    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 6u);
    CHECK_EQ(mFifo.Free(), 4u);
    CHECK(Compare(out, {5, 1, 2, 3}));

    CHECK_EQ(mFifo.Read(out), 4u);
    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 2u);
    CHECK_EQ(mFifo.Free(), 8u);
    CHECK(Compare(out, {4, 5, 1, 2}));

    std::memset(c_out, 0, sizeof(c_out));
    CHECK_EQ(mFifo.Read(out), 2u);
    CHECK(Compare(out, {3, 4, 0, 0}));
    CHECK(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 0u);
    CHECK_EQ(mFifo.Free(), 10u);

    CHECK_NOTHROW(mFifo.Push('A'));
    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 1u);
    CHECK_EQ(mFifo.Free(), 9u);

    CHECK_EQ(mFifo.Pop(), 'A');
    CHECK(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 0u);
    CHECK_EQ(mFifo.Free(), 10u);

    CHECK_EQ(mFifo.Write({&data[1], 4}), 4u);

    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 4u);
    CHECK_EQ(mFifo.Free(), 6u);

    std::memset(c_out, 0, sizeof(c_out));
    CHECK_EQ(mFifo.Read(out), 4u);

    CHECK(Compare(out, {2, 3, 4, 5}));
}

TEST_SUITE_END();
