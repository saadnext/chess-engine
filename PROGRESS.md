# Chess Engine Progress

## Current phase / milestone
Phase 1: Board representation. Milestone 1 (bitboard type, square mapping, print, targeted tests): COMPLETE once the commit below is pushed.
Next milestone: piece/color representation and a Board holding the piece bitboards (still Phase 1).

## Completed and verified (with how it was verified)
- Phase 0 complete: Git 2.53.0, GCC 15.2.0, CMake 4.2.3, clangd 21.1.8, clang-format 21.1.8 in WSL2 (home /home/saad-dev). Verified with --version commands.
- Local Git repo in ~/chess-engine, branch main, identity configured. Verified with git status / git config.
- Minimal CMake project (C++20, extensions off, -Wall -Wextra -Wpedantic, compile_commands export). Verified: zero-warning build.
- GitHub via SSH key (ed25519, passphrase) as saadnext; origin = git@github.com:saadnext/chess-engine.git. Verified with ssh -T and a successful push.
- CTest smoke test (tests/test_smoke.cpp). Verified: passed with 1+1==2, failed with a deliberate break (reverted, never committed).
- clangd config (.clangd pointing at build/), .clang-format (LLVM base, 4 spaces, column limit 100), .cache/ ignored. Commit cf5f44e pushed. (VS Code hover/squiggle check: to be confirmed by user.)
- Bitboard basics (src/bitboard.h, src/bitboard.cpp): Bitboard = uint64_t; square mapping little-endian rank-file (a1=0, h1=7, a2=8, h8=63); square_bb, test_bit, square_of (all constexpr, with assert preconditions); print_bb (rank 8 on top).
  - Verified: tests/test_bitboard.cpp via CTest (square_bb at 0/28/63, test_bit positive and negative cases around e4, square_of at four corner/known points). Passed.
  - Verified tests can fail: deliberately changed square_of to rank*7+file; exactly square_of(4,3) and square_of(7,7) failed, exit code 1; reverted and re-passed.
  - Verified with UBSan (scratch experiment) that 1 << 40 on an int is flagged at runtime as "shift exponent 40 is too large for 32-bit type 'int'".
  - Verified assert behavior: square_of(8,0) aborts in a normal build, silently returns 8 (a2) under -DNDEBUG.
  - Verified print_bb from the real engine executable (./build/engine shows a1 and e4).
- CMake: static library target engine_core (src/bitboard.cpp, PUBLIC include dir src); engine, test_smoke, test_bitboard link against it as needed. Verified: configure, build, ctest 2/2 passed.

## Known bugs / open issues
- .clang-format shows as modified in git status; reason not yet checked (git diff .clang-format, then commit separately or restore).
- scratch/ is untracked but not yet in .gitignore.
- Build type is not set explicitly (default has no -DNDEBUG, so asserts are active). Need named Debug/Release/sanitizer configurations before any Release builds.
- Not yet tested: all 64 file/rank combinations round-trip through square_of.

## Design decisions (with reasons)
- Language/standard: C++20, GCC 13+ (or recent Clang), CMake. Reason: modern, portable, matches learning goals.
- Warnings: -Wall -Wextra -Wpedantic from day one. Reason: catch bugs early.
- CMAKE_CXX_EXTENSIONS OFF. Reason: standard C++20 for portability.
- Project location: ~/chess-engine inside WSL2 (not /mnt/c). Reason: much faster file I/O and builds.
- Environment: Windows 11 + VS Code + Ubuntu on WSL2, 8 GB RAM (avoid memory-heavy tools). clangd extension; clang-format with 4 spaces.
- Dependencies: none until a test framework is needed. Tests are plain executables registered with CTest (exit 0 = pass).
- Tests use explicit if/check helper (never assert, since assert vanishes under NDEBUG). Internal preconditions in engine code use assert (zero cost in release; tests and sanitizers run on debug builds). static_assert is fine for compile-time checks of constexpr functions.
- Squares are plain int (0..63) for now; a dedicated Square type (enum class or thin struct) will be introduced later to prevent mixing ranks, files, and squares. uint8_t only for compact storage (e.g. packed moves), decided later by measurement.
- All raw bit shifts for single squares go through square_bb (always uses a 64-bit left operand).
- Layout: src/, tests/, scratch/ (scratch code never mixed with engine code). build/ and .cache/ are git-ignored. Shared code lives in the engine_core static library.
- Git: explicit "git add <files>", never "git add ."; SSH auth to GitHub.

## Repo state (last commit message, key files)
- Last pushed commit: cf5f44e "Add clangd config, clang-format (4 spaces), progress file". Pending commit: "Add bitboard basics: square_bb, test_bit, square_of, print_bb, with tests" (staged, not yet committed at time of writing).
- Key files: CMakeLists.txt, .gitignore, .clangd, .clang-format, PROGRESS.md, src/main.cpp, src/bitboard.h, src/bitboard.cpp, tests/test_smoke.cpp, tests/test_bitboard.cpp.
- scratch/ exists locally (contains bb.cpp experiment), untracked.

## Next step
Resolve the .clang-format change and add scratch/ to .gitignore. Then Phase 1, next milestone: represent colors and piece types, and a Board struct holding the piece bitboards, with a test that places pieces and reads them back.
