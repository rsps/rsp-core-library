#include <doctest.h>

#include <network/HttpRequest.h>
#include <iostream>
#include <logging/Logger.h>

using namespace rsp::logging;

using namespace rsp::network::http;

TEST_CASE("asdasd")
{
    CHECK_EQ(true, true);
    std::cout << "test 1" << std::endl;
    //Logger::GetDefault().Info() << "asd" << std::endl;
    HttpRequest asd;
    
    CHECK_EQ(true, false);
}