add_library(worker_webapi
    event_target.cc
    fetch_event.cc
    global_scope.cc
)

target_include_directories (worker_webapi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})