cmake_minimum_required(VERSION 3.20)

project(TNPC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(airuntime_core STATIC
    src/types.cpp
    src/rng.cpp
    src/blackboard.cpp
    src/memory.cpp
    src/perception.cpp
    src/intent_queue.cpp
    src/debug_trace.cpp
    src/behavior_tree.cpp
    src/adapter_interfaces.cpp
)

target_include_directories(airuntime_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(airuntime_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(airuntime_core PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(airuntime_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(sim_demo apps/sim_demo/main.cpp)
target_link_libraries(sim_demo PRIVATE airuntime_core)

add_executable(tests
    tests/main.cpp
    tests/test_rng.cpp
    tests/test_blackboard.cpp
    tests/test_memory.cpp
    tests/test_intent_queue.cpp
    tests/test_behavior_tree.cpp
    tests/test_demo.cpp
    tests/test_debug_trace.cpp
)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
target_link_libraries(tests PRIVATE airuntime_core)

enable_testing()
add_test(NAME airuntime_tests COMMAND tests)
