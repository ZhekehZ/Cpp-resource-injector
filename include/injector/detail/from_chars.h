#pragma once

#include <cstdint>
#include <utility>

namespace injector::detail {

enum class parse_error_code : std::size_t {
    NO_ERROR,
    NO_CHARS,
    INVALID_FORMAT,
    INVALID_SIGNEDNESS
};

namespace detail {

struct from_chars_result : std::pair<char const *, parse_error_code> {
    using Base = std::pair<char const *, parse_error_code>;
    using Base::Base;
};

static_assert(sizeof(from_chars_result) == sizeof(char const *) + sizeof(parse_error_code));

constexpr bool is_blank(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

constexpr bool is_digit(char c) {
    return '0' <= c && c <= '9';
}

constexpr int to_num(char c) {
    return c - '0';
}

constexpr from_chars_result from_chars_sign(char const *begin, char const *end, int &sign) {
    sign = 1;
    if (begin != end && *begin == '-') {
        sign = -1;
        ++begin;
    }
    return {begin, parse_error_code::NO_ERROR};
}

template<typename T>
concept IntegralNotChar = std::integral<T> and not std::same_as<char, T>;

}// namespace detail

constexpr detail::from_chars_result from_chars(char const *begin, char const *end, char &c) {
    if (begin == end) {
        return {begin, parse_error_code::NO_CHARS};
    }
    c = *begin;
    return {begin + 1, parse_error_code::NO_ERROR};
}

template<detail::IntegralNotChar Integral, bool WithSign = true>
constexpr detail::from_chars_result from_chars(char const *begin, char const *end, Integral &i) {
    if (begin == end) {
        return {begin, parse_error_code::NO_CHARS};
    }

    char const *pos = begin;
    int sign;

    if constexpr (WithSign) {
        pos = detail::from_chars_sign(begin, end, sign).first;

        if (begin == end) {
            return {begin, parse_error_code::INVALID_FORMAT};
        }

        if (std::unsigned_integral<Integral> && sign < 0) {
            return {begin, parse_error_code::INVALID_SIGNEDNESS};
        }
    }

    i = 0;
    while (pos != end && detail::is_digit(*pos)) {
        i *= 10;
        if constexpr (WithSign) {
            i += detail::to_num(*pos) * sign;
        } else {
            i += detail::to_num(*pos);
        }
        ++pos;
    }

    return {pos, parse_error_code::NO_ERROR};
}

template<std::floating_point Real>
constexpr detail::from_chars_result from_chars(char const *begin, char const *end, Real &r) {// TODO: Finish
    int integer;
    char symbol;

    auto [pos, err] = from_chars(begin, end, integer);
    if (err != parse_error_code::NO_ERROR) {
        return {begin, parse_error_code::INVALID_FORMAT};
    }
    r = integer;

    auto [dot_pos, err2] = from_chars(pos, end, symbol);
    if (err2 == parse_error_code::NO_CHARS) {
        return {pos, parse_error_code::NO_ERROR};
    }

    if (symbol == '.') {
        auto [fin, err3] = from_chars<int, false>(dot_pos, end, integer);
        if (err3 != parse_error_code::NO_ERROR) {
            return {begin, parse_error_code::INVALID_FORMAT};
        }

        Real after_dot = integer;
        for (int i = 0; i < fin - dot_pos; ++i) {
            after_dot /= 10.0;
        }

        r += after_dot;

        return {fin, parse_error_code::NO_ERROR};
    } else if (symbol == 'e' || symbol == 'E') {
        auto [fin, err3] = from_chars(dot_pos, end, integer);
        if (err3 != parse_error_code::NO_ERROR) {
            return {begin, parse_error_code::INVALID_FORMAT};
        }

        if (integer > 0) {
            while (integer != 0) {
                r *= 10.0;
                --integer;
            }
        } else {
            while (integer != 0) {
                r /= 10.0;
                ++integer;
            }
        }

        return {fin, parse_error_code::NO_ERROR};
    }

    return {pos, parse_error_code::NO_ERROR};
}

}// namespace injector::detail
