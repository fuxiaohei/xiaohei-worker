add_library(worker_v8wrap
    v8wrap.cc

    js_class.cc
    js_object.cc
    js_value.cc
)

target_include_directories (worker_v8wrap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})