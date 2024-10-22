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
#include <chrono>
#include <filesystem>
#include <network/IHttpRequest.h>
#include <network/HttpRequest.h>
#include <network/HttpDownload.h>
#include <network/NetworkLibrary.h>
#include <network/HttpSession.h>
#include <network/HttpStringBody.h>
#include <network/NetworkException.h>
#include <posix/FileSystem.h>
#include <posix/FileIO.h>
#include <utils/StrUtils.h>
#include <TestHelpers.h>
#include <cstdlib>
#include <unistd.h>

using namespace rsp::logging;
using namespace rsp::network;
using namespace rsp::utils;
using namespace rsp::posix;

TEST_CASE("Network")
{
    TestLogger logger;

    HttpRequestOptions opt;
//    opt.Body = std::make_shared<HttpStringBody>();
    opt.CertCaPath = "webserver/ssl/ca/ca.crt";
    opt.CertPath = "webserver/ssl/certs/SN1234.crt";
    opt.KeyPath = "webserver/ssl/private/SN1234.key";

    // Run lighttpd directly from build directory, no need to install it.
    std::string cwd = std::filesystem::current_path();
    std::string command = cwd + "/_deps/lighttpd_src-build/build/lighttpd -f " + cwd + "/webserver/lighttpd.conf -m " + cwd + "/_deps/lighttpd_src-build/build";
    CHECK_EQ(0, std::system(command.c_str()));
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

        logger.Info() << "Request:\n" << request << std::endl;

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

        logger.Info() << "Response:\n" << *resp << std::endl;

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

        CHECK_THROWS_AS(auto *resp = &request.Execute(), NetworkException);
        CHECK_THROWS_WITH_AS(auto *resp = &request.Execute(), doctest::Contains(" (56) Failure when receiving data from the peer"), NetworkException);
    }

    SUBCASE("Validated Client") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";

        request.SetOptions(opt);

        IHttpResponse *resp;
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
            FileSystem::DeleteFile(cFile);
            request.SetFileName("");

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().size(), source.size());
            CHECK_EQ(resp->GetStatusCode(), 200);
            CHECK_EQ(std::memcmp(source.data(), resp->GetBody().data(), source.size()), 0);
        }

        SUBCASE("To File") {
            FileSystem::DeleteFile(cFile);

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().size(), 0);
            CHECK_EQ(resp->GetStatusCode(), 206);
        }

        SUBCASE("Partial To File") {
            CHECK_EQ(0, truncate(cFile.c_str(), 20*1024)); // This changes mtime

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().size(), 0);
            CHECK_EQ(resp->GetStatusCode(), 206);
        }

        SUBCASE("Unmodified To File") {
            using namespace std::literals::chrono_literals;

            auto mtime = FileSystem::GetFileModifiedTime(cFile);
            CHECK_EQ(0, truncate(cFile.c_str(), 20*1024)); // This changes mtime
            // This line will work, as the result is the partial data from an unmodified file.
            FileSystem::SetFileModifiedTime(cFile, mtime + 2h);

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().size(), 0);
            CHECK_EQ(resp->GetStatusCode(), 206);
        }

        SUBCASE("Modified To Fail") {
            using namespace std::literals::chrono_literals;

//            opt.Verbose = 1;
//            request.SetOptions(opt);

            auto mtime = FileSystem::GetFileModifiedTime(cFile);
            CHECK_EQ(0, truncate(cFile.c_str(), 20*1024)); // This changes mtime
            // FIXME: This line should fail with a "412 Precondition Failed", lighttpd does not send the correct result.
            FileSystem::SetFileModifiedTime(cFile, mtime - 2h);

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().size(), 0);
//            CHECK_EQ(resp->GetStatusCode(), 412); // This is the correct result.
            CHECK_EQ(resp->GetStatusCode(), 206); // FIXME: This is the wrong result!!!
            MESSAGE("Warning: This test should currently fail!!! It passes because of a bug in lighttpd.");
        }

        CHECK_EQ(resp->GetHeader("content-type"), "image/png");

//        MESSAGE("Request:\n" << resp->GetRequest());
//        MESSAGE("Response:\n" << *resp);

        if (FileSystem::FileExists(cFile)) {
            CHECK_EQ(FileSystem::GetFileModifiedTime(cFile), FileSystem::GetFileModifiedTime(cSourceFile));
            FileIO file2(cFile, std::ios_base::in);
            auto s2 = file2.GetContents();
            CHECK_EQ(s2.size(), source.size());
            CHECK_EQ(std::memcmp(source.data(), s2.data(), source.size()), 0);
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

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

//        MESSAGE(resp->GetBody());

        CHECK_EQ(resp->GetBody().size(), 48);
        CHECK_EQ(resp->GetStatusCode(), 200);

        CHECK(FileSystem::FileExists(cUploadedFile));
        FileIO file2(cUploadedFile, std::ios_base::in);
        auto s2 = file2.GetContents();
        CHECK_EQ(s2.size(), source.size());
        CHECK_EQ(std::memcmp(source.data(), s2.data(), source.size()), 0);

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

        IHttpResponse *resp;
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
        CHECK_EQ(std::memcmp(source.data(), s2.data(), source.size()), 0);

        FileSystem::DeleteFile(std::string(cUploadedFile));
    }

    SUBCASE("Body Stream") {
        HttpStringBody body(R"(
              O freddled gruntbuggly thy micturations are to me
                 As plured gabbleblochits on a lurgid bee.
              Groop, I implore thee my foonting turlingdromes.
           And hooptiously drangle me with crinkly bindlewurdles,
Or I will rend thee in the gobberwarts with my blurlecruncheon, see if I don't.
)");

//        MESSAGE(body.GetString(100));
        CHECK_EQ(body.GetString(100), body.Get().substr(0, 97) + "...");

        char buffer[51];
        size_t written;
        size_t chunk_index = 0;
        size_t payload_index = 0;
        bool eof = false;
        while (!eof) {
            eof = body.GetChunk(buffer, sizeof(buffer) - 1, written, chunk_index, payload_index);
            buffer[written] = '\0';
//            MESSAGE("\nChunk:   " << buffer << "\nwritten: " << written << "\nindex:   " << chunk_index);
        }
        CHECK_EQ(written, 33);
        CHECK_EQ(chunk_index, 333);
        CHECK_EQ("blurlecruncheon, see if I don't.\n", std::string(buffer));
    }

    SUBCASE("Post JSON") {
        opt.BaseUrl = "https://server.localhost:44300/cgi/post.sh";
        opt.RequestType = HttpRequestType::POST;
//        opt.Verbose = 1;

        const std::string json(R"({ "name": "temperature", "value": 24.03 })");

        HttpRequest request;
        request.SetOptions(opt);
        request.SetBody(std::make_shared<HttpStringBody>(json));

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

        auto body = resp->GetBody();
//        MESSAGE(body);

        std::string expected = R"(
Content length: 41
Request Method: POST
Body: )" + json + "\n";

        CHECK_EQ(body, expected);
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

    SUBCASE("Authenticated") {
        CHECK_NOTHROW(HttpSession session1(1));
        bool respHead = false;
        bool respHead2 = false;
        bool respErrHead = false;
        bool resp1 = false;
        HttpSession session(5);

        opt.BaseUrl = "https://server.localhost:44300/";
//        opt.Verbose = 1;
        session.SetDefaultOptions(opt);

        session.Head("/authenticated/index.html",
                                 [&respErrHead](IHttpResponse& resp) {
//                                     MESSAGE("Request Head:\n" << resp.GetRequest());
//                                     MESSAGE("Response Head:\n" << resp);
                                     CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                                     CHECK_EQ(resp.GetHeaders().at("content-length"), "164");
                                     CHECK_EQ(resp.GetHeaders().at("http/2 401"), "present");
                                     CHECK_EQ(resp.GetBody().size(), 0);
                                     CHECK_EQ(resp.GetStatusCode(), 401);
                                     respErrHead = true;
                                 });
        CHECK_NOTHROW(session.ProcessRequests());

        opt.BasicAuthUsername = "jb";
        opt.BasicAuthPassword = "agent007";
        session.SetDefaultOptions(opt);

        session.Head("/authenticated/index.html",
                                 [&respHead](IHttpResponse& resp) {
//                                     MESSAGE("Request Head:\n" << resp.GetRequest());
//                                     MESSAGE("Response Head:\n" << resp);
                                     CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                                     CHECK_EQ(resp.GetHeaders().at("content-length"), "131");
                                     CHECK_EQ(resp.GetHeaders().at("http/2 200"), "present");
                                     CHECK_EQ(resp.GetBody().size(), 0);
                                     CHECK_EQ(resp.GetStatusCode(), 200);

                                     CHECK_EQ(resp.GetRequest().GetOptions().BasicAuthUsername, "jb");
                                     CHECK_EQ(resp.GetRequest().GetOptions().BasicAuthPassword, "agent007");
                                     std::stringstream ss;
                                     ss << resp.GetRequest();
                                     CHECK_FALSE(StrUtils::Contains(ss.str(), "jb"));
                                     CHECK_FALSE(StrUtils::Contains(ss.str(), "agent007"));
                                     respHead = true;
                                 });
        CHECK_NOTHROW(session.ProcessRequests());

        opt.BasicAuthUsername = "";
        opt.BasicAuthPassword = "";
        opt.Headers["Authorization"] = "Basic amI6YWdlbnQwMDc="; // Found in lighttpd error.log. Curl adds this from above used BasicAuthXXX credentials
        session.SetDefaultOptions(opt);

        session.Head("/authenticated/index.html",
                     [&respHead2](IHttpResponse& resp) {
//                         MESSAGE("Request Head:\n" << resp.GetRequest());
//                         MESSAGE("Response Head:\n" << resp);
                         CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                         CHECK_EQ(resp.GetHeaders().at("content-length"), "131");
                         CHECK_EQ(resp.GetHeaders().at("http/2 200"), "present");
                         CHECK_EQ(resp.GetBody().size(), 0);
                         CHECK_EQ(resp.GetStatusCode(), 200);
                         CHECK_EQ(resp.GetRequest().GetOptions().Headers.at("Authorization"), "Basic amI6YWdlbnQwMDc=");
                         std::stringstream ss;
                         ss << resp.GetRequest();
                         CHECK(StrUtils::Contains(ss.str(), "Authorization"));
                         CHECK_FALSE(StrUtils::Contains(ss.str(), "Basic amI6YWdlbnQwMDc="));
                         respHead2 = true;
                     });
        CHECK_NOTHROW(session.ProcessRequests());

        session.Get("/authenticated/index.html",
                    [&resp1](IHttpResponse& resp) {
//                        MESSAGE("Response 1:\n" << resp);
                        CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                        CHECK_EQ(resp.GetHeaders().at("content-length"), "131");
                        CHECK_EQ(resp.GetHeaders().at("http/2 200"), "present");
                        CHECK_EQ(resp.GetBody().size(), 131);
                        CHECK_EQ(200, resp.GetStatusCode());
                        std::stringstream ss;
                        ss << resp.GetRequest();
                        CHECK(StrUtils::Contains(ss.str(), "Authorization"));
                        CHECK_FALSE(StrUtils::Contains(ss.str(), "Basic amI6YWdlbnQwMDc="));
                        resp1 = true;
                    });
        CHECK_NOTHROW(session.ProcessRequests());

        CHECK(respErrHead);
        CHECK(respHead);
        CHECK(respHead2);
        CHECK(resp1);
    }

    CHECK_EQ(0, std::system("killall lighttpd"));
}
