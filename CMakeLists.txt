cmake_minimum_required(VERSION 3.10)

project(xiaohei_worker VERSION 0.0.1)

# set c++ version
set(CMAKE_CXX_STANDARD 17)

# set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)


# set system info
set(OS_ARCH "${CMAKE_SYSTEM_PROCESSOR}")

# set system library dependencies
find_package(Threads REQUIRED)

option(BUILD_V8_RUNTIME "Build v8 runtime" ON)

# debug or release
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif()
message("-- CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")

# add compile arguments
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions("-DV8_COMPRESS_POINTERS -O3 -fno-rtti")
    message("-- RELEASE BUILD")
else()
    add_definitions("-g -DV8_COMPRESS_POINTERS -O0 -fno-rtti")
endif()


# include headers
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/lib)

# include dependencies
set(thirdparty_dir ${CMAKE_SOURCE_DIR}/thirdparty)

# libhv
include_directories(${thirdparty_dir}/libhv/include)
find_library(libhv_file hv_static "${thirdparty_dir}/libhv/lib/")

# openssl
find_library(libcrypto_file crypto "${thirdparty_dir}/openssl/lib/")
find_library(libssl_file ssl "${thirdparty_dir}/openssl/lib/")

# v8
include_directories(${thirdparty_dir}/v8/include)
find_library(libv8_file v8_monolith "${thirdparty_dir}/v8/lib/")

configure_file(${CMAKE_SOURCE_DIR}/src/lib/common/version.h.in  ${CMAKE_SOURCE_DIR}/src/lib/common/version.h @ONLY)

# add libraries
add_subdirectory(src/lib/common)
add_subdirectory(src/lib/v8wrap)
add_subdirectory(src/lib/webapi)
add_subdirectory(src/runtime)
add_subdirectory(src/core)

if (BUILD_V8_RUNTIME)
    add_subdirectory(src/bindings/v8serviceworker)
endif()

# add worker cmd
add_subdirectory(cmd/worker)