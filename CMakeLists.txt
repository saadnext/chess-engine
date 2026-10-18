cmake_minimum_required(VERSION 3.20)
project(ChessEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_SANITIZE "-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS_SANITIZE "-fsanitize=address,undefined")

add_library(engine_core STATIC src/bitboard.cpp src/board.cpp)
target_include_directories(engine_core PUBLIC src)
target_compile_options(engine_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(engine src/main.cpp)
target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(engine PRIVATE engine_core)

enable_testing()
add_executable(test_smoke tests/test_smoke.cpp)
target_compile_options(test_smoke PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME smoke COMMAND test_smoke)

add_executable(test_bitboard tests/test_bitboard.cpp)
target_compile_options(test_bitboard PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(test_bitboard PRIVATE engine_core)
add_test(NAME bitboard COMMAND test_bitboard)

add_executable(test_board tests/test_board.cpp)
target_compile_options(test_board PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(test_board PRIVATE engine_core)
add_test(NAME board COMMAND test_board)

add_executable(test_move tests/test_move.cpp)
target_compile_options(test_move PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(test_move PRIVATE engine_core)
add_test(NAME move COMMAND test_move)

add_executable(test_attacks tests/test_attacks.cpp)
target_compile_options(test_attacks PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(test_attacks PRIVATE engine_core)
add_test(NAME attacks COMMAND test_attacks)
