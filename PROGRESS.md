# Chess Engine Progress

## Current phase / milestone
Phase 0: Minimal Project Skeleton. COMPLETE.
Next: Phase 1, board representation. Milestone: represent a board as 64-bit bitboards and print it, with targeted tests.

## Completed and verified (with how it was verified)
- Tooling present in WSL2 (home dir /home/saad-dev): Git 2.53.0, GCC 15.2.0, CMake 4.2.3, clangd 21.1.8, clang-format 21.1.8. Verified with --version commands.
- Local Git repo in ~/chess-engine, branch main, identity configured. Verified with git status / git config.
- Minimal CMake project (C++20, extensions off, -Wall -Wextra -Wpedantic, compile_commands export). Verified: configure + build with zero warnings, ./build/engine prints "Chess engine v0.1".
- GitHub connection via SSH key (ed25519, passphrase) as user saadnext; origin = git@github.com:saadnext/chess-engine.git. Verified with ssh -T and a successful push; main tracks origin/main.
- CTest smoke test (tests/test_smoke.cpp, exit code 0 = pass). Verified: Passed with 1+1==2, Failed with 1+1==3 (deliberate break, reverted, never committed).
- clangd config (.clangd pointing at build/), .clang-format (LLVM base, 4 spaces, column limit 100), .cache/ ignored. Verified: commit cf5f44e pushed, working tree clean. (VS Code hover/squiggle check: to be confirmed by user.)

## Known bugs / open issues
- None.

## Design decisions (with reasons)
- Language/standard: C++20, GCC 13+ (or recent Clang), CMake. Reason: modern, portable, matches learning goals.
- Warnings: -Wall -Wextra -Wpedantic from day one. Reason: catch bugs early.
- CMAKE_CXX_EXTENSIONS OFF. Reason: standard C++20 for portability.
- Project location: ~/chess-engine inside WSL2 (not /mnt/c). Reason: much faster file I/O and builds.
- Environment: Windows 11 + VS Code + Ubuntu on WSL2, 8 GB RAM (avoid memory-heavy tools). clangd extension instead of Microsoft C++ extension; clang-format with 4 spaces.
- Dependencies: none until a test framework is needed. Tests are plain executables registered with CTest (exit 0 = pass); use explicit if/return, not assert (assert vanishes under NDEBUG).
- Layout: src/, tests/, scratch/ (scratch code never mixed with engine code). build/ and .cache/ are git-ignored.
- Git: explicit "git add <files>", never "git add ."; SSH auth to GitHub.

## Repo state (last commit message, key files)
- Last commit: cf5f44e "Add clangd config, clang-format (4 spaces), progress file" (pushed to origin/main).
- Key files: CMakeLists.txt, .gitignore, .clangd, .clang-format, PROGRESS.md, src/main.cpp, tests/test_smoke.cpp.
- scratch/ exists locally but is empty (untracked by Git).

## Next step
Phase 1: board representation. Start with the concept of a square (0..63 numbering) and a 64-bit bitboard, with a small scratch experiment that prints a bitboard.
