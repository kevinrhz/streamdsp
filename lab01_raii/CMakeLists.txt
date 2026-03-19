# lab01_raii/CMakeLists.txt

add_executable(lab01_test_owned_buffer test_owned_buffer.cpp)
add_executable(lab01_test_owned_file test_owned_file.cpp)

add_custom_target(lab01_run
    COMMAND lab01_test_owned_buffer
    COMMAND lab01_test_owned_file
    DEPENDS lab01_test_owned_buffer lab01_test_owned_file
    COMMENT "Running Lab 01: RAII Resource Wrappers"
)
