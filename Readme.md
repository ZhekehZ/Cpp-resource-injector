## Resource injector [c++20]

A header only c++ library for resource injection

[![Conan version - resource-injector/0.1@zhekehz/stable](https://img.shields.io/badge/Conan_version-resource--injector%2F0.1%40zhekehz%2Fstable-6699cb?logo=conan&logoColor=6699cb)](https://https://zhekehz.jfrog.io/artifactory/api/conan/zhekehz-conan)
[![20](https://img.shields.io/badge/20-red?logo=c%2B%2B)](https://)
---

### Usage

CmakeLists.txt:
```cmake
INJECT_RESOURCES(
    TARGET          target_name
    GENERATED_DIR   <some pah>
    RESOURCES_DIR   <resources dir>
    RESOURCES
        res.txt AS NAME1
        res2.txt AS NAME2
    COMPILE_TIME_RESOURCES
        compile_time.txt AS NAME3
)
```


Code.cpp:
```c++
auto stream1 = get_resource_stream<injected_resources::NAME1>(); 
// The resource "NAME1" will be copied and linked to the binary file.

// If NAME2 is not used in the code, then the resource "NAME2" will NOT 
//  be copied and linked to the binary file. (*)

constinit auto stream2 = 
    get_resource_stream<constinit_injected_resources::NAME3>();
// The NAME3 resource will be injected at compile time

// (*) If compilation flags "-fdata-sections -ffunction-sections -Wl,--gc-sections" are used
```

### Installation:
- via Cmake
    ```bash
    mkdir -p build && \
    cmake -B build && \
    cmake --build build --target test && \
    sudo cmake --build build --target install
    ```
- via Conan:
    1. add remote repo:
        ```bash
        conan remote add zhekehz-conan https://zhekehz.jfrog.io/artifactory/api/conan/zhekehz-conan
        ```
    2. Configure CMake project:
        ```cmake
        conan_cmake_run(REQUIRES resource-injector/0.1@zhekehz/stable)
        conan_basic_setup()
        find_package(resource-injector)
        ```
       see the [exmaple subproject](example) for more details


### TODO:
- [ ] refactor compile-time parsers 
- [ ] ??? support binary files for COMPILE_TIME_RESOURCES 
