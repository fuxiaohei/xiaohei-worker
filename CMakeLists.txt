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

# add compile arguments
add_definitions("-g -DV8_COMPRESS_POINTERS -O0 -fno-rtti")

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

# add libraries
add_subdirectory(src/core)
add_subdirectory(src/lib/v8wrap)

# add worker cmd
add_subdirectory(cmd/worker)