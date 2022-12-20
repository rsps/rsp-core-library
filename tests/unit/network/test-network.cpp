/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Jesper Madsen, Steffen Brummer
 */

#include <doctest.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <logging/Logger.h>
#include <network/IHttpRequest.h>
#include <network/HttpRequest.h>
#include <network/HttpDownload.h>
#include <network/NetworkLibrary.h>
#include <network/HttpSession.h>
#include <network/NetworkException.h>
#include <posix/FileSystem.h>
#include <posix/FileIO.h>
#include <utils/AnsiEscapeCodes.h>
#include <utils/StrUtils.h>
#include <TestHelpers.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>

using namespace rsp::logging;
using namespace rsp::network;
using namespace rsp::utils;
using namespace rsp::utils::AnsiEscapeCodes;
using namespace rsp::posix;

TEST_CASE("Network")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    HttpRequestOptions opt;
    opt.CertCaPath = "webserver/ssl/ca/ca.crt";
    opt.CertPath = "webserver/ssl/certs/SN1234.crt";
    opt.KeyPath = "webserver/ssl/private/SN1234.key";

    // Run lighttpd directly from build directory, no need to install it.
    std::system("_deps/lighttpd_src-build/build/lighttpd -f webserver/lighttpd.conf -m _deps/lighttpd_src-build/build");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    SUBCASE("Library Version"){
        CHECK_EQ(NetworkLibrary::Get().GetLibraryName(), "libcurl");
        CHECK_GE(NetworkLibrary::Get().GetVersion(), "7.68.0");
//        MESSAGE("Network Library: " << NetworkLibrary::Get().GetLibraryName());
//        MESSAGE("Network Library Version: " << NetworkLibrary::Get().GetVersion());
    }

    SUBCASE("Online") {
        std::string ip;
        CHECK_NOTHROW(ip = FileSystem::GetCurrentIpAddress());
        MESSAGE("IP: " << ip);
        std::vector<std::string> list;
        CHECK_EQ(StrUtils::Split(ip, list, '.', false), 4);
    }

    SUBCASE("TLS to localhost") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:8443";

        SUBCASE("HEAD") {
            opt.RequestType = HttpRequestType::HEAD;
        }
        SUBCASE("GET") {
            opt.RequestType = HttpRequestType::GET;
        }

        request.SetOptions(opt);

        IHttpResponse *resp = nullptr;
        CHECK_NOTHROW(resp = &request.Execute());

//        MESSAGE("Request:\n" << resp->GetRequest());
//        MESSAGE("Response:\n" << *resp);

        CHECK_EQ(resp->GetHeader("content-type"), "text/html");

        if (opt.RequestType == HttpRequestType::HEAD) {
            CHECK_EQ(resp->GetBody().size(), 0);
        }
        else {
            CHECK_EQ(resp->GetBody().size(), 120);
        }

        CHECK_EQ(resp->GetStatusCode(), 200);
    }

    SUBCASE("Invalid Client") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";
        opt.CertPath = "";
        opt.KeyPath = "";

        request.SetOptions(opt);

        IHttpResponse *resp = nullptr;
        CHECK_THROWS_AS(resp = &request.Execute(), NetworkException);
        CHECK_THROWS_WITH_AS(resp = &request.Execute(), doctest::Contains("curl_easy_perform() failed. (56) Failure when receiving data from the peer"), NetworkException);
    }

    SUBCASE("Validated Client") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";

        request.SetOptions(opt);

        IHttpResponse *resp = nullptr;
        CHECK_NOTHROW(resp = &request.Execute());

//        MESSAGE("Request:\n" << resp->GetRequest());
//        MESSAGE("Response:\n" << *resp);

        CHECK_EQ(resp->GetHeader("content-type"), "text/html");
        CHECK_EQ(resp->GetBody().size(), 120);
        CHECK_EQ(resp->GetStatusCode(), 200);
    }

    SUBCASE("File Download") {
        const std::string cFile("./image.png");
        const std::string cSourceFile("./webserver/public/image.png");

        FileIO file(cSourceFile, std::ios_base::in);
        auto source = file.GetContents();

        HttpDownload request(cFile);
        opt.BaseUrl = "https://server.localhost:44300/image.png";
//        opt.Verbose = 1;

        request.SetOptions(opt);

        IHttpResponse *resp = nullptr;

        SUBCASE("To Memory") {
            FileSystem::DeleteFile(std::string(cFile));
            request.SetFileName("");

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().size(), source.size());
            CHECK_EQ(resp->GetStatusCode(), 200);
            CHECK(std::memcmp(source.data(), resp->GetBody().data(), source.size()) == 0);
        }

        SUBCASE("To File") {
            FileSystem::DeleteFile(std::string(cFile));

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().size(), 0);
            CHECK_EQ(resp->GetStatusCode(), 206);
        }

        SUBCASE("Partial To File") {
            truncate(cFile.c_str(), 20*1024); // This changes mtime

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().size(), 0);
            CHECK_EQ(resp->GetStatusCode(), 206);
        }

        SUBCASE("Unmodified To File") {
            using namespace std::literals::chrono_literals;

            opt.Verbose = 1;
            request.SetOptions(opt);

            auto mtime = FileSystem::GetFileModifiedTime(cFile);
            truncate(cFile.c_str(), 20*1024); // This changes mtime
            FileSystem::SetFileModifiedTime(cFile, mtime - 8h); // TODO: This line should fail with 412, lighttpd does not
//            FileSystem::SetFileModifiedTime(cFile, mtime + 2h);

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().size(), 0);
            CHECK_EQ(resp->GetStatusCode(), 206);
        }

        CHECK_EQ(resp->GetHeader("content-type"), "image/png");

//        MESSAGE("Request:\n" << resp->GetRequest());
//        MESSAGE("Response:\n" << *resp);

        if (FileSystem::FileExists(cFile)) {
            CHECK_EQ(FileSystem::GetFileModifiedTime(cFile), FileSystem::GetFileModifiedTime(cSourceFile));
            FileIO file2(cFile, std::ios_base::in);
            auto s2 = file2.GetContents();
            CHECK_EQ(s2.size(), source.size());
            CHECK(std::memcmp(source.data(), s2.data(), source.size()) == 0);
        }
    }

    SUBCASE("File Upload") {
        const char* cUploadedFile = "./webserver/upload.png";
        const char* cSourceFile = "./webserver/public/image.png";

        FileIO file(cSourceFile, std::ios_base::in);
        auto source = file.GetContents();
        file.Seek(0);

        opt.BaseUrl = "https://server.localhost:44300/cgi/upload.sh";
        opt.RequestType = HttpRequestType::POST;
        opt.ReadFile = &file;
//        opt.Verbose = 1;

        HttpRequest request;
        request.SetOptions(opt);

        IHttpResponse *resp = nullptr;
        CHECK_NOTHROW(resp = &request.Execute());

//        MESSAGE(resp->GetBody());

        CHECK_EQ(resp->GetBody().size(), 48);
        CHECK_EQ(resp->GetStatusCode(), 200);

        CHECK(FileSystem::FileExists(cUploadedFile));
        FileIO file2(cUploadedFile, std::ios_base::in);
        auto s2 = file2.GetContents();
        CHECK_EQ(s2.size(), source.size());
        CHECK(std::memcmp(source.data(), s2.data(), source.size()) == 0);

        FileSystem::DeleteFile(std::string(cUploadedFile));
    }

    SUBCASE("Form Upload") {
        const char* cUploadedFile = "./webserver/upload.png";
        const char* cSourceFile = "./webserver/public/image.png";

        FileIO file(cSourceFile, std::ios_base::in);
        auto source = file.GetContents();
        file.Seek(0);

        opt.BaseUrl = "https://server.localhost:44300/cgi/upload-form.sh";
        opt.RequestType = HttpRequestType::POST;
//        opt.Verbose = 1;

        HttpRequest request;
        request.SetOptions(opt);
        request.AddField("filename", "uploaded.png");
        request.AddFile("filedata", file);

        IHttpResponse *resp = nullptr;
        CHECK_NOTHROW(resp = &request.Execute());

        auto body = resp->GetBody();
//        MESSAGE(body);

        std::string expected = "\n"
            "Uploaded file size: 25437\n"
            "CTYPE: multipart/form-data\n"
            "filename: uploaded.png\r\n"
            "filedata: filename=\"image.png\"; Content-Type: image/png\r\n";

//        std::cout << TestHelpers::ToHex(body) << std::endl;
//        std::cout << TestHelpers::ToHex(expected) << std::endl;

        CHECK_EQ(body, expected);

        CHECK_EQ(resp->GetBody().size(), 135);
        CHECK_EQ(resp->GetStatusCode(), 200);

        CHECK(FileSystem::FileExists(cUploadedFile));
        FileIO file2(cUploadedFile, std::ios_base::in);
        auto s2 = file2.GetContents();
        CHECK_EQ(s2.size(), source.size());
        CHECK(std::memcmp(source.data(), s2.data(), source.size()) == 0);

        FileSystem::DeleteFile(std::string(cUploadedFile));
    }

    SUBCASE("Http Session") {
        CHECK_NOTHROW(HttpSession session1(1));
        bool respHead = false;
        bool resp1 = false;
        bool resp2 = false;
        HttpSession session(5);

        opt.BaseUrl = "https://server.localhost:44300/";
//        opt.Verbose = 1;
        session.SetDefaultOptions(opt);

        session.Head("index.html",
            [&respHead](IHttpResponse& resp) {
//                MESSAGE("Response Head:\n" << resp);
                CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                CHECK_EQ(resp.GetHeaders().at("content-length"), "120");
                CHECK_EQ(resp.GetHeaders().at("http/2 200"), "present");
                CHECK_EQ(resp.GetBody().size(), 0);
                CHECK_EQ(resp.GetStatusCode(), 200);
                respHead = true;
            });
        CHECK_NOTHROW(session.ProcessRequests());

        session.Get("index.html",
            [&resp1](IHttpResponse& resp) {
//                MESSAGE("Response 1:\n" << resp);
                CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                CHECK_EQ(resp.GetHeaders().at("content-length"), "120");
                CHECK_EQ(resp.GetHeaders().at("http/2 200"), "present");
                CHECK_EQ(resp.GetBody().size(), 120);
                CHECK_EQ(200, resp.GetStatusCode());
                resp1 = true;
            });
        session.Get("image.png",
            [&resp2](IHttpResponse& resp) {
//                MESSAGE("Response 2:\n" << resp);
                CHECK_EQ(resp.GetHeaders().at("content-type"), "image/png");
                CHECK_EQ(resp.GetHeaders().at("content-length"), "25138");
                CHECK_EQ(resp.GetHeaders().at("http/2 200"), "present");
                CHECK_EQ(resp.GetBody().size(), 25138);
                CHECK_EQ(200, resp.GetStatusCode());
                resp2 = true;
            });
        CHECK_NOTHROW(session.ProcessRequests());

        CHECK(respHead);
        CHECK(resp1);
        CHECK(resp2);
    }

    std::system("killall lighttpd");
}
