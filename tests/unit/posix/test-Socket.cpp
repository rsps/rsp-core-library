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
#include <posix/SocketAddress.h>
#include <logging/LoggerInterface.h>
#include <logging/ConsoleLogWriter.h>

using namespace rsp::posix;

TEST_CASE("Socket")
{
    auto logger = rsp::logging::LoggerInterface::GetDefault();
    auto handle = logger->MakeLogWriter<rsp::logging::ConsoleLogWriter>(rsp::logging::LogLevel::Debug);

    SUBCASE("Address") {
        CHECK_NOTHROW(SocketAddress socket_path("/tmp/test-sock", Type::Stream));
        CHECK_NOTHROW(AddressInfo info("/tmp/test-sock"));
    }

    SUBCASE("Construct") {
        CHECK_NOTHROW(Socket s);
        CHECK_NOTHROW(Socket s(Domain::Unix, Type::Stream, Protocol::Unspecified));
    }

    SUBCASE("Unix Socket") {
        const std::string msg("Hello Client");
        std::string socket_path("/tmp/test-sock");
        AddressInfo info(socket_path);

        Socket server(Domain::Unix, Type::Stream, Protocol::Unspecified);
        CHECK_NOTHROW(server.Bind(info));
        CHECK_NOTHROW(server.Listen(2));

        Socket client(Domain::Unix, Type::Stream, Protocol::Unspecified);
        CHECK_NOTHROW(client.Connect(info));
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

    SUBCASE("TCP Socket") {
        const std::string msg("Hello Client");
        std::string socket_path("localhost:46555");
        AddressInfo info(socket_path, true, Domain::Unspecified, Type::Stream);

        Socket server(Domain::Inet, Type::Stream);
        CHECK_NOTHROW(server.Bind(info));
        CHECK_NOTHROW(server.Listen(2));

        Socket client(Domain::Inet, Type::Stream);
        CHECK_NOTHROW(client.Connect(info));
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

        CHECK_NOTHROW();
    }

}
