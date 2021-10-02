cmake_minimum_required(VERSION 3.12.4)

# Download automatically, you can also just copy the conan.cmake file
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()
include(${CMAKE_BINARY_DIR}/conan.cmake)
include(CTest)

conan_cmake_run(REQUIRES catch2/2.13.7
        BASIC_SETUP NO_OUTPUT_DIRS CMAKE_TARGETS)

include(${PROJECT_SOURCE_DIR}/cmake/injector/Injector.cmake)

add_executable(injector_all_tests)

target_sources(injector_all_tests PRIVATE main.cpp tests.cpp)

target_include_directories(
        injector_all_tests
        PRIVATE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
            $<BUILD_INTERFACE:${CONAN_INCLUDE_DIRS_CATCH2}/catch2>)

set_target_properties(
        injector_all_tests
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)

if (MSVC)
    target_compile_options(injector_all_tests PRIVATE /WX)
else ()
    target_compile_options(injector_all_tests PRIVATE -Wall)
    target_compile_options(injector_all_tests PRIVATE -Werror -Wno-error=uninitialized)
    target_compile_options(injector_all_tests PRIVATE -Wpedantic)
endif ()

inject_resources(
        TARGET injector_all_tests
        GENERATED_DIR ${CMAKE_BINARY_DIR}/generated
        RESOURCES_DIR ${CMAKE_CURRENT_LIST_DIR}/resources
        RESOURCES
            test_resource1.txt             AS KEK1
            test_resource2.txt             AS KEK2
            test_resource_fibonacci.binary AS FIBONACCI
        COMPILE_TIME_RESOURCES
            test_resource_fibonacci.txt    AS FIBONACCI_CT
            test_resource2.txt             AS KEK2_CT
)

add_test(NAME injector_all_tests COMMAND injector_all_tests)
