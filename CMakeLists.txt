cmake_minimum_required(VERSION 3.12.4)

project(resource-injector
        VERSION 1.0.0
        DESCRIPTION "A header only c++ library for resource injection"
        HOMEPAGE_URL "https://github.com/ZhekehZ/Resource-Injector")

option(DO_NOT_TEST "Do not run unit test after build" OFF)
option(INSTALL_VIA_CONAN "Write relative conan include paths" OFF)

include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
        ${PROJECT_NAME}
        INTERFACE $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

IF (NOT DO_NOT_TEST)
    enable_testing()
    add_subdirectory(test)
ENDIF()

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

if (${INSTALL_VIA_CONAN})
    set(PATH_TO_INJECTOR_CMAKE
            "\${CONAN_LIB_DIRS_RESOURCE-INJECTOR}/injector/cmake/injector/")
else()
    set(PATH_TO_INJECTOR_CMAKE
            "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/injector/cmake/injector/")
endif()

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/injector DESTINATION include)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/cmake/injector DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/injector/cmake)
