# Chess Engine Progress

## Current phase / milestone
Phase 1: Board representation: COMPLETE (bitboards, Color/PieceType, Board with piece bitboards, starting position, Board::print).
Next: Phase 2, move representation. First, a short housekeeping pass (explicit Debug/Release/sanitizer build configurations, test cleanups) before any benchmarking.

## Completed and verified (with how it was verified)
- Phase 0 complete: Git 2.53.0, GCC 15.2.0, CMake 4.2.3, clangd 21.1.8, clang-format 21.1.8 in WSL2 (home /home/saad-dev). Verified with --version commands.
- Local Git repo in ~/chess-engine, branch main, identity configured. GitHub via SSH (ed25519) as saadnext; origin = git@github.com:saadnext/chess-engine.git. Verified with ssh -T and successful pushes.
- Minimal CMake project (C++20, extensions off, -Wall -Wextra -Wpedantic, compile_commands export). Verified: zero-warning build.
- CTest smoke test. Verified: passed, and failed with a deliberate break (reverted).
- clangd config, .clang-format (LLVM base, 4 spaces, column limit 100, pointers left-aligned), .cache/ and scratch/ ignored. Housekeeping verified: git status clean.
- Bitboard basics (src/bitboard.h/.cpp): Bitboard = uint64_t; little-endian rank-file mapping (a1=0, h8=63); square_bb, test_bit, square_of (constexpr, assert preconditions); print_bb.
  - Verified with tests/test_bitboard.cpp via CTest, deliberate break of square_of (rank*7+file) caught, UBSan shift experiment, assert vs NDEBUG behavior, print_bb from the engine executable.
- is_valid_square(int) and NUM_SQUARES = 64 in bitboard.h; used by asserts in square_bb and test_bit. Verified: checks for -1, 0, 63, 64; deliberate breaks caught and reverted.
- src/types.h: enum class Color and PieceType, constexpr index() helpers, NUM_COLORS = 2, NUM_PIECE_TYPES = 6. Verified: static_asserts for the full mapping; a deliberately wrong static_assert failed.
- Scoped vs unscoped enum experiment: int x = Color::White compiles for unscoped, fails for enum class; int to enum is never implicit.
- src/board.h: class Board with private pieces_[NUM_COLORS][NUM_PIECE_TYPES]{}, put_piece, remove_piece, piece_on (std::optional<Piece>), has_piece, pieces(). Piece struct with defaulted operator==.
  - Verified: tests/test_board.cpp via CTest (empty board, put/remove, has_piece with correct and wrong color, piece_on, king untouched after knight removal). Tests seen failing with a deliberate break (reverted).
  - Verified by hand (scratch/board.cpp) that put_piece onto an occupied square aborts with the occupancy assert.
- Board::set_start_position() (clears first, then places 32 pieces using back_rank_order[file] and square_of) and Board::print() (in src/board.cpp, part of engine_core). Verified: printed output checked by eye (queen on d, king on e, Black not mirrored); tests via std::popcount: 16 per side, 32 total, per-piece counts for both colors, spot-checks d1/e1/d8/e8. Deliberate break (Queen and King swapped in back_rank_order) made the board test fail; reverted and green. CTest 3/3.
- Derived the popcount idiom bb & (bb - 1) (clears the lowest set bit); std::popcount (C++20, <bit>) used in tests.

## Known bugs / open issues
- Build type is not set explicitly (default has no -DNDEBUG, so asserts are active). Need named Debug/Release/sanitizer configurations before any Release builds. Asserts in Board are not testable in-process (they abort).
- Board::clear() uses memset (C idiom); replace with *this = Board{} or std::fill.
- tests/test_board.cpp: duplicate has_piece(Black, King, 60) check; per-check std::string building in the empty-board loop; 12 count variables could be a loop over an expected-counts array; size_t used for popcount results (use int); Black_* naming. Run clang-format on tests/test_board.cpp and src/board.h.
- `index` is a generic global name (POSIX historically declares index() in <strings.h>); rename or namespace if it ever collides.
- NUM_PIECE_TYPES and the enum are not linked automatically; static_assert on King guards the last value.
- piece_on loops over 12 bitboards; a square-to-piece array is the planned upgrade for movegen, to be justified by measurement.
- back_rank_order and piece_to_char are namespace-scope constexpr arrays in a header; prefer inline constexpr.
- scratch/print_start.cpp is the scratch driver for printing the start position.

## Design decisions (with reasons)
- Language/standard: C++20, GCC 13+ (or recent Clang), CMake. Warnings -Wall -Wextra -Wpedantic from day one. CMAKE_CXX_EXTENSIONS OFF.
- Project location ~/chess-engine inside WSL2 (not /mnt/c). Environment: Windows 11 + VS Code + Ubuntu on WSL2, 8 GB RAM.
- Dependencies: none until needed. Tests are plain executables registered with CTest (exit 0 = pass).
- Tests use an explicit check helper (never assert, since assert vanishes under NDEBUG). Engine preconditions use assert. static_assert for compile-time checks. A test is not trusted until it has been seen failing.
- Squares are plain int (0..63) for now; a dedicated Square type will be introduced when attack/move generation starts, as its own milestone.
- All raw single-square shifts go through square_bb.
- Bit set/clear: plain |= square_bb(sq) and &= ~square_bb(sq), no set_bit/clear_bit helpers. Never ^= to clear (toggles).
- is_valid_square returns bool so it can later validate untrusted UCI input.
- enum class for Color and PieceType, conversions via index() helpers; values contiguous from 0.
- Board: private pieces_ so invariants are enforced by member functions; read access via const getters. Header-only for the tiny hot functions; printing lives in board.cpp (keeps <iostream> out of the header).
- set_start_position resets the board first, so after the call the board equals the start position regardless of prior contents. set_start_position may later be replaced by set_fen(start_fen) when FEN parsing arrives.
- Layout: src/, tests/, scratch/ (never mixed with engine code). Shared code in the engine_core static library. Git: explicit "git add <files>", never "git add .".

## Repo state (last commit message, key files)
- Last pushed commit I have seen: 0686221 "PROGRESS.md: mark Milestone 2 as committed". Starting-position work (board.h, src/board.cpp, tests/test_board.cpp, CMakeLists.txt) is not yet confirmed committed.
- Key files: CMakeLists.txt, .gitignore, .clangd, .clang-format, PROGRESS.md, src/main.cpp, src/bitboard.h/.cpp, src/types.h, src/board.h, src/board.cpp, tests/test_smoke.cpp, tests/test_bitboard.cpp, tests/test_board.cpp.

## Next step
Commit the starting-position work (explicit git add of the changed files) and this PROGRESS.md. Then a small housekeeping milestone: Debug/Release/sanitizer CMake configurations and the pending test cleanups. After that, Phase 2: move representation.
