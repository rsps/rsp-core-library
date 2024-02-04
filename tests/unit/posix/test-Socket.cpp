/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <doctest.h>
#include <posix/Socket.h>

using namespace rsp::posix;

TEST_CASE("Socket")
{
    SUBCASE("SocketAddress") {
        CHECK_THROWS_AS(Socket::SocketAddress socket_path("/tmp/test-socket"), ESocketError);
        CHECK_NOTHROW(Socket::SocketAddress socket_path("/tmp/test-sock"));
    }

    SUBCASE("Construct") {
        CHECK_NOTHROW(Socket s);
        CHECK_NOTHROW(Socket s(Socket::Domain::Unix, Socket::Type::Stream, Socket::Protocol::Unix));
    }

    SUBCASE("Unix Socket") {
        const std::string msg("Hello Client");
        std::string socket_path("/tmp/test-sock");

        Socket server(Socket::Domain::Unix, Socket::Type::Stream, Socket::Protocol::Unix);
        CHECK_NOTHROW(server.Bind(socket_path));
        CHECK_NOTHROW(server.Listen(2));

        Socket client(Socket::Domain::Unix, Socket::Type::Stream, Socket::Protocol::Unix);
        CHECK_NOTHROW(client.Connect(socket_path));
        Socket sc;
        CHECK_NOTHROW(sc = server.Accept());
        CHECK_FALSE(client.IsDataReady());
        CHECK_NOTHROW(sc.Send(reinterpret_cast<const std::uint8_t*>(msg.data()), msg.size()));
        CHECK(client.IsDataReady());

        std::string result(20, 'A');
        size_t len;
        CHECK_NOTHROW(len = client.Receive(reinterpret_cast<std::uint8_t*>(result.data()), result.size()));
        CHECK_NOTHROW(result.resize(len));
        MESSAGE(result);
        CHECK_EQ(result, msg);
    }
}
