add_library(worker_core
    main_context.cc
)

target_include_directories (worker_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})