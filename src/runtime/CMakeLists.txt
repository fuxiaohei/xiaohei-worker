
if (BUILD_V8_RUNTIME)
    message("-- BUILD_V8_RUNTIME")
    set(v8rt_src_files 
        "v8rt/v8rt.cc"
        "v8rt/v8js_context.cc"
    )
endif ()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.h.in  ${CMAKE_CURRENT_SOURCE_DIR}/conf.h)

add_library(worker_runtime
    runtime.cc
    ${v8rt_src_files}
)

target_include_directories (worker_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})