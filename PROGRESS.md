# Chess Engine Progress

## Current phase / milestone
Phase 1: Board representation. Milestone 1 (bitboards) and Milestone 2 (Color/PieceType types, Board with piece bitboards): COMPLETE.
Next milestone: board setup helpers (starting position, print Board), then move to Phase 2 (move representation). A Square type is planned for when attack generation begins.

## Completed and verified (with how it was verified)
- Phase 0 complete: Git 2.53.0, GCC 15.2.0, CMake 4.2.3, clangd 21.1.8, clang-format 21.1.8 in WSL2 (home /home/saad-dev). Verified with --version commands.
- Local Git repo in ~/chess-engine, branch main, identity configured. GitHub via SSH (ed25519) as saadnext; origin = git@github.com:saadnext/chess-engine.git. Verified with ssh -T and successful pushes.
- Minimal CMake project (C++20, extensions off, -Wall -Wextra -Wpedantic, compile_commands export). Verified: zero-warning build.
- CTest smoke test. Verified: passed, and failed with a deliberate break (reverted).
- clangd config, .clang-format (LLVM base, 4 spaces, column limit 100, pointers left-aligned), .cache/ and scratch/ ignored. Housekeeping verified: git status clean at commit aea4c1f.
- Bitboard basics (src/bitboard.h/.cpp): Bitboard = uint64_t; little-endian rank-file mapping (a1=0, h8=63); square_bb, test_bit, square_of (constexpr, assert preconditions); print_bb.
  - Verified with tests/test_bitboard.cpp via CTest, deliberate break of square_of (rank*7+file) caught, UBSan shift experiment, assert vs NDEBUG behavior, print_bb from the engine executable.
- is_valid_square(int) and NUM_SQUARES = 64 in bitboard.h; used by asserts in square_bb and test_bit. Verified: checks for -1, 0, 63, 64. Break `<=` caught by !is_valid_square(64); break `> 0` caught by assert abort in square_bb(0). Reverted and green.
- src/types.h: enum class Color : int {White, Black}, enum class PieceType : int {Pawn..King}, constexpr index() helpers, NUM_COLORS = 2, NUM_PIECE_TYPES = 6. Verified: static_asserts for the full mapping; a deliberately wrong static_assert failed with "(1 == 2)".
- Scoped vs unscoped enum experiment: int x = Color::White compiles for unscoped, fails for enum class; int to enum is never implicit.
- src/board.h: header-only class Board with private pieces_[NUM_COLORS][NUM_PIECE_TYPES]{}, and put_piece, remove_piece, piece_on (returns std::optional<Piece>, const), has_piece, pieces() getter. Piece struct with defaulted operator==.
  - Verified: tests/test_board.cpp via CTest (fresh board all 12 empty, put/remove, has_piece with correct and wrong color, piece_on for occupied and empty squares, king untouched after knight removal). CTest 3/3 passed.
  - Verified the tests can fail: has_piece hardcoded to White row made the wrong-color check and the Black King check fail; reverted and green.
  - Verified by hand (scratch/board.cpp) that put_piece onto an occupied square aborts with the occupancy assert.

## Known bugs / open issues
- Build type is not set explicitly (default has no -DNDEBUG, so asserts are active). Need named Debug/Release/sanitizer configurations before any Release builds. Asserts in Board are not testable in-process (they abort).
- test_board.cpp: duplicate has_piece(Black, King, 60) check, and the 12-combination loop builds a std::string per check (cheap fix: plain messages). Run clang-format on tests/test_board.cpp and src/board.h.
- `index` is a generic global name (POSIX historically declares index() in <strings.h>); rename or namespace if it ever collides.
- NUM_PIECE_TYPES and the enum are not linked automatically; static_assert on King guards the last value.
- piece_on loops over 12 bitboards; a square-to-piece array is the planned upgrade for movegen, to be justified by measurement.


## Design decisions (with reasons)
- Language/standard: C++20, GCC 13+ (or recent Clang), CMake. Warnings -Wall -Wextra -Wpedantic from day one. CMAKE_CXX_EXTENSIONS OFF.
- Project location ~/chess-engine inside WSL2 (not /mnt/c). Environment: Windows 11 + VS Code + Ubuntu on WSL2, 8 GB RAM.
- Dependencies: none until needed. Tests are plain executables registered with CTest (exit 0 = pass).
- Tests use an explicit check helper (never assert, since assert vanishes under NDEBUG). Engine preconditions use assert. static_assert for compile-time checks. A test is not trusted until it has been seen failing.
- Squares are plain int (0..63) for now; a dedicated Square type will be introduced when attack/move generation starts (squares are computed there: sq+8, sq^56, loops), as its own milestone.
- All raw single-square shifts go through square_bb.
- Bit set/clear: plain |= square_bb(sq) and &= ~square_bb(sq), no set_bit/clear_bit helpers (they add a second spelling for an idiomatic one-liner; revisit if &= ~ proves error-prone). Never ^= to clear (toggles).
- is_valid_square returns bool (usable in assert, if, and static contexts) instead of a void asserting helper, so it can later validate untrusted UCI input.
- enum class for Color and PieceType, conversions via index() helpers; values contiguous from 0.
- Board: private pieces_ so invariants are enforced by member functions (no two pieces on a square; remove_piece asserts the specified piece is there); read access via const getters. Header-only for now (tiny hot functions should inline); board.cpp is created when FEN parsing / printing arrive.
- Layout: src/, tests/, scratch/ (never mixed with engine code). Shared code in the engine_core static library. Git: explicit "git add <files>", never "git add .".

## Repo state (last commit message, key files)
- Last pushed commit: 48bfab3 "Add Color/PieceType types, is_valid_square, Board with piece bitboards, tests".

- Key files: CMakeLists.txt, .gitignore, .clangd, .clang-format, PROGRESS.md, src/main.cpp, src/bitboard.h/.cpp, src/types.h, src/board.h, tests/test_smoke.cpp, tests/test_bitboard.cpp, tests/test_board.cpp.

## Next step
Commit the uncommitted files. Then Phase 1, next milestone: a starting-position setup (place all 32 pieces) and a Board print function, verified by tests that count pieces per side, so we have a real position to work with before move representation.
