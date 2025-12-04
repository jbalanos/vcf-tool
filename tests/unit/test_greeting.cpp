#include <catch2/catch_test_macros.hpp>
#include <vcf_tool/domain/greeting.h>

using namespace vcf_tool::domain;

TEST_CASE("hello_from_domain returns correct message", "[domain][greeting]") {
    auto message = hello_from_domain();

    REQUIRE_FALSE(message.empty());
    REQUIRE(message == "Hello from the domain module!");
}

TEST_CASE("hello_from_domain is consistent", "[domain][greeting]") {
    auto message1 = hello_from_domain();
    auto message2 = hello_from_domain();

    REQUIRE(message1 == message2);
}
