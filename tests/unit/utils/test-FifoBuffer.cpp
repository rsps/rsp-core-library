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
#include <utils/FifoBuffer.h>

using namespace rsp::utils;

TEST_CASE("FifoBuffer")
{
    FifoBuffer<char, 10> mFifo;

    char data[] = {1,2,3,4,5};

    CHECK(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 0);
    CHECK_EQ(mFifo.Free(), 10);

    CHECK_EQ(mFifo.Write(data, sizeof(data)), sizeof(data));

    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 5);
    CHECK_EQ(mFifo.Free(), 5);

    CHECK_EQ(mFifo.Write(data, sizeof(data)), sizeof(data));

    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 10);
    CHECK_EQ(mFifo.Free(), 0);

    CHECK_EQ(mFifo.Write(data, sizeof(data)), 0);

    char out[4]{};
    char expected[4] = {1, 2, 3, 4};
    CHECK_EQ(mFifo.Read(out, sizeof(out)), sizeof(out));

    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 6);
    CHECK_EQ(mFifo.Free(), 4);
    CHECK_EQ(std::memcmp(out, expected, sizeof(out)), 0);

    CHECK_EQ(mFifo.Read(out, sizeof(out)), sizeof(out));

    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 2);
    CHECK_EQ(mFifo.Free(), 8);

    char expected2[4] = {5, 1, 2, 3};
    CHECK_EQ(std::memcmp(out, expected2, 4), 0);

    std::memset(out, 0, sizeof(out));
    CHECK_EQ(mFifo.Read(out, sizeof(out)), 2);

    CHECK(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 0);
    CHECK_EQ(mFifo.Free(), 10);

    CHECK_NOTHROW(mFifo.Push('A'));
    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 1);
    CHECK_EQ(mFifo.Free(), 9);

    CHECK_EQ(mFifo.Pop(), 'A');
    CHECK(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 0);
    CHECK_EQ(mFifo.Free(), 10);

    char expected3[4] = {4, 5, 0, 0};
    CHECK_EQ(std::memcmp(out, expected3, 4), 0);

    CHECK_EQ(mFifo.Write(&data[1], 4), 4);

    CHECK_FALSE(mFifo.IsEmpty());
    CHECK_EQ(mFifo.Used(), 4);
    CHECK_EQ(mFifo.Free(), 6);

    std::memset(out, 0, sizeof(out));
    CHECK_EQ(mFifo.Read(out, sizeof(out)), 4);

    char expected4[4] = {2, 3, 4, 5};
    CHECK_EQ(std::memcmp(out, expected4, 4), 0);
}