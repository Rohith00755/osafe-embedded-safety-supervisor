cmake_minimum_required(VERSION 3.16)
project(osafe_supervisor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(osafe_supervisor src/supervisor.cpp)
target_include_directories(osafe_supervisor PRIVATE include)

enable_testing()
add_executable(osafe_protocol_tests tests/protocol_tests.cpp)
target_include_directories(osafe_protocol_tests PRIVATE include)
add_test(NAME protocol_parser COMMAND osafe_protocol_tests)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(osafe_supervisor PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(osafe_protocol_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()
