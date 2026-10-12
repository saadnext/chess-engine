# Chess Engine Progress

## Current phase / milestone
Phase 1: Board representation: COMPLETE (bitboards, Color/PieceType, Board with piece bitboards, starting position, Board::print).
Housekeeping milestone (build configurations, .clangd, formatting check): COMPLETE.
Header hygiene and naming cleanup: COMPLETE (pending the commit and the clang-format dry-run check).
Now: Phase 2: move representation.

## Completed and verified (with how it was verified)
- Phase 0 complete: Git 2.53.0, GCC 15.2.0, CMake 4.2.3, clangd 21.1.8, clang-format 21.1.8 in WSL2 (home /home/saad-dev). Verified with --version commands.
- Local Git repo in ~/chess-engine, branch main, identity configured. GitHub via SSH (ed25519) as saadnext; origin = git@github.com:saadnext/chess-engine.git. Verified with ssh -T and successful pushes.
- Minimal CMake project (C++20, extensions off, -Wall -Wextra -Wpedantic, compile_commands export). Verified: zero-warning build.
- CTest smoke test. Verified: passed, and failed with a deliberate break (reverted).
- .clang-format (LLVM base, 4 spaces, column limit 100, pointers left-aligned), .cache/ and scratch/ ignored.
- clang-format check on board.h/board.cpp/test_board.cpp printed nothing earlier (already formatted); cross-checked with a deliberately ugly scratch file and --dump-config.
- .clangd points CompilationDatabase at build-debug (commit c7ad234). Verified: clangd hover shows the signature of square_of in board.cpp.
- Bitboard basics (src/bitboard.h/.cpp): Bitboard = uint64_t; little-endian rank-file mapping (a1=0, h8=63); square_bb, test_bit, square_of (constexpr, assert preconditions); is_valid_square, NUM_SQUARES; print_bb. Verified with tests/test_bitboard.cpp via CTest, deliberate breaks caught and reverted, UBSan shift experiment, assert vs NDEBUG behavior.
- src/types.h: enum class Color and PieceType, constexpr to_index() helpers (renamed from index()), NUM_COLORS, NUM_PIECE_TYPES. Verified with static_asserts (and a deliberately wrong one failing).
- src/board.h: class Board with private pieces_[NUM_COLORS][NUM_PIECE_TYPES]{}, inline hot functions (pieces, put_piece, remove_piece, piece_on, has_piece); declarations of set_start_position, clear, print. Piece struct with defaulted operator==.
- src/board.cpp (in engine_core): Board::print(), set_start_position(), clear(), plus BACK_RANK_ORDER and PIECE_TO_CHAR in an anonymous namespace.
- tests/test_board.cpp via CTest: empty board, put/remove, has_piece right/wrong color, piece_on, start position (popcount 16 per side, 32 total, per-piece counts, spot-checks d1/e1/d8/e8). Seen failing with a deliberate break (Queen/King swapped in back_rank_order), reverted and green.
- Derived the popcount idiom bb & (bb - 1); std::popcount (C++20, <bit>) used in tests.
- .gitignore uses build*/ (verified with git check-ignore -v).
- Build configurations, one directory each: build-debug (Debug: -g, asserts on), build-release (Release: -O3 -DNDEBUG), build-san (Sanitize: -O1 -g -fsanitize=address,undefined). Verified by reading real compile lines. Custom Sanitize type via CMAKE_CXX_FLAGS_SANITIZE (CMake silently accepts unknown types, so flags must be verified).
- Sanitizer seen catching a planted bug (scratch/oob.cpp): UBSan and ASan reports; the same file without sanitizers ran silently with garbage.
- Header hygiene cleanup: cold Board code and single-use constant arrays moved to board.cpp; constants renamed UPPER_CASE; index() renamed to_index(). A find-and-replace damaged a local variable (square_index became square_to_index); caught in review and fixed. Verified: cmake --build recompiled board.cpp and test_board in all three configurations, then ctest 3/3 passed in each; grep "to_index" src tests shows only the definitions and real calls; git diff --stat showed only board.cpp, board.h, types.h.

## Known bugs / open issues
- Release uses CMake's default -O3. Not yet compared with -O2; decide only with a benchmark (nodes/second).
- Asserts in Board are not testable in-process (they abort). Release passing is evidence, not proof (UB can hide at -O3).
- tests/test_board.cpp: per-piece-count check in the loop has a generic failure message; duplicate has_piece(Black, King, 60) check; std::string built per check in the empty-board loop.
- NUM_PIECE_TYPES and the enum are not linked automatically; static_assert on King guards the last value.
- piece_on loops over 12 bitboards; a square-to-piece array is the planned upgrade for movegen, to be justified by measurement. It stays in the header until then.
- Magic number 8 in BACK_RANK_ORDER and loops; introduce NUM_FILES/NUM_RANKS later.
- Run clang-format --dry-run on src/board.h src/board.cpp src/types.h after the cleanup (output not yet seen).
- Lesson: ctest does not rebuild; always run cmake --build before ctest.

## Design decisions (with reasons)
- Language/standard: C++20, GCC 13+ (or recent Clang), CMake. Warnings -Wall -Wextra -Wpedantic from day one. CMAKE_CXX_EXTENSIONS OFF.
- Project location ~/chess-engine inside WSL2 (not /mnt/c). Environment: Windows 11 + VS Code + Ubuntu on WSL2, 8 GB RAM.
- Dependencies: none until needed. Tests are plain executables registered with CTest (exit 0 = pass).
- Tests use an explicit check helper (never assert). Engine preconditions use assert. static_assert for compile-time checks. A test is not trusted until it has been seen failing.
- Squares are plain int (0..63) for now; a dedicated Square type will be introduced when attack/move generation starts, as its own milestone.
- All raw single-square shifts go through square_bb. Set/clear with |= and &= ~, never ^=.
- is_valid_square returns bool so it can later validate untrusted UCI input.
- enum class for Color and PieceType, conversions via to_index(); values contiguous from 0.
- Header vs .cpp rule: tiny hot functions (and anything constexpr) stay in headers so they can inline; cold or bulky functions go in .cpp. Constants used by one .cpp live in that .cpp inside an anonymous namespace; shared constants use inline constexpr.
- Naming: PascalCase types, snake_case functions/variables, trailing underscore for private members, UPPER_CASE constants.
- Board: private pieces_ so invariants are enforced by member functions; read access via const getters.
- set_start_position resets the board first; may later become set_fen(start_fen).
- Layout: src/, tests/, scratch/ (never mixed). Shared code in the engine_core static library. Git: explicit "git add <files>", never "git add .".
- One build directory per configuration. Configure with: cmake -S . -B <dir> -DCMAKE_BUILD_TYPE=<Debug|Release|Sanitize>. Debug for gdb/clangd, Sanitize for correctness runs, Release only for benchmarks.

## Repo state (last commit message, key files)
- Last pushed commit before cleanup: c7ad234 "Point clangd at build-debug". Uncommitted/pending: PROGRESS.md updates, and the cleanup commit "Move cold Board code to board.cpp; rename constants and to_index".
- Key files: CMakeLists.txt, .gitignore, .clangd, .clang-format, PROGRESS.md, src/main.cpp, src/bitboard.h/.cpp, src/types.h, src/board.h, src/board.cpp, tests/test_smoke.cpp, tests/test_bitboard.cpp, tests/test_board.cpp. scratch/ (ignored). Build dirs build-debug/, build-release/, build-san/ (ignored).

## Next step
Run clang-format --dry-run, commit the cleanup, commit PROGRESS.md separately. Then Phase 2: move representation. First concept: what information a chess move must carry (from, to, promotion, special-move flags) and why engines pack it into a 16-bit integer. Task: count the distinct move kinds the 4 spare bits must encode and see whether "is capture?" and "is promotion?" can each be a single bit test.
