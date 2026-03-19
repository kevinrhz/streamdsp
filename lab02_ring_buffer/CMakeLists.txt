# lab02_ring_buffer/CMakeLists.txt

add_executable(lab02_test_ring_buffer test_ring_buffer.cpp)

add_executable(lab02_test_spsc_stress test_spsc_stress.cpp)
target_link_libraries(lab02_test_spsc_stress PRIVATE pthread)

add_executable(lab02_test_break_ordering test_break_ordering.cpp)
target_link_libraries(lab02_test_break_ordering PRIVATE pthread)

add_custom_target(lab02_run
    COMMAND lab02_test_ring_buffer
    COMMAND lab02_test_spsc_stress
    DEPENDS lab02_test_ring_buffer lab02_test_spsc_stress
    COMMENT "Running Lab 02: Lock-Free SPSC Ring Buffer"
)
