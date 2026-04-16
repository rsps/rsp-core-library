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

TEST_SUITE_BEGIN("Posix");

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
        CHECK_EQ(sc.Send(msg), msg.size());
        CHECK(client.IsDataReady());

        std::string result(20, 'A');
        size_t len;
        CHECK_NOTHROW(len = client.Receive(result));
        CHECK_NOTHROW(result.resize(len));
        MESSAGE(result);
        CHECK_EQ(result, msg);
    }

    SUBCASE("TCP Socket") {
        const std::string hello_client("Hello TCP Client");
        const std::string hello_server("Hello TCP Server");
        std::string socket_path("localhost:46555");

        AddressInfo info(socket_path, true, Domain::Unspecified, Type::Stream);
        CHECK_GE(info.GetCount(), 1u);

        Socket server(Domain::Inet, Type::Stream);
        CHECK_NOTHROW(server.SetOptions(SockOptions::ReUseAddress, 1));
        CHECK_NOTHROW(server.Bind(info));
        CHECK_NOTHROW(server.Listen(2));

        Socket client(Domain::Inet, Type::Stream);
        CHECK_NOTHROW(client.Connect(info));
        Socket sc;
        CHECK_NOTHROW(sc = server.Accept());
        CHECK_FALSE(client.IsDataReady());
        CHECK_EQ(sc.Send(hello_client), hello_client.size());
        CHECK(client.IsDataReady());

        std::string result(32, 'A');
        size_t len;
        CHECK_NOTHROW(len = client.Receive(result));
        CHECK_NOTHROW(result.resize(len));
        MESSAGE(result);
        CHECK_EQ(result, hello_client);

        CHECK_FALSE(sc.IsDataReady());
        CHECK_EQ(client.Send(hello_server), hello_server.size());
        CHECK(sc.IsDataReady());
        result.clear();
        result.resize(32);
        CHECK_NOTHROW(len = sc.Receive(result));
        CHECK_NOTHROW(result.resize(len));
        MESSAGE(result);
        CHECK_EQ(result, hello_server);
    }

    SUBCASE("UDP Socket") {
        const std::string hello_client("Hello UDP Client");
        const std::string hello_server("Hello UDP Server");
        std::string socket_path("localhost:46555");

        AddressInfo info(socket_path, true, Domain::Unspecified, Type::Stream);
        CHECK_GE(info.GetCount(), 1u);

        Socket server(Domain::Inet, Type::Datagram);
        CHECK_NOTHROW(server.SetOptions(SockOptions::ReUseAddress, 1));
        CHECK_NOTHROW(server.Bind(info));
        CHECK_FALSE(server.IsDataReady());

        Socket client(Domain::Inet, Type::Datagram);
        CHECK_NOTHROW(client.Connect(info));
        CHECK_FALSE(client.IsDataReady());
        CHECK_EQ(client.Send(hello_server), hello_server.size());

        std::string result(32, 'A');
        size_t len;
        Socket peer;
        CHECK(server.IsDataReady());
        CHECK_NOTHROW(len = server.ReceiveFrom(peer, result));
        CHECK_NOTHROW(result.resize(len));
        MESSAGE(result);
        CHECK_EQ(result, hello_server);

        CHECK_EQ(server.SendTo(peer, hello_client), hello_client.size());
        CHECK(client.IsDataReady());
        result.clear();
        result.resize(32);
        CHECK_NOTHROW(len = client.Receive(result));
        CHECK_NOTHROW(result.resize(len));
        MESSAGE(result);
        CHECK_EQ(result, hello_client);
    }

}

TEST_SUITE_END();
