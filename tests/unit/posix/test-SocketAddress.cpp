/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 */

#include <rsp/posix/SocketAddress.h>
#include <doctest.h>
#include <arpa/inet.h>
#include <algorithm>
#include <string>

using namespace rsp::posix;
using namespace std::string_literals;

TEST_SUITE_BEGIN("Posix");

TEST_CASE("SocketAddress - Inet")
{
    sockaddr_in in{};
    in.sin_family = AF_INET;
    in.sin_port = htons(8080);
    inet_pton(AF_INET, "10.1.2.3", &in.sin_addr);

    SocketAddress sa(reinterpret_cast<const sockaddr&>(in), sizeof(in), Domain::Inet, Type::Stream, Protocol::Unspecified);

    CHECK_EQ(sa.GetDomain(), Domain::Inet);
    CHECK_EQ(sa.GetSize(), sizeof(sockaddr_in));
    CHECK_EQ(sa.GetPort(), in.sin_port);
    CHECK_EQ(sa.GetCanonicalName(), "10.1.2.3:8080"s);
}

TEST_CASE("SocketAddress - Inet6")
{
    sockaddr_in6 in6{};
    in6.sin6_family = AF_INET6;
    in6.sin6_port = htons(443);
    inet_pton(AF_INET6, "2001:db8::1", &in6.sin6_addr);

    SocketAddress sa(reinterpret_cast<const sockaddr&>(in6), sizeof(in6), Domain::Inet6, Type::Stream, Protocol::Unspecified);

    CHECK_EQ(sa.GetDomain(), Domain::Inet6);
    CHECK_EQ(sa.GetSize(), sizeof(sockaddr_in6));
    CHECK_EQ(sa.GetPort(), in6.sin6_port);
    CHECK_EQ(sa.GetCanonicalName(), "[2001:db8::1]:443"s);
}

TEST_CASE("SocketAddress - Unix")
{
    sockaddr_un un{};
    un.sun_family = AF_UNIX;
    const std::string path("/tmp/a-socket-path-longer-than-14-bytes");
    std::ranges::copy(path, un.sun_path);

    SocketAddress sa(reinterpret_cast<const sockaddr&>(un), sizeof(un), Domain::Unix, Type::Stream, Protocol::Unspecified);

    CHECK_EQ(sa.GetDomain(), Domain::Unix);
    CHECK_EQ(sa.GetCanonicalName(), path);
}

TEST_SUITE_END();
