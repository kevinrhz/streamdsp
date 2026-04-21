cmake_minimum_required(VERSION 3.16)
project(SigTekXReconstruction LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-O2 -Wall -Wextra -Wpedantic)

# Common headers available to all labs
include_directories(${CMAKE_SOURCE_DIR}/common)

# Labs (uncomment as you work through them)
add_subdirectory(lab01_raii)
# add_subdirectory(lab02_ring_buffer)
# add_subdirectory(lab03_pipeline)
# add_subdirectory(lab04_executor)
