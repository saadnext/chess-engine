cmake_minimum_required(VERSION 3.20)
project(ChessEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(engine src/main.cpp)
target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_executable(test_smoke tests/test_smoke.cpp)
target_compile_options(test_smoke PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME smoke COMMAND test_smoke)
