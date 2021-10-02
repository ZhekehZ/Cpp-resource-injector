## Resource injector
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

### Limitations:
- COMPILE_TIME_RESOURCES support text files only
- only gcc and clang compilers are supported

### TODO:
- [ ] implement `operator>>(double&)` for compile time resource stream 
- [ ] ??? support binary files for COMPILE_TIME_RESOURCES 
- [ ] support msvc compiler