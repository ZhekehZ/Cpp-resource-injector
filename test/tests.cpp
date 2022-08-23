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

TEST_CASE("Basic-4", "[Compile time][Run time]") {
    using namespace injector;

    constexpr auto check = [] (auto & array) {
        char expected[] = "test_";

        for (int i = 0; i < 5; ++i) {
            if (array[i] != expected[i]) {
                return false;
            }
        }
        return true;
    };

    constexpr auto lambda = [=] {
        auto istream = get_resource_stream<constinit_injected_resources::KEK2_CT>();
        std::array<char, 5> s = {'a', 'b', 'c', 'd', 'e'};
        istream >> s;
        return check(s);
    };

    constexpr auto lambda2 = [=] {
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

TEST_CASE("From chars integral", "[from_chars]") {
    using T = std::tuple<std::string_view, int, bool>;
    for (auto [str, expected, full_str] : {
            T("+123", 123, true),
            T("-123", -123, true),
            T("123", 123, true),
            T("0000123", 123, true),
            T("0000123   ", 123, false),
            T("0000123,", 123, false),
         })
    {
        std::string_view str_sv(str);

        int res;
        auto [pos, code] = injector::detail::from_chars(
            str_sv.data(), str_sv.data() + str_sv.size(), res);

        REQUIRE(code == injector::detail::parse_error_code::NO_ERROR);
        if (full_str) {
            REQUIRE(pos == str_sv.data() + str_sv.size());
        }
        REQUIRE(res == expected);
    }

    for (auto test_case: {
             "aaa",
             "111111111111111111111111111111" // overflow
        })
    {
        std::string_view str = test_case;
        int res;
        auto [pos, code] = injector::detail::from_chars(
            str.data(), str.data() + str.size(), res);
        REQUIRE(code != injector::detail::parse_error_code::NO_ERROR);
        REQUIRE(pos == str.data());
    }
}

TEST_CASE("Parse injected enum", "[parse_enum]") {
    auto s = injector::get_resource_stream<injector::constinit_injected_resources::KEK3>();
    injector::injected_resources kek1;
    injector::injected_resources fail;
    std::string_view fail_skip;
    injector::injected_resources fibonacci;
    injector::constinit_injected_resources fibonacci_ct;

    SECTION("OK [KEK1]") {
        s >> kek1;
        REQUIRE(s.last_error() == injector::detail::parse_error_code::NO_ERROR);
        REQUIRE(s.current_pos() == 4);
        REQUIRE(kek1 == injector::injected_resources::KEK1);
    }

    SECTION("INVALID_ENUM_NAME") {
        s >> kek1 >> fail;
        REQUIRE(s.current_pos() == 4);
        REQUIRE(s.last_error() == injector::detail::parse_error_code::INVALID_ENUM_NAME);
    }

    SECTION("OK [FIBONACCI]") {
        s >> kek1 >> fail_skip >> fibonacci;
        REQUIRE(s.last_error() == injector::detail::parse_error_code::NO_ERROR);
        REQUIRE(fibonacci == injector::injected_resources::FIBONACCI);
    }

    SECTION("OK [FIBONACCI_CT]") {
        s >> kek1 >> fail_skip >> fibonacci >> fibonacci_ct;
        REQUIRE(s.last_error() == injector::detail::parse_error_code::NO_ERROR);
        REQUIRE(fibonacci_ct == injector::constinit_injected_resources::FIBONACCI_CT);
        REQUIRE(s.eof());
    }
}
