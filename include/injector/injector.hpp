#pragma once

#include "detail/stream.h"
#include <array>
#include <sstream>

namespace injector {

#ifdef CMAKE_RESOURCE_INJECTOR_PREFIX_ENUM
enum class injected_resources { CMAKE_RESOURCE_INJECTOR_PREFIX_ENUM,
                                __ENUM_SIZE }; // NOLINT(bugprone-reserved-identifier)
#else
enum class injected_resources {};
#endif

#ifdef CMAKE_RESOURCE_INJECTOR_PREFIX_CONSTEXPR_ENUM
enum class constinit_injected_resources { CMAKE_RESOURCE_INJECTOR_PREFIX_CONSTEXPR_ENUM,
                                          __ENUM_SIZE }; // NOLINT(bugprone-reserved-identifier)
#else
enum class constinit_injected_resources {};
#endif

template<injector::constinit_injected_resources>
inline consteval int ___compile_time_data_size();// NOLINT(bugprone-reserved-identifier)
template<injector::constinit_injected_resources>
inline consteval char const *___compile_time_data();// NOLINT(bugprone-reserved-identifier)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#ifdef CMAKE_RESOURCE_INJECTOR_PREFIX_CONSTEXPR_ENUM_IMPLEMENTATION
CMAKE_RESOURCE_INJECTOR_PREFIX_CONSTEXPR_ENUM_IMPLEMENTATION
#endif
#pragma GCC diagnostic pop

}// namespace injector

template<injector::injected_resources>
inline char const *___compile_time_data();// NOLINT(bugprone-reserved-identifier)
template<injector::injected_resources>
inline int ___compile_time_data_size();// NOLINT(bugprone-reserved-identifier)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#ifdef CMAKE_RESOURCE_INJECTOR_PREFIX_ENUM_IMPLEMENTATION
CMAKE_RESOURCE_INJECTOR_PREFIX_ENUM_IMPLEMENTATION
#endif
#pragma GCC diagnostic pop

namespace injector {

template<injected_resources Injection>
inline auto get_resource_stream() {
    return detail::resource_stream(::___compile_time_data<Injection>(),
                                   ::___compile_time_data_size<Injection>());
}

template<constinit_injected_resources Injection>
inline constexpr auto get_resource_stream() {
    return detail::compile_time_stream(___compile_time_data<Injection>(),
                                       ___compile_time_data_size<Injection>());
}

namespace detail {

    template <size_t ... Indices>
    resource_stream get_impl(size_t idx, std::index_sequence<Indices...>) {
        const char * data = nullptr;
        int size = 0;
        ((Indices == idx ? (data = ::___compile_time_data<
            static_cast<injected_resources>(Indices)>()) : data) , ...);
        ((Indices == idx ? (size = ::___compile_time_data_size<
            static_cast<injected_resources>(Indices)>()) : size) , ...);
        return {data, static_cast<size_t>(size)};
    }

} // namespace detail


inline detail::resource_stream get_resource_stream(injected_resources injection) {
    return detail::get_impl(static_cast<size_t>(injection),
                std::make_index_sequence<static_cast<size_t>(injected_resources::__ENUM_SIZE)>());
}

namespace detail {

template<typename T, typename SELF>
inline constexpr compile_time_stream & parse_enum(compile_time_stream & cs, T & val) {
    char const *it = SELF::TEXT;
    size_t idx = 0;

    std::string_view word;
    size_t initial_pos = cs.current_pos();
    cs >> word;

    while (*it != '\0') {
        char const *cmp_it = word.begin();
        while (cmp_it < word.end() && *it != '\0' && *it != ',' && *it == *cmp_it) {
            ++cmp_it;
            ++it;
        }
        if (*it == '\0' || *it == ',') {
            val = static_cast<T>(idx);
            return cs;
        }
        while (*it != '\0' && *it != ',') {
            ++it;
        }
        if (*it == ',') {
            ++it;
        }
        ++idx;
    }

    cs.set_pos(initial_pos);
    cs.set_error(parse_error_code::INVALID_ENUM_NAME);
    return cs;
}

#define __INJ_RES_STR(...) #__VA_ARGS__
#define __INJ_RES_MACRO_TO_STRING(X) __INJ_RES_STR(X)

template <bool CONSTEXPR>
struct injected_enum_parser;

template <>
struct injected_enum_parser<true> {
    static constexpr char const * TEXT =
        __INJ_RES_MACRO_TO_STRING(CMAKE_RESOURCE_INJECTOR_PREFIX_ENUM);

    constexpr static compile_time_stream & parse_enum(
        compile_time_stream & cs, injected_resources & val) {
        return ::injector::detail::parse_enum<injected_resources,
                                              injected_enum_parser>(cs, val);
    };
};

template <>
struct injected_enum_parser<false> {
    static constexpr char const * TEXT =
        __INJ_RES_MACRO_TO_STRING(CMAKE_RESOURCE_INJECTOR_PREFIX_CONSTEXPR_ENUM);

    constexpr static compile_time_stream & parse_enum(
        compile_time_stream & cs, constinit_injected_resources & val) {
        return ::injector::detail::parse_enum<constinit_injected_resources,
                                              injected_enum_parser>(cs, val);
    };
};

#undef __INJ_RES_STR
#undef __INJ_RES_MACRO_TO_STRING

}// namespace detail

inline constexpr detail::compile_time_stream& operator>>(
    detail::compile_time_stream & s, injected_resources & val) {
    return detail::injected_enum_parser<true>::parse_enum(s, val);
}

inline constexpr detail::compile_time_stream& operator>>(
    detail::compile_time_stream & s, constinit_injected_resources & val) {
    return detail::injected_enum_parser<false>::parse_enum(s, val);
}

}// namespace injector
