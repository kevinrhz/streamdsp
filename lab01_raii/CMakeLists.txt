# lab01_raii/CMakeLists.txt

add_executable(lab01_test_buffer test_buffer.cpp)
add_executable(lab01_test_file test_file.cpp)

add_custom_target(lab01_run
    COMMAND lab01_test_buffer
    COMMAND lab01_test_file
    DEPENDS lab01_test_buffer lab01_test_file
    COMMENT "Running Lab 01: RAII Resource Wrappers"
)
