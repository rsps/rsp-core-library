/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 */

#include <rsp/posix/Socket.h>
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
    CHECK_EQ(sa.AsString(), "10.1.2.3"s);
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
    CHECK_EQ(sa.AsString(), "2001:db8::1"s);
    CHECK_EQ(sa.GetCanonicalName(), "[2001:db8::1]:443"s);
}

TEST_CASE("SocketAddress - Unix")
{
    sockaddr_un un{};
    un.sun_family = AF_UNIX;
    const std::string path("/tmp/a-socket-path-longer-than-14-bytes");
    std::ranges::copy(path, un.sun_path);

    SocketAddress sa(reinterpret_cast<const sockaddr&>(un), SUN_LEN(&un), Domain::Unix, Type::Stream, Protocol::Unspecified);

    CHECK_EQ(sa.GetDomain(), Domain::Unix);
    CHECK_EQ(sa.GetUnixKind(), UnixKind::Named);
    CHECK_EQ(sa.GetSize(), SUN_LEN(&un));
    CHECK_EQ(sa.AsString(), path);
    CHECK_EQ(sa.GetCanonicalName(), path);
}

TEST_CASE("SocketAddress - Unix path constructor")
{
    const std::string path("/tmp/test.sock");
    SocketAddress sa(path, Type::Datagram, Protocol::Unspecified);

    CHECK_EQ(sa.GetDomain(), Domain::Unix);
    CHECK_EQ(sa.GetType(), Type::Datagram);
    CHECK_EQ(sa.GetSize(), SUN_LEN(&reinterpret_cast<const sockaddr_un&>(sa.Get())));
    CHECK_EQ(sa.AsString(), path);
    CHECK_EQ(sa.GetCanonicalName(), path);
}

TEST_CASE("SocketAddress - Unix kinds")
{
    constexpr size_t cPathOffset = offsetof(sockaddr_un, sun_path);

    SUBCASE("Named")
    {
        SocketAddress sa("/tmp/test.sock"s, Type::Stream);
        CHECK_EQ(sa.GetUnixKind(), UnixKind::Named);
        CHECK_EQ(sa.GetCanonicalName(), "/tmp/test.sock"s);
    }

    SUBCASE("Named with terminator included in kernel reported length")
    {
        sockaddr_un un{};
        un.sun_family = AF_UNIX;
        std::ranges::copy("/tmp/a.sock"s, un.sun_path);
        SocketAddress sa(reinterpret_cast<const sockaddr&>(un), SUN_LEN(&un) + 1, Domain::Unix, Type::Stream, Protocol::Unspecified);
        CHECK_EQ(sa.GetUnixKind(), UnixKind::Named);
        CHECK_EQ(sa.AsString(), "/tmp/a.sock"s);
    }

    SUBCASE("Abstract")
    {
        SocketAddress sa = SocketAddress::Abstract("my-service", Type::Stream);
        CHECK_EQ(sa.GetDomain(), Domain::Unix);
        CHECK_EQ(sa.GetUnixKind(), UnixKind::Abstract);
        CHECK_EQ(sa.GetSize(), cPathOffset + 1 + 10);
        CHECK_EQ(sa.AsString(), "my-service"s);
        CHECK_EQ(sa.GetCanonicalName(), "@my-service"s);
    }

    SUBCASE("Abstract name may contain NUL bytes")
    {
        SocketAddress sa = SocketAddress::Abstract("a\0b"s, Type::Stream);
        CHECK_EQ(sa.GetUnixKind(), UnixKind::Abstract);
        CHECK_EQ(sa.AsString(), "a\0b"s);
    }

    SUBCASE("Anonymous")
    {
        sockaddr_un un{};
        un.sun_family = AF_UNIX;
        SocketAddress sa(reinterpret_cast<const sockaddr&>(un), cPathOffset, Domain::Unix, Type::Stream, Protocol::Unspecified);
        CHECK_EQ(sa.GetUnixKind(), UnixKind::Anonymous);
        CHECK_EQ(sa.GetSize(), cPathOffset);
        CHECK(sa.AsString().empty());
        CHECK_EQ(sa.GetCanonicalName(), "<unnamed>"s);
    }

    SUBCASE("Not a Unix address")
    {
        SocketAddress sa;
        CHECK_EQ(sa.GetUnixKind(), UnixKind::None);
        CHECK_EQ(sa.GetSize(), 0u);
    }
}

TEST_CASE("SocketAddress - Unix path constructor length limit")
{
    constexpr size_t cMaxLen = sizeof(sockaddr_un::sun_path) - 1; // leave room for null terminator

    SUBCASE("Longest path leaving room for terminator is accepted")
    {
        const std::string path(cMaxLen, 'a');
        SocketAddress sa(path, Type::Stream);
        CHECK_EQ(sa.AsString(), path);
    }

    SUBCASE("Path filling the whole buffer is rejected")
    {
        const std::string path(cMaxLen + 1, 'a');
        CHECK_THROWS_AS(SocketAddress(path, Type::Stream), ESocketError);
    }

    SUBCASE("Path exceeding the buffer is rejected")
    {
        const std::string path(cMaxLen + 10, 'a');
        CHECK_THROWS_AS(SocketAddress(path, Type::Stream), ESocketError);
    }
}

TEST_SUITE_END();
