cmake_minimum_required(VERSION 3.16)

include(lib/framework64/cmake/configure_platform.cmake REQUIRED)

if (NOT FW64_PLATFORM_DESKTOP)
    project(framework64 C)
else()
    project(framework64)
endif()

set(Fw64_BUILD_TESTS OFF)
set(FW64_BUILD_EXAMPLES OFF)
set(FW64_BUILD_TOOLS OFF)

add_subdirectory(lib/framework64)
add_subdirectory(src)