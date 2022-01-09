add_library(worker_common
    common.cc
    json.cc
)

target_include_directories (worker_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})