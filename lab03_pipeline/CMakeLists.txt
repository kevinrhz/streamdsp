# lab03_pipeline/CMakeLists.txt

add_executable(lab03_test_stages
    test_stages.cpp
    src/window_stage.cpp
    src/fft_stage.cpp
    src/magnitude_stage.cpp
    src/stage_factory.cpp
    src/pipeline_builder.cpp
)
target_include_directories(lab03_test_stages PRIVATE include)

add_executable(lab03_test_pipeline
    test_pipeline.cpp
    src/window_stage.cpp
    src/fft_stage.cpp
    src/magnitude_stage.cpp
    src/stage_factory.cpp
    src/pipeline_builder.cpp
)
target_include_directories(lab03_test_pipeline PRIVATE include)

add_custom_target(lab03_run
    COMMAND lab03_test_stages
    COMMAND lab03_test_pipeline
    DEPENDS lab03_test_stages lab03_test_pipeline
    COMMENT "Running Lab 03: Processing Pipeline"
)
