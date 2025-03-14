/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022-2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Jesper Madsen, Steffen Brummer
 */

#include <doctest.h>
#include <array>
#include <cstring>
#include <filesystem>
#include <json/Json.h>
#include <network/FileBody.h>
#include <network/HttpDownload.h>
#include <network/HttpRequest.h>
#include <network/HttpSession.h>
#include <network/IHttpRequest.h>
#include <network/MultipartBody.h>
#include <network/MultipartBoundary.h>
#include <network/NetworkException.h>
#include <network/NetworkLibrary.h>
#include <network/StringBody.h>
#include <posix/FileSystem.h>
#include <posix/FileIO.h>
#include <utils/StrUtils.h>
#include <TestHelpers.h>
#include <unistd.h>

using namespace rsp::logging;
using namespace rsp::network;
using namespace rsp::utils;
using namespace rsp::posix;
using namespace std::chrono_literals;

TEST_SUITE_BEGIN("Network");

TEST_CASE("Network")
{
//    TestLogger::mLogLevel = rsp::logging::LogLevel::Notice;
    TestLogger logger;

    HttpRequestOptions opt;
    opt.CertCaPath = "webserver/ssl/ca/ca.crt";
    opt.CertPath = "webserver/ssl/certs/SN1234.crt";
    opt.KeyPath = "webserver/ssl/private/SN1234.key";

    // Run lighttpd directly from build directory, no need to install it.
    CHECK_EQ(TestHelpers::StartWebServer(), 0);
    std::this_thread::sleep_for(50ms);

    SUBCASE("Library Version"){
        CHECK_FALSE(NetworkLibrary::Get().GetLibraryName().empty());
        CHECK_GE(NetworkLibrary::Get().GetVersion(), "0.1.0");
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

    SUBCASE("HTTP to localhost") {
        HttpRequest request;
        opt.BaseUrl = "http://server.localhost:48080";

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
            CHECK_EQ(resp->GetBody().GetStreamSize(), 0);
        }
        else {
            CHECK_EQ(resp->GetBody().GetStreamSize(), 120);
        }

        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
    }

    SUBCASE("HTTP Request Reuse") {
        HttpRequest request;
        opt.BaseUrl = "http://server.localhost:48080";
        opt.Uri = "/";
        opt.RequestType = HttpRequestType::HEAD;
        request.SetOptions(opt);

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

        CHECK_EQ(resp->GetHeader("content-type"), "text/html");
        CHECK_EQ(resp->GetHeader("content-length"), "120");
        CHECK_EQ(resp->GetBody().GetStreamSize(), 0);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);

        opt.RequestType = HttpRequestType::GET;
        request.SetOptions(opt);

        CHECK_NOTHROW(resp = &request.Execute());
        CHECK_EQ(resp->GetHeader("content-type"), "text/html");
        CHECK_EQ(resp->GetHeader("content-length"), "120");
        CHECK_EQ(resp->GetBody().GetStreamSize(), 120);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
//        std::stringstream ss;
//        ss << resp->GetBody();
//        MESSAGE("Body: " << ss.str());
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
            CHECK_EQ(resp->GetBody().GetStreamSize(), 0);
        }
        else {
            CHECK_EQ(resp->GetBody().GetStreamSize(), 120);
        }

        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
    }

    SUBCASE("Invalid Client") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";
        opt.CertPath = "";
        opt.KeyPath = "";

        request.SetOptions(opt);

        CHECK_THROWS_AS(auto *resp = &request.Execute(), NetworkException);
//        CHECK_THROWS_WITH_AS(auto *resp = &request.Execute(), doctest::Contains(" (56) Failure when receiving data from the peer"), NetworkException);
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
        CHECK_EQ(resp->GetBody().GetStreamSize(), 120);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
    }

    SUBCASE("Request Reuse") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";
        opt.Uri = "/";
        opt.RequestType = HttpRequestType::HEAD;
        request.SetOptions(opt);

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

        CHECK_EQ(resp->GetHeader("content-type"), "text/html");
        CHECK_EQ(resp->GetHeader("content-length"), "120");
        CHECK_EQ(resp->GetBody().GetStreamSize(), 0);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);

        opt.RequestType = HttpRequestType::GET;
        request.SetOptions(opt);

        CHECK_NOTHROW(resp = &request.Execute());
        CHECK_EQ(resp->GetHeader("content-type"), "text/html");
        CHECK_EQ(resp->GetHeader("content-length"), "120");
        CHECK_EQ(resp->GetBody().GetStreamSize(), 120);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
//        std::stringstream ss;
//        ss << resp->GetBody();
//        MESSAGE("Body: " << ss.str());
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

            CHECK_EQ(resp->GetBody().GetStreamSize(), source.size());
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
            std::string buf;
            buf.resize(source.size());
            resp->GetBody().Read({ reinterpret_cast<std::byte*>(buf.data()), buf.size() });
            CHECK_EQ(std::memcmp(source.data(), buf.data(), source.size()), 0);
        }

        SUBCASE("To File") {
            FileSystem::DeleteFile(cFile);

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetContentLength(), 25138);
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::PartialContent);
        }

        SUBCASE("Partial To File") {
            CHECK_EQ(0, truncate(cFile.c_str(), 20*1024)); // This changes mtime

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetContentLength(), 25138 - (20*1024));
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::PartialContent);
        }

        SUBCASE("Unmodified To File") {
            auto mtime = FileSystem::GetFileModifiedTime(cFile);
            CHECK_EQ(0, truncate(cFile.c_str(), 20*1024)); // This changes mtime
            // This line will work, as the result is the partial data from an unmodified file.
            FileSystem::SetFileModifiedTime(cFile, mtime);

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().GetStreamSize(), 0);
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::PartialContent);
        }

        SUBCASE("Existing To File") {
            using namespace std::literals::chrono_literals;

            CHECK_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().GetStreamSize(), 0);
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
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
        const char* cUploadedFile = "./webserver/uploaded.png";
        const char* cSourceFile = "./webserver/public/image.png";

        FileIO file(cSourceFile, std::ios_base::in);
        auto source = file.GetContents();
        file.Seek(0);

        opt.BaseUrl = "https://server.localhost:44300/cgi/upload.sh";
        opt.RequestType = HttpRequestType::POST;
        opt.RequestBody = std::make_shared<FileBody>(file);
        opt.Headers.emplace("x-filename", "uploaded.png");
//        opt.Verbose = 1;

        HttpRequest request;
        request.SetOptions(opt);

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

        MESSAGE("Body: " << resp->GetBody());

        CHECK_EQ(resp->GetBody().GetStreamSize(), 70);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);

        CHECK(FileSystem::FileExists(cUploadedFile));
        FileIO file2(cUploadedFile, std::ios_base::in);
        auto s2 = file2.GetContents();
        CHECK_EQ(s2.size(), source.size());
        CHECK_EQ(std::memcmp(source.data(), s2.data(), source.size()), 0);

        FileSystem::DeleteFile(std::string(cUploadedFile));
    }

    SUBCASE("Form Upload") {
        const char* cUploadedFile = "./webserver/uploaded.png";
        const char* cSourceFile = "./webserver/public/image.png";

        FileIO file(cSourceFile, std::ios_base::in);
        auto source = file.GetContents();
        file.Seek(0);

        auto form = std::make_shared<MultipartBody>();
        opt.BaseUrl = "https://server.localhost:44300/cgi/upload-form.sh";
        opt.RequestType = HttpRequestType::POST;
        opt.RequestBody = form;
//        opt.Verbose = 1;

        HttpRequest request;
        request.SetOptions(opt);
        form->Add("filename", "uploaded.png");
        form->Add("filedata", file);

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

        auto body = dynamic_cast<StringBody&>(resp->GetBody()).Get();

        size_t request_size = 25455;
        if (request.GetOptions().RequestBody) {
            request_size = request.GetBody().GetStreamSize();
        }

        std::string expected = "Content Length: " + std::to_string(request_size) + "\n"
                               "CTYPE: multipart/form-data\n"
                               "filename: uploaded.png\r\n"
                               "filedata: filename=\"image.png\"; Content-Type: image/png\r\n"
                               "file-size: 25138\n";

//        std::cout << TestHelpers::ToHex(body) << std::endl;
//        std::cout << TestHelpers::ToHex(expected) << std::endl;

        CHECK_EQ(body, expected);

        CHECK_EQ(body.size(), 147);
        CHECK_EQ(resp->GetBody().GetStreamSize(), 147);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);

        CHECK(FileSystem::FileExists(cUploadedFile));
        FileIO file2(cUploadedFile, std::ios_base::in);
        auto s2 = file2.GetContents();
        CHECK_EQ(s2.size(), source.size());
        CHECK_EQ(std::memcmp(source.data(), s2.data(), source.size()), 0);

        FileSystem::DeleteFile(std::string(cUploadedFile));
    }

    SUBCASE("RequestBody Stream") {
        StringBody body(R"(
              O freddled gruntbuggly thy micturations are to me
                 As plured gabbleblochits on a lurgid bee.
              Groop, I implore thee my foonting turlingdromes.
           And hooptiously drangle me with crinkly bindlewurdles,
Or I will rend thee in the gobberwarts with my blurlecruncheon, see if I don't.
)");

        std::stringstream ss;
        ss << body; // Uses operator<<(std::ostream &o, IStreamDataProvider &s)
        CHECK_EQ(ss.str(), body.Get());
        ss.str("");
        body.Rewind();

        std::array<char, 51> _buffer{};
        std::span buffer(reinterpret_cast<std::byte*>(_buffer.data()), sizeof(_buffer));

        size_t written;
        while ((written = body.Read(buffer))) {
            std::string chunk = std::string(_buffer.data(), written);
//            MESSAGE("\nChunk:   " << chunk << "\nwritten: " << written << "\nindex:   " << rd.GetChunkIndex());
            ss << chunk;
        }
        CHECK_EQ(ss.str(), body.Get());
//        CHECK_EQ(rd.GetWritten(), 0);
//        CHECK_EQ(rd.GetChunkIndex(), 333);
    }

    SUBCASE("Post JSON") {
        opt.BaseUrl = "https://server.localhost:44300/cgi/post.sh";
        opt.RequestType = HttpRequestType::POST;
//        opt.Verbose = 1;

        const std::string json(R"({ "name": "temperature", "value": 24.03 })");

        HttpRequest request;
        request.SetOptions(opt);
        request.SetBody(std::make_shared<StringBody>(json));

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

        auto body = dynamic_cast<StringBody&>(resp->GetBody()).Get();
//        MESSAGE(body);

        std::string expected = R"(Content length: 41
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

        auto req = [&session](HttpRequestType aType, std::string_view aUri, std::string_view aMimeType, size_t aLength, size_t aBodySize, bool &result) {
            session.Request(aType,
                            aUri,
                            [=,&result](IHttpResponse& resp) {
//                               MESSAGE("Response " << aType << ":\n" << resp);
                               CHECK_EQ(resp.GetHeaders().at("content-type"), aMimeType);
                               CHECK_EQ(resp.GetContentLength(), aLength);
                               CHECK_EQ(resp.GetStatusLine().GetStatusCode(), 200);
                               CHECK(resp.GetStatusLine().GetHttpVersion().starts_with("HTTP/"));
                               CHECK_EQ(resp.GetBody().GetStreamSize(), aBodySize);
                               CHECK_EQ(resp.GetStatusCode(), StatusCodes::Ok);
                               result = true;
                           });
        };

        req(HttpRequestType::HEAD, "index.html", "text/html", 120, 0, respHead);
        CHECK_NOTHROW(session.ProcessRequests());

        req(HttpRequestType::GET, "index.html", "text/html", 120, 120, resp1);
        req(HttpRequestType::GET, "image.png", "image/png", 25138, 25138, resp2);
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
                                     CHECK_EQ(resp.GetContentLength(), 164);
                                     CHECK_EQ(resp.GetStatusLine().GetStatusCode(), 401);
                                     CHECK(resp.GetStatusLine().GetHttpVersion().starts_with("HTTP/"));
                                     CHECK_EQ(resp.GetBody().GetStreamSize(), 0);
                                     CHECK_EQ(resp.GetStatusCode(), StatusCodes::Unauthorized);
                                     respErrHead = true;
                                 });
        CHECK_NOTHROW(session.ProcessRequests());

        opt.BasicAuthUsername = "jb";
        opt.BasicAuthPassword = "agent007";
        session.SetDefaultOptions(opt);

        session.Head("/authenticated/index.html",
                                 [&respHead](IHttpResponse& resp) {
                                     MESSAGE("Request Head:\n" << resp.GetRequest());
                                     MESSAGE("Response Head:\n" << resp);
                                     CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                                     CHECK_EQ(resp.GetContentLength(), 131);
                                     CHECK_EQ(resp.GetStatusLine().GetStatusCode(), 200);
                                     CHECK(resp.GetStatusLine().GetHttpVersion().starts_with("HTTP/"));
                                     CHECK_EQ(resp.GetBody().GetStreamSize(), 0);
                                     CHECK_EQ(resp.GetStatusCode(), StatusCodes::Ok);

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
                         CHECK_EQ(resp.GetContentLength(), 131);
                         CHECK_EQ(resp.GetStatusLine().GetStatusCode(), 200);
                         CHECK(resp.GetStatusLine().GetHttpVersion().starts_with("HTTP/"));
                         CHECK_EQ(resp.GetBody().GetStreamSize(), 0);
                         CHECK_EQ(resp.GetStatusCode(), StatusCodes::Ok);
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
                        CHECK_EQ(resp.GetContentLength(), 131);
                        CHECK_EQ(resp.GetStatusLine().GetStatusCode(), 200);
                        CHECK(resp.GetStatusLine().GetHttpVersion().starts_with("HTTP/"));
                        CHECK_EQ(resp.GetBody().GetStreamSize(), 131);
                        CHECK_EQ(resp.GetStatusCode(), StatusCodes::Ok);
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

    SUBCASE("GET with arguments") {
        opt.BaseUrl = "https://server.localhost:44300/cgi/get.sh/subdir1/?key1=value1;key2=value2#Fragment1";
        opt.RequestType = HttpRequestType::GET;
//        opt.Verbose = 1;

        HttpRequest request;
        request.SetOptions(opt);

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

        auto body = dynamic_cast<StringBody&>(resp->GetBody()).Get();
        MESSAGE(body);
        CHECK(TestHelpers::ValidateJson(body));
        auto js = rsp::json::Json::Decode(body);

        CHECK_EQ(js["request-uri"].AsString(), "/cgi/get.sh/subdir1/?key1=value1;key2=value2#Fragment1");
        CHECK_EQ(js["method"].AsString(), "GET");
        CHECK_EQ(js["path"].AsString(), "/subdir1/");
        CHECK_EQ(js["query"].AsString(), "key1=value1;key2=value2");
    }

    CHECK_EQ(0, TestHelpers::StopWebServer());
}

TEST_SUITE_END();
