# src
add_executable(worker.app 
    main.cc
)

# link
target_link_libraries(worker.app
    worker_runtime
    worker_webapi
    worker_core
    worker_common
    worker_v8wrap
    ${libv8_file}
    ${libhv_file}
    ${libcrypto_file}
    ${libssl_file}
    pthread
)