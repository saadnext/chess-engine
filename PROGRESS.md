# Chess Engine Progress

## Current phase / milestone
Phase 1: Board representation: COMPLETE (bitboards, Color/PieceType, Board with piece bitboards, starting position, Board::print).
Housekeeping milestone (named build configurations): COMPLETE and verified (Debug, Release, Sanitize).
Now: tidy .clangd, then Phase 2: move representation.

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
- tests/test_board.cpp via CTest: empty board, put/remove, has_piece right/wrong color, piece_on, start position (popcount 16 per side, 32 total, per-piece counts via expected[] loop, spot-checks d1/e1/d8/e8). Seen failing with a deliberate break (Queen and King swapped in back_rank_order), reverted and green.
- Derived the popcount idiom bb & (bb - 1) (clears the lowest set bit); std::popcount (C++20, <bit>) used in tests.
- .gitignore uses build*/ (covers build/, build-debug/, build-release/, build-san/). Verified with git check-ignore -v (match for build-debug/, silence for build_notes.txt) and a clean git status after creating build-debug/.
- Build configurations, one directory each: build-debug (CMAKE_BUILD_TYPE=Debug), build-release (Release), build-san (Sanitize). Verified by reading the real compile lines via `cmake --build <dir> --verbose | grep "board.cpp.o -c"`:
  - Debug: -g only (no -O flag so GCC uses -O0, no -DNDEBUG, asserts active).
  - Release: -O3 -DNDEBUG, no -g (CMake's default for GCC; asserts compiled out).
  - Sanitize: -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer.
- Custom Sanitize type defined in CMakeLists.txt via CMAKE_CXX_FLAGS_SANITIZE and CMAKE_EXE_LINKER_FLAGS_SANITIZE (CMake has no built-in Sanitize type; an unknown type is accepted silently with no flags, so the flags must be verified).
- CTest 3/3 passed in all three configurations (ctest --test-dir <dir> --output-on-failure). ldd on build-san/test_board shows libasan.so.8 and libubsan.so.1, so the sanitizer runtime is linked.
- Sanitizer seen catching a planted bug: scratch/oob.cpp (a[argc + 3] on int[4]) built with the Sanitize flags produced a UBSan "index 4 out of bounds" report and an ASan stack-buffer-overflow report with file and line; the same file built without sanitizers printed garbage (73728) and exited normally.
- All work pushed; origin/main up to date, working tree clean.

## Known bugs / open issues
- Release uses CMake's default -O3. Not yet compared with -O2; decide only with a benchmark (nodes/second), not by guessing.
- Asserts in Board are not testable in-process (they abort). Release passing is evidence, not proof (UB can hide at -O3).
- .clangd points CompilationDatabase at `build`, which we no longer use. Point it at one of the new directories (e.g. build-debug); each build dir has its own compile_commands.json.
- tests/test_board.cpp: per-piece-count check in the loop has a generic failure message (name color/type); duplicate has_piece(Black, King, 60) check; std::string built per check in the empty-board loop.
- clang-format not yet run on src/board.h, src/board.cpp, tests/test_board.cpp (do as its own commit).
- `index` is a generic global name (POSIX may declare index()); rename or namespace if it collides.
- NUM_PIECE_TYPES and the enum are not linked automatically; static_assert on King guards the last value.
- piece_on loops over 12 bitboards; a square-to-piece array is the planned upgrade for movegen, to be justified by measurement.
- back_rank_order and piece_to_char are namespace-scope constexpr arrays in a header; prefer inline constexpr.

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
- set_start_position resets the board first; may later become set_fen(start_fen).
- Layout: src/, tests/, scratch/ (never mixed with engine code). Shared code in the engine_core static library. Git: explicit "git add <files>", never "git add .".
- One build directory per configuration so incremental builds are not invalidated by switching types. Configure with: cmake -S . -B <dir> -DCMAKE_BUILD_TYPE=<Debug|Release|Sanitize>. Use Debug for gdb, Sanitize for correctness runs, Release only for benchmarks.

## Repo state (last commit message, key files)
- Last pushed commit: "Update PROGRESS.md after build configurations" (main = origin/main, working tree clean as of the last git status). This PROGRESS.md update is not yet committed.
- Key files: CMakeLists.txt, .gitignore, .clangd, .clang-format, PROGRESS.md, src/main.cpp, src/bitboard.h/.cpp, src/types.h, src/board.h, src/board.cpp, tests/test_smoke.cpp, tests/test_bitboard.cpp, tests/test_board.cpp. scratch/ (ignored, contains oob.cpp experiment). Build dirs build-debug/, build-release/, build-san/ (ignored).

## Next step
Commit this PROGRESS.md. Then fix .clangd (point CompilationDatabase at build-debug), one small edit, verified by checking clangd resolves includes in VS Code. After that: run clang-format as its own commit, then Phase 2 (move representation).
