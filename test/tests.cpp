#include "../include/injector/injector.hpp"

#include <catch.hpp>
#include <array>

TEST_CASE("Basic-1", "[Text][Run time]") {
    using namespace injector;

    {
        auto istream = get_resource_stream<injected_resources::KEK1>();
        std::string s;
        istream >> s;
        REQUIRE(s == "test_resource1");
        for (int i : {1, 2, 3, 4}) {
            int x;
            istream >> x;
            REQUIRE(x == i);
        }
        REQUIRE(istream.eof());
    }

    {
        auto istream = get_resource_stream<injected_resources::KEK2>();
        std::string s;
        istream >> s;
        REQUIRE(s == "test_resource2");
        istream >> s;
        REQUIRE(s == "hello");
        REQUIRE(istream.eof());
    }

}

TEST_CASE("Basic-2", "[Binary][Run time]") {
    using namespace injector;

    auto istream = injector::get_resource_stream<injected_resources::FIBONACCI>();
    for (int i : {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377}) {
        int x;
        istream.read(reinterpret_cast<char*>(&x), sizeof(x));
        REQUIRE(x == i);
    }
}


TEST_CASE("Basic-3", "[Text][Compile time][Run time]") {

    auto lambda = [] {
        using namespace injector;
        auto fib_ct = injector::get_resource_stream<constinit_injected_resources::FIBONACCI_CT>();

        if (!fib_ct.data()) return false;
        if (!fib_ct.size()) return false;

        for (int i : {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377}) {
            int x = -1;
            fib_ct >> x;
            if (fib_ct.last_error() != injector::detail::parse_error_code::NO_ERROR) {
                return false;
            }
            if (x != i) {
                return false;
            }
        }

        return true;
    };

    STATIC_REQUIRE(lambda());
    REQUIRE(lambda());
}

TEST_CASE("Basic-4", "[Compile time]") {
    using namespace injector;

    auto check = [] (auto & array) {
        char expected[] = "test_";

        for (int i = 0; i < 5; ++i) {
            if (array[i] != expected[i]) {
                return false;
            }
        }
        return true;
    };

    auto lambda = [=] {
        auto istream = get_resource_stream<constinit_injected_resources::KEK2_CT>();
        std::array<char, 5> s = {'a', 'b', 'c', 'd', 'e'};
        istream >> s;
        return check(s);
    };

    auto lambda2 = [=] {
      auto istream = get_resource_stream<constinit_injected_resources::KEK2_CT>();
      char s[5] = {'a', 'b', 'c', 'd', 'e'};
      istream >> s;
      return check(s);
    };

    STATIC_REQUIRE(lambda());
    REQUIRE(lambda());
    STATIC_REQUIRE(lambda2());
    REQUIRE(lambda2());
}
