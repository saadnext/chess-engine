# Chess Engine Progress

## Current phase / milestone
Phase 1: Board representation: COMPLETE (bitboards, Color/PieceType, Board with piece bitboards, starting position, Board::print).
Now: housekeeping milestone (named build configurations: Debug, Release, sanitizers), then Phase 2: move representation.

## Completed and verified (with how it was verified)
- Phase 0 complete: Git 2.53.0, GCC 15.2.0, CMake 4.2.3, clangd 21.1.8, clang-format 21.1.8 in WSL2 (home /home/saad-dev). Verified with --version commands.
- Local Git repo in ~/chess-engine, branch main, identity configured. GitHub via SSH (ed25519) as saadnext; origin = git@github.com:saadnext/chess-engine.git. Verified with ssh -T and successful pushes.
- Minimal CMake project (C++20, extensions off, -Wall -Wextra -Wpedantic, compile_commands export). Verified: zero-warning build.
- CTest smoke test. Verified: passed, and failed with a deliberate break (reverted).
- clangd config, .clang-format (LLVM base, 4 spaces, column limit 100, pointers left-aligned), .cache/ and scratch/ ignored.
- Bitboard basics (src/bitboard.h/.cpp): Bitboard = uint64_t; little-endian rank-file mapping (a1=0, h8=63); square_bb, test_bit, square_of (constexpr, assert preconditions); is_valid_square, NUM_SQUARES; print_bb. Verified with tests/test_bitboard.cpp via CTest, deliberate breaks caught and reverted, UBSan shift experiment, assert vs NDEBUG behavior.
- src/types.h: enum class Color and PieceType, constexpr index() helpers, NUM_COLORS, NUM_PIECE_TYPES. Verified with static_asserts (and a deliberately wrong one failing).
- src/board.h: class Board with private pieces_[NUM_COLORS][NUM_PIECE_TYPES]{}, put_piece, remove_piece, piece_on (std::optional<Piece>), has_piece, pieces(), clear() (*this = Board{}), set_start_position() (clears first, then places 32 pieces via back_rank_order and square_of). Piece struct with defaulted operator==.
- src/board.cpp (in engine_core): Board::print().
- tests/test_board.cpp via CTest: empty board, put/remove, has_piece right/wrong color, piece_on, start position (popcount 16 per side, 32 total, per-piece counts via expected[] loop, spot-checks d1/e1/d8/e8). Seen failing with a deliberate break (Queen and King swapped in back_rank_order), reverted and green. CTest 3/3.
- Derived the popcount idiom bb & (bb - 1) (clears the lowest set bit); std::popcount (C++20, <bit>) used in tests.

## Known bugs / open issues
- Build type is not set explicitly (default has no -DNDEBUG, so asserts are active, no optimization). Need named Debug/Release/sanitizer configurations before any benchmark. Asserts in Board are not testable in-process (they abort).
- tests/test_board.cpp: per-piece-count check in the loop may have an empty failure message (give it a message naming color/type); duplicate has_piece(Black, King, 60) check; std::string built per check in the empty-board loop. Confirm CTest is green after the message fix.
- clang-format not yet run on src/board.h, src/board.cpp, tests/test_board.cpp (do as its own commit).
- `index` is a generic global name (POSIX may declare index()); rename or namespace if it collides.
- NUM_PIECE_TYPES and the enum are not linked automatically; static_assert on King guards the last value.
- piece_on loops over 12 bitboards; a square-to-piece array is the planned upgrade for movegen, to be justified by measurement.
- back_rank_order and piece_to_char are namespace-scope constexpr arrays in a header; prefer inline constexpr.
- .gitignore only ignores build/; add build*/ when extra build directories are created.

## Design decisions (with reasons)
- Language/standard: C++20, GCC 13+ (or recent Clang), CMake. Warnings -Wall -Wextra -Wpedantic from day one. CMAKE_CXX_EXTENSIONS OFF.
- Project location ~/chess-engine inside WSL2 (not /mnt/c). Environment: Windows 11 + VS Code + Ubuntu on WSL2, 8 GB RAM.
- Dependencies: none until needed. Tests are plain executables registered with CTest (exit 0 = pass).
- Tests use an explicit check helper (never assert, since assert vanishes under NDEBUG). Engine preconditions use assert. static_assert for compile-time checks. A test is not trusted until it has been seen failing.
- Squares are plain int (0..63) for now; a dedicated Square type will be introduced when attack/move generation starts, as its own milestone.
- All raw single-square shifts go through square_bb.
- Bit set/clear: plain |= square_bb(sq) and &= ~square_bb(sq). Never ^= to clear (toggles).
- is_valid_square returns bool so it can later validate untrusted UCI input.
- enum class for Color and PieceType, conversions via index() helpers; values contiguous from 0.
- Board: private pieces_ so invariants are enforced by member functions; read access via const getters. Tiny hot functions header-only; printing in board.cpp (keeps <iostream> out of the header).
- set_start_position resets the board first, so the result is the start position regardless of prior contents; may later become set_fen(start_fen).
- Layout: src/, tests/, scratch/ (never mixed with engine code). Shared code in the engine_core static library. Git: explicit "git add <files>", never "git add .".
- Planned build configs: one build directory per config (build-debug, build-release, build-san); Debug -O0 -g, Release -O2 -DNDEBUG, Sanitize -O1 -g -fsanitize=address,undefined.

## Repo state (last commit message, key files)
- Last pushed commit: c264d60 "Add start position, Board::print, and start-position tests" (main = origin/main). This PROGRESS.md update is not yet committed.
- Key files: CMakeLists.txt, .gitignore, .clangd, .clang-format, PROGRESS.md, src/main.cpp, src/bitboard.h/.cpp, src/types.h, src/board.h, src/board.cpp, tests/test_smoke.cpp, tests/test_bitboard.cpp, tests/test_board.cpp. scratch/print_start.cpp (ignored).

## Next step
Commit this PROGRESS.md. Then the build-configuration milestone, taken slowly: (1) run cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug and build it, (2) do the same for Release, (3) compare the compile lines for -O and -DNDEBUG. Student was overwhelmed by the size of this step; go one command at a time, with a short explanation each.
