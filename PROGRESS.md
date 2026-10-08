# Chess Engine Progress

## Current phase / milestone
Phase 0: Minimal Project Skeleton
Milestone: build, test, and Git push workflow verified end to end. DONE, pending optional tooling polish (clangd + .clang-format) before starting Phase 1.

## Completed and verified (with how it was verified)
- Tooling present in WSL2 (home dir /home/saad-dev): Git 2.53.0, GCC 15.2.0, CMake 4.2.3. Verified with --version commands.
- Local Git repo in ~/chess-engine, branch main, identity configured. Verified with git status / git config.
- Minimal CMake project (C++20, extensions off, -Wall -Wextra -Wpedantic, compile_commands export). Verified: cmake configure + build with zero warnings, ./build/engine prints "Chess engine v0.1".
- First commit 7a6bea3 and .gitignore (build/, compile_commands.json). Verified with git log and a clean git status.
- GitHub connection via SSH key (ed25519, with passphrase) as user saadnext. Verified with ssh -T git@github.com.
- Remote origin = git@github.com:saadnext/chess-engine.git; first push succeeded and main tracks origin/main. Verified with push output and git status "up to date".
- CTest smoke test (tests/test_smoke.cpp, exit code 0 = pass). Verified: Passed with 1+1==2 and Failed with 1+1==3 (deliberate break, reverted, never committed); commit 69bf724 pushed.

## Known bugs / open issues
- None.
- Minor: scratch/ and tests/ style: test_smoke.cpp uses 2-space indent (will be fixed by .clang-format).

## Design decisions (with reasons)
- Language/standard: C++20, GCC 13+ (or recent Clang), CMake. Reason: modern, portable, and matches the learning goals.
- Warnings: -Wall -Wextra -Wpedantic from day one. Reason: catch bugs early.
- CMAKE_CXX_EXTENSIONS OFF. Reason: standard C++20 rather than GNU dialect, for portability.
- Project location: ~/chess-engine inside WSL2 (not /mnt/c). Reason: much faster file I/O and builds.
- Environment: Windows 11 + VS Code + Ubuntu on WSL2, 8 GB RAM (avoid memory-heavy tools). Use the clangd extension instead of the Microsoft C++ extension, and clang-format with 4 spaces instead of the default 2.
- Dependencies: none until a test framework is needed. Tests are plain executables registered with CTest (exit code 0 = pass); use explicit if/return, not assert, because assert vanishes under NDEBUG.
- Layout: src/, tests/, scratch/ (scratch code never mixed with engine code). Build output in build/ (git-ignored).
- Git: explicit "git add <files>" instead of "git add ."; SSH authentication to GitHub.

## Repo state (last commit message, key files)
- Last commit: 69bf724 "Add CTest smoke test" (pushed to origin/main).
- Key files: CMakeLists.txt, .gitignore, src/main.cpp, tests/test_smoke.cpp.
- scratch/ exists locally but is empty (not tracked by Git yet).
- PROGRESS.md is to be committed next.

## Next step
1. git restore tests/test_smoke.cpp, then commit PROGRESS.md.
2. Set up clangd (VS Code extension, compile_commands.json) and a .clang-format with 4-space indentation.
3. Then begin Phase 1: board representation (bitboards), starting with the concept of a square and a 64-bit board.
