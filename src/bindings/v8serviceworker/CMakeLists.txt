add_library(worker_v8serviceworker
    serviceworker.cc

    base64.cc
    timer.cc

    event/event_target.cc
    event/event_listener.cc
    event/fetch_event.cc

    fetch/fetch_request.cc
    fetch/fetch_response.cc
    fetch/fetch.cc

    webstreams/webstreams.cc
    webstreams/readablestream.cc
    webstreams/readablestream_controller.cc
    webstreams/underlying_source.cc
    webstreams/strategy.cc
)

target_include_directories(worker_v8serviceworker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})