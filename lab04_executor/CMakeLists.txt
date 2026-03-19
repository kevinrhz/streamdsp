# lab04_executor/CMakeLists.txt

# Lab 04 depends on Lab 02 (ring buffer) and Lab 03 (pipeline stages)
add_executable(lab04_test_batch
    test_batch_executor.cpp
    src/batch_executor.cpp
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/src/window_stage.cpp
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/src/fft_stage.cpp
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/src/magnitude_stage.cpp
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/src/stage_factory.cpp
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/src/pipeline_builder.cpp
)
target_include_directories(lab04_test_batch PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/lab02_ring_buffer
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/include
)
target_link_libraries(lab04_test_batch PRIVATE pthread)

add_executable(lab04_test_streaming
    test_streaming_executor.cpp
    src/streaming_executor.cpp
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/src/window_stage.cpp
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/src/fft_stage.cpp
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/src/magnitude_stage.cpp
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/src/stage_factory.cpp
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/src/pipeline_builder.cpp
)
target_include_directories(lab04_test_streaming PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/lab02_ring_buffer
    ${CMAKE_SOURCE_DIR}/lab03_pipeline/include
)
target_link_libraries(lab04_test_streaming PRIVATE pthread)

add_custom_target(lab04_run
    COMMAND lab04_test_batch
    COMMAND lab04_test_streaming
    DEPENDS lab04_test_batch lab04_test_streaming
    COMMENT "Running Lab 04: Threaded Executors"
)
