add_library(worker_webapi
    event_target.cc

    fetch/fetch_event.cc
    fetch/fetch_request.cc
    fetch/fetch_response.cc

    global_scope.cc
)

target_include_directories (worker_webapi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})