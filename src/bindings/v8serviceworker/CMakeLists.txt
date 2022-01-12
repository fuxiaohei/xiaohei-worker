add_library(worker_v8serviceworker
    serviceworker.cc

    event/event_target.cc
    event/event_listener.cc
    event/fetch_event.cc
)

target_include_directories(worker_v8serviceworker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})