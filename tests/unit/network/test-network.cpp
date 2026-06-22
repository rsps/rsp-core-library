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
#include <rsp/json/Json.h>
#include <rsp/network/FileBody.h>
#include <rsp/network/HttpRequest.h>
#include <rsp/network/HttpSession.h>
#include <rsp/network/IHttpRequest.h>
#include <rsp/network/MultipartBody.h>
#include <rsp/network/MultipartBoundary.h>
#include <rsp/network/NetworkException.h>
#include <rsp/network/NetworkLibrary.h>
#include <rsp/network/StringBody.h>
#include <rsp/posix/FileSystem.h>
#include <rsp/posix/FileIO.h>
#include <rsp/utils/StrUtils.h>
#include <TestHelpers.h>
#include <unistd.h>

using namespace rsp::logging;
using namespace rsp::network;
using namespace rsp::utils;
using namespace rsp::posix;
using namespace std::chrono_literals;

constexpr bool IsVerbose() { return false; }
constexpr int VerboseValue() { return IsVerbose() ? 1 : 0; }

TEST_SUITE_BEGIN("Network");

TEST_CASE("Network")
{
    if constexpr (IsVerbose()) {
        TestLogger::mLogLevel = rsp::logging::LogLevel::Notice;
    }
    TestLogger logger;

    HttpRequestOptions opt;
    opt.CertCaPath = "webserver/ssl/ca/ca.crt";
    opt.CertPath = "webserver/ssl/certs/SN1234.crt";
    opt.KeyPath = "webserver/ssl/private/SN1234.key";

    REQUIRE_EQ(TestHelpers::StartWebServer(), 0);
    std::this_thread::sleep_for(50ms);

    SUBCASE("Library Version"){
        CHECK_FALSE(NetworkLibrary::Get().GetLibraryName().empty());
        CHECK_GE(NetworkLibrary::Get().GetVersion(), "0.1.0");
        if constexpr (IsVerbose()) {
            MESSAGE("Network Library: " << NetworkLibrary::Get().GetLibraryName());
            MESSAGE("Network Library Version: " << NetworkLibrary::Get().GetVersion());
        }
    }

    SUBCASE("Online") {
        std::string ip;
        CHECK_NOTHROW(ip = FileSystem::GetCurrentIpAddress());
        MESSAGE("IP: " << ip);
        std::vector<std::string> list;
        CHECK_EQ(StrUtils::Split(ip, list, '.', false), 4u);
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

        IHttpResponse *resp = nullptr;
        REQUIRE_NOTHROW(resp = &request.Execute());

        logger.Info() << "Response:\n" << *resp << std::endl;

        if constexpr (IsVerbose()) {
            MESSAGE("Request:\n" << resp->GetRequest());
            MESSAGE("Response:\n" << *resp);
        }

        CHECK_EQ(resp->GetHeader("content-type"), "text/html");

        if (opt.RequestType == HttpRequestType::HEAD) {
            CHECK_EQ(resp->GetBody().GetStreamSize(), 0u);
        }
        else {
            CHECK_EQ(resp->GetBody().GetStreamSize(), 120u);
        }

        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
    }

    SUBCASE("HTTP Request Reuse") {
        HttpRequest request;
        opt.BaseUrl = "http://server.localhost:48080";
        opt.Uri = "/";
        opt.RequestType = HttpRequestType::HEAD;
        request.SetOptions(opt);

        const IHttpResponse *resp = nullptr;
        REQUIRE_NOTHROW(resp = &request.Execute());

        CHECK_EQ(resp->GetHeader("content-type"), "text/html");
        CHECK_EQ(resp->GetHeader("content-length"), "120");
        CHECK_EQ(resp->GetBody().GetStreamSize(), 0u);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);

        opt.RequestType = HttpRequestType::GET;
        request.SetOptions(opt);

        CHECK_NOTHROW(resp = &request.Execute());
        CHECK_EQ(resp->GetHeader("content-type"), "text/html");
        CHECK_EQ(resp->GetHeader("content-length"), "120");
        CHECK_EQ(resp->GetBody().GetStreamSize(), 120u);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
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
        SUBCASE("Omit Client Validation") {
            opt.RequestType = HttpRequestType::HEAD;
            opt.CertPath.clear();
            opt.KeyPath.clear();
        }

        request.SetOptions(opt);

        logger.Info() << "Request:\n" << request << std::endl;

        IHttpResponse *resp = nullptr;
        REQUIRE_NOTHROW(resp = &request.Execute());

        if (resp) {
            logger.Info() << "Response:\n" << *resp << std::endl;
        }

        if constexpr (IsVerbose()) {
            MESSAGE("Request:\n" << resp->GetRequest());
            MESSAGE("Response:\n" << *resp);
        }

        CHECK_EQ(resp->GetHeader("content-type"), "text/html");

        if (opt.RequestType == HttpRequestType::HEAD) {
            CHECK_EQ(resp->GetBody().GetStreamSize(), 0u);
        }
        else {
            CHECK_EQ(resp->GetBody().GetStreamSize(), 120u);
        }

        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
    }

    SUBCASE("Invalid Client") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";
        opt.CertPath = "";
        opt.KeyPath = "";

        request.SetOptions(opt);

        REQUIRE_THROWS_AS(auto *resp = &request.Execute(), NetworkException);
    }

    SUBCASE("Validated Client") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";

        request.SetOptions(opt);

        const IHttpResponse *resp = nullptr;
        REQUIRE_NOTHROW(resp = &request.Execute());

        if constexpr (IsVerbose()) {
            MESSAGE("Request:\n" << resp->GetRequest());
            MESSAGE("Response:\n" << *resp);
        }

        CHECK_EQ(resp->GetHeader("content-type"), "text/html");
        CHECK_EQ(resp->GetBody().GetStreamSize(), 120u);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
    }

    SUBCASE("Request Reuse") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";
        opt.Uri = "/";
        opt.RequestType = HttpRequestType::HEAD;
        request.SetOptions(opt);

        const IHttpResponse *resp = nullptr;
        REQUIRE_NOTHROW(resp = &request.Execute());

        CHECK_EQ(resp->GetHeader("content-type"), "text/html");
        CHECK_EQ(resp->GetHeader("content-length"), "120");
        CHECK_EQ(resp->GetBody().GetStreamSize(), 0u);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);

        opt.RequestType = HttpRequestType::GET;
        request.SetOptions(opt);

        REQUIRE_NOTHROW(resp = &request.Execute());
        CHECK_EQ(resp->GetHeader("content-type"), "text/html");
        CHECK_EQ(resp->GetHeader("content-length"), "120");
        CHECK_EQ(resp->GetBody().GetStreamSize(), 120u);
        CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
        if constexpr (IsVerbose()) {
            std::stringstream ss;
            ss << resp->GetBody();
            MESSAGE("Body: " << ss.str());
        }
    }

    SUBCASE("File Download") {
        const std::filesystem::path cFile("./image.png");
        const std::filesystem::path cSourceFile("./webserver/public/image.png");

        FileIO file(cSourceFile, std::ios_base::in);
        auto source = file.GetContents();

        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300/image.png";
        opt.Verbose = VerboseValue();
        opt.ResponseBody = std::make_shared<FileBody>(cFile);
        request.SetOptions(opt);

        const IHttpResponse *resp = nullptr;

        SUBCASE("To Memory") {
            FileSystem::DeleteFile(cFile);
            request.SetResponseBody(nullptr); // Omit destination file to download to memory

            REQUIRE_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().GetStreamSize(), source.size());
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
            std::string buf;
            buf.resize(source.size());
            auto sz = resp->GetBody().Read({ reinterpret_cast<std::byte*>(buf.data()), buf.size() });
            CHECK_EQ(sz, source.size());
            CHECK_EQ(std::memcmp(source.data(), buf.data(), source.size()), 0);
        }

        SUBCASE("To File") {
            FileSystem::DeleteFile(cFile);
            request.MakeResponseBody<FileBody>(cFile); // Reload after removing file

            REQUIRE_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetContentLength(), 25138u);
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::PartialContent);
        }

        SUBCASE("Partial To File") {
            CHECK_EQ(0, truncate(cFile.c_str(), 20*1024)); // This changes mtime

            REQUIRE_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetContentLength(), 25138u - (20u*1024));
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::PartialContent);
        }

        SUBCASE("Unmodified To File") {
            auto mtime = FileSystem::GetFileModifiedTime(cFile);
            CHECK_EQ(0, truncate(cFile.c_str(), 20*1024)); // This changes mtime
            // This line will work, as the result is the partial data from an unmodified file.
            FileSystem::SetFileModifiedTime(cFile, mtime);

            REQUIRE_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().GetStreamSize(), 0u);
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::PartialContent);
        }

        SUBCASE("Existing To File") {
            using namespace std::literals::chrono_literals;

            REQUIRE_NOTHROW(resp = &request.Execute());

            CHECK_EQ(resp->GetBody().GetStreamSize(), 0u);
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
        }

        CHECK_EQ(resp->GetHeader("content-type"), "image/png");

        if constexpr (IsVerbose()) {
            MESSAGE("Request:\n" << resp->GetRequest());
            MESSAGE("Response:\n" << *resp);
        }

        if (FileSystem::FileExists(cFile)) {
            CHECK_EQ(FileSystem::GetFileModifiedTime(cFile), FileSystem::GetFileModifiedTime(cSourceFile));
            FileIO file2(cFile, std::ios_base::in);
            auto s2 = file2.GetContents();
            CHECK_EQ(s2.size(), source.size());
            CHECK_EQ(std::memcmp(source.data(), s2.data(), source.size()), 0);
        }
    }

    SUBCASE("File Upload") {
        auto cUploadedFile = "./webserver/uploaded.png";
        auto cSourceFile = "./webserver/public/image.png";

        FileIO file(cSourceFile, std::ios_base::in);
        auto source = file.GetContents();
        file.Seek(0);

        opt.BaseUrl = "https://server.localhost:44300/cgi/upload.sh";
        opt.RequestType = HttpRequestType::POST;
        opt.RequestBody = std::make_shared<FileBody>(file);
        opt.Headers.try_emplace("x-filename", "uploaded.png");
        opt.Verbose = VerboseValue();

        HttpRequest request;
        request.SetOptions(opt);

        const IHttpResponse *resp = nullptr;
        REQUIRE_NOTHROW(resp = &request.Execute());
        if (resp) {
            MESSAGE("Body: " << resp->GetBody());

            CHECK_EQ(resp->GetBody().GetStreamSize(), 70u);
            CHECK_EQ(resp->GetStatusCode(), StatusCodes::Ok);
        }
        CHECK(FileSystem::FileExists(cUploadedFile));
        FileIO file2(cUploadedFile, std::ios_base::in);
        auto s2 = file2.GetContents();
        CHECK_EQ(s2.size(), source.size());
        CHECK_EQ(std::memcmp(source.data(), s2.data(), source.size()), 0);

        FileSystem::DeleteFile(std::string(cUploadedFile));
    }

    SUBCASE("Form Upload") {
        auto cUploadedFile = "./webserver/uploaded.png";
        auto cSourceFile = "./webserver/public/image.png";

        FileIO file(cSourceFile, std::ios_base::in);
        auto source = file.GetContents();
        file.Seek(0);

        auto form = std::make_shared<MultipartBody>();
        opt.BaseUrl = "https://server.localhost:44300/cgi/upload-form.sh";
        opt.RequestType = HttpRequestType::POST;
        opt.RequestBody = form;
        opt.Verbose = VerboseValue();

        HttpRequest request;
        request.SetOptions(opt);
        form->Add("filename", "uploaded.png");
        form->Add("filedata", file);

        const IHttpResponse *resp = nullptr;
        REQUIRE_NOTHROW(resp = &request.Execute());

        auto body = dynamic_cast<StringBody&>(resp->GetBody()).Get();

        size_t request_size = 25455;
        if (request.GetOptions().RequestBody) {
            request_size = request.GetBody().GetStreamSize();
        }

        std::string expected = std::format("Content Length: {}\n"
                               "CTYPE: multipart/form-data\n"
                               "filename: uploaded.png\r\n"
                               "filedata: filename=\"image.png\"; Content-Type: image/png\r\n"
                               "file-size: 25138\n", request_size);

        CHECK_EQ(body, expected);

        CHECK_EQ(body.size(), 147u);
        REQUIRE(resp != nullptr);
        CHECK_EQ(resp->GetBody().GetStreamSize(), 147u);
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
            auto chunk = std::string(_buffer.data(), written);
//            MESSAGE("\nChunk:   " << chunk << "\nwritten: " << written << "\nindex:   " << rd.GetChunkIndex());
            ss << chunk;
        }
        CHECK_EQ(ss.str(), body.Get());
    }

    SUBCASE("Post JSON") {
        opt.BaseUrl = "https://server.localhost:44300/cgi/post.sh";
        opt.RequestType = HttpRequestType::POST;
        opt.Verbose = VerboseValue();

        const std::string json(R"({ "name": "temperature", "value": 24.03 })");

        HttpRequest request;
        request.SetOptions(opt);
        request.SetBody(std::make_shared<StringBody>(json));

        const IHttpResponse *resp = nullptr;
        REQUIRE_NOTHROW(resp = &request.Execute());

        if (resp) {
            auto body = dynamic_cast<StringBody&>(resp->GetBody()).Get();
            if constexpr (IsVerbose()) {
                MESSAGE(body);
            }
            std::string expected = R"(Content length: 41
Request Method: POST
Body: )" + json + "\n";

            CHECK_EQ(body, expected);
        }
    }

    SUBCASE("Http Session") {
        CHECK_NOTHROW(HttpSession session1(1));
        bool respHead = false;
        bool resp1 = false;
        bool resp2 = false;
        HttpSession session(5);

        opt.BaseUrl = "https://server.localhost:44300/";
        opt.Verbose = VerboseValue();
        session.SetDefaultOptions(opt);

        auto req = [&session](HttpRequestType aType, std::string_view aUri, std::string_view aMimeType, size_t aLength, size_t aBodySize, bool &result) {
            session.Request(
                aType,
                aUri,
                [aType, aMimeType, aLength, aBodySize, &result](const IHttpResponse& resp) {
                    if constexpr (IsVerbose()) {
                        MESSAGE("Response " << aType << ":\n" << resp);
                    }
                    else {
                        (void)aType;
                    }
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
        opt.Verbose = VerboseValue();
        session.SetDefaultOptions(opt);

        session.Head(
            "/authenticated/index.html",
            [&respErrHead](const IHttpResponse& resp) {
                if constexpr (IsVerbose()) {
                    MESSAGE("Request Head:\n" << resp.GetRequest());
                    MESSAGE("Response Head:\n" << resp);
                }
                CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                CHECK_EQ(resp.GetContentLength(), 164u);
                CHECK_EQ(resp.GetStatusLine().GetStatusCode(), 401);
                CHECK(resp.GetStatusLine().GetHttpVersion().starts_with("HTTP/"));
                CHECK_EQ(resp.GetBody().GetStreamSize(), 0u);
                CHECK_EQ(resp.GetStatusCode(), StatusCodes::Unauthorized);
                respErrHead = true;
            });
        CHECK_NOTHROW(session.ProcessRequests());

        opt.BasicAuthUsername = "jb";
        opt.BasicAuthPassword = "agent007"; // TODO: use testing .env file for this, to avoid linter complains
        session.SetDefaultOptions(opt);

        session.Head(
            "/authenticated/index.html",
            [&respHead](const IHttpResponse& resp) {
                MESSAGE("Request Head:\n" << resp.GetRequest());
                MESSAGE("Response Head:\n" << resp);
                CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                CHECK_EQ(resp.GetContentLength(), 131u);
                CHECK_EQ(resp.GetStatusLine().GetStatusCode(), 200);
                CHECK(resp.GetStatusLine().GetHttpVersion().starts_with("HTTP/"));
                CHECK_EQ(resp.GetBody().GetStreamSize(), 0u);
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

        session.Head(
            "/authenticated/index.html",
            [&respHead2](const IHttpResponse& resp) {
                if constexpr (IsVerbose()) {
                    MESSAGE("Request Head:\n" << resp.GetRequest());
                    MESSAGE("Response Head:\n" << resp);
                }
                CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                CHECK_EQ(resp.GetContentLength(), 131u);
                CHECK_EQ(resp.GetStatusLine().GetStatusCode(), 200);
                CHECK(resp.GetStatusLine().GetHttpVersion().starts_with("HTTP/"));
                CHECK_EQ(resp.GetBody().GetStreamSize(), 0u);
                CHECK_EQ(resp.GetStatusCode(), StatusCodes::Ok);
                CHECK_EQ(resp.GetRequest().GetOptions().Headers.at("Authorization"), "Basic amI6YWdlbnQwMDc=");
                std::stringstream ss;
                ss << resp.GetRequest();
                CHECK(StrUtils::Contains(ss.str(), "Authorization"));
                CHECK_FALSE(StrUtils::Contains(ss.str(), "Basic amI6YWdlbnQwMDc="));
                respHead2 = true;
            });
        CHECK_NOTHROW(session.ProcessRequests());

        session.Get(
            "/authenticated/index.html",
            [&resp1](const IHttpResponse& resp) {
                if constexpr (IsVerbose()) {
                    MESSAGE("Response 1:\n" << resp);
                }
                CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                CHECK_EQ(resp.GetContentLength(), 131u);
                CHECK_EQ(resp.GetStatusLine().GetStatusCode(), 200);
                CHECK(resp.GetStatusLine().GetHttpVersion().starts_with("HTTP/"));
                CHECK_EQ(resp.GetBody().GetStreamSize(), 131u);
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
        opt.Verbose = VerboseValue();

        HttpRequest request;
        request.SetOptions(opt);

        const IHttpResponse *resp = nullptr;
        REQUIRE_NOTHROW(resp = &request.Execute());

        auto body = dynamic_cast<StringBody&>(resp->GetBody()).Get();
        MESSAGE(body);
        CHECK(TestHelpers::ValidateJson(body));
        auto js = rsp::json::Json::Decode(body);

        CHECK_EQ(js["request-uri"].AsString(), "/cgi/get.sh/subdir1/?key1=value1;key2=value2");
        CHECK_EQ(js["method"].AsString(), "GET");
        CHECK_EQ(js["path"].AsString(), "/subdir1/");
        CHECK_EQ(js["query"].AsString(), "key1=value1;key2=value2");
    }

    CHECK_EQ(0, TestHelpers::StopWebServer());
}

TEST_SUITE_END();
