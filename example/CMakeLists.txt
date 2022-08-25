cmake_minimum_required(VERSION 3.12.4)

project(example-project)

## 1. INCLUDE CONAN
# Download automatically, you can also just copy the conan.cmake file
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()
include(${CMAKE_BINARY_DIR}/conan.cmake)

## 2. ADD CONAN DEPENDENCIES
conan_cmake_run(REQUIRES resource-injector/0.1@zhekehz/stable)  ## finds c++ headers
conan_basic_setup()
find_package(resource-injector)  ## imports cmake functions

## 3. CREATE EXECUTABLE
add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CONAN_INCLUDE_DIRS})  ## adds headers to target
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

## 4. INJECT RESOURCES
inject_resources(TARGET ${PROJECT_NAME}
                 RESOURCES CMakeLists.txt AS cmake)
