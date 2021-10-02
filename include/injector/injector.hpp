#pragma once

#include "detail/stream.h"
#include <array>
#include <sstream>

namespace injector {

#ifdef CMAKE_RESOURCE_INJECTOR_PREFIX_ENUM
enum class injected_resources { CMAKE_RESOURCE_INJECTOR_PREFIX_ENUM };
#else
enum class injected_resources {};
#endif

#ifdef CMAKE_RESOURCE_INJECTOR_PREFIX_CONSTEXPR_ENUM
enum class constinit_injected_resources { CMAKE_RESOURCE_INJECTOR_PREFIX_CONSTEXPR_ENUM };
#else
enum class constinit_injected_resources {};
#endif

template<injector::constinit_injected_resources>
consteval int ___compile_time_data_size();// NOLINT(bugprone-reserved-identifier)
template<injector::constinit_injected_resources>
consteval char const *___compile_time_data();// NOLINT(bugprone-reserved-identifier)
#ifdef CMAKE_RESOURCE_INJECTOR_PREFIX_CONSTEXPR_ENUM_IMPLEMENTATION
CMAKE_RESOURCE_INJECTOR_PREFIX_CONSTEXPR_ENUM_IMPLEMENTATION
#endif

}// namespace injector

template<injector::injected_resources>
char const *___compile_time_data();// NOLINT(bugprone-reserved-identifier)
template<injector::injected_resources>
int ___compile_time_data_size();// NOLINT(bugprone-reserved-identifier)

#ifdef CMAKE_RESOURCE_INJECTOR_PREFIX_ENUM_IMPLEMENTATION
CMAKE_RESOURCE_INJECTOR_PREFIX_ENUM_IMPLEMENTATION
#endif

namespace injector {

template<injected_resources Injection>
auto get_resource_stream() {
    return detail::resource_stream(::___compile_time_data<Injection>(),
                                   ::___compile_time_data_size<Injection>());
}

template<constinit_injected_resources Injection>
constexpr auto get_resource_stream() {
    return detail::compile_time_stream(___compile_time_data<Injection>(),
                                       ___compile_time_data_size<Injection>());
}

}// namespace injector
