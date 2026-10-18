# Chess Engine Progress

## Current phase / milestone
Phase 1: Board representation: COMPLETE.
Housekeeping (build configurations, .clangd, formatting check, header hygiene/naming cleanup): COMPLETE.
Phase 2: Move representation: COMPLETE (16-bit encoding, flag layout, Move class, tests). Committed as 6226104.
Phase 3: attack generation. Milestone 1: knight attack table: COMPLETE and pushed (commit 4cc0010).
Now: Phase 3, milestone 2: king attacks (same table pattern, file-wrap on a/h only), then pawn attacks (color-dependent, captures only).

## Completed and verified (with how it was verified)
- Phase 0 complete: Git 2.53.0, GCC 15.2.0, CMake 4.2.3, clangd 21.1.8, clang-format 21.1.8 in WSL2 (home /home/saad-dev). Verified with --version commands.
- Local Git repo in ~/chess-engine, branch main, identity configured. GitHub via SSH (ed25519, passphrase-protected) as saadnext; origin = git@github.com:saadnext/chess-engine.git. Verified with ssh -T and successful pushes (latest: d9f5d7a..4cc0010).
- Minimal CMake project (C++20, extensions off, -Wall -Wextra -Wpedantic, compile_commands export). Verified: zero-warning build.
- CTest smoke test. Verified: passed, and failed with a deliberate break (reverted).
- .clang-format (LLVM base, 4 spaces, column limit 100, pointers left-aligned), .cache/ and scratch/ ignored. ColumnLimit stays 100: the formatter conforms code to the rule, not the other way round.
- .clangd points CompilationDatabase at build-debug (commit c7ad234). Verified: clangd hover shows signature of square_of in board.cpp.
- Bitboard basics (src/bitboard.h/.cpp): Bitboard = uint64_t; little-endian rank-file mapping (a1=0, h8=63); square_bb, test_bit, square_of (constexpr, assert preconditions); is_valid_square, NUM_SQUARES; print_bb. Verified with tests/test_bitboard.cpp via CTest, deliberate breaks caught and reverted, UBSan shift experiment, assert vs NDEBUG behavior.
- src/types.h: enum class Color and PieceType, constexpr to_index() helpers, NUM_COLORS, NUM_PIECE_TYPES. Verified with static_asserts (and a deliberately wrong one failing).
- src/board.h / src/board.cpp (in engine_core): Board with private pieces_ bitboards, inline hot functions, cold code (print, set_start_position, clear) in board.cpp. Verified with tests/test_board.cpp; seen failing with a deliberate break, reverted and green.
- popcount idiom bb & (bb - 1) derived; std::popcount (C++20, <bit>) used in tests.
- Build configurations, one directory each: build-debug (Debug), build-release (Release: -O3 -DNDEBUG), build-san (Sanitize: -O1 -g -fsanitize=address,undefined). Verified by reading real compile lines. Sanitizer seen catching a planted bug.
- Header hygiene and naming cleanup (cold code to board.cpp, UPPER_CASE constants, index() renamed to_index()), commit 9e12bc2.
- Move encoding (designed from requirements): 16 bits = [flags:4 | to:6 | from:6]. 14 move kinds, 2 spare codes (0110, 0111). bit 3 = promotion, bit 2 = capture, low 2 bits = "which". Promotion piece = (flag & 3) + 1. Verified in scratch/move_bits.cpp.
- Corruption experiment: pack_move(12, 64, FLAG_QUIET) fires the assert in Debug; with -DNDEBUG it silently yields To=0 and flag DOUBLE_PUSH. Asserts are the only guard in Release.
- src/move.h: constants, static_asserts, constexpr pack_move, class Move. Layout static_assert seen failing with FLAG_SHIFT = 11, reverted.
- tests/test_move.cpp via CTest (default move, extreme move, asymmetric move, truth table over all 16 flags, operator==/!=, sizeof == 2). Seen failing with a deliberate break, reverted. check() takes const std::string&.
- Knight attacks (src/attacks.h, header-only): make_knight_table() builds std::array<Bitboard, 64> with 8 shifts per square (<<17, <<15, >>15, >>17, <<10, <<6, >>6, >>10), then masks by SOURCE file: A/B sources AND NOT_FILE_GH, G/H sources AND NOT_FILE_AB. KNIGHT_ATTACKS is inline constexpr (computed at compile time); knight_attacks(sq) is one lookup with an assert. Why masking by source file works: wrapped landings always end on the far side of the board, legal landings never do.
- Knight verification: (1) scratch experiments showing wrap bugs; (2) independent coordinate-based knight_reference() matched on all 64 squares; (3) popcount sum == 336; (4) deliberate break NOT_FILE_GH -> ~FILE_H failed squares 0,8,...,56 (a-file only, since a-file wraps onto g AND h while b-file wraps onto h only) and the 336 sum; (5) deliberate break NOT_FILE_AB -> ~FILE_A failed squares 7,15,...,63 (h-file only, since h-file wraps onto a AND b while g-file wraps onto a only); (6) compile-time static_asserts: a1 -> {c2, b3}, e4 -> 8 targets. Test: tests/test_attacks.cpp via CTest (reference match, sum 336, nonzero exit on failure). Seen failing, reverted, green. ctest 5/5 in Debug, Release, Sanitize; zero warnings.
- bitboard.h additions: FILE_A..FILE_H (inline constexpr), NOT_FILE_AB, NOT_FILE_GH, file_of, rank_of (with asserts), is_valid_square(file, rank) overload (used by square_of), round-trip static_assert(square_of(file_of(37), rank_of(37)) == 37).
- clang-format --dry-run --Werror on src/attacks.h, src/bitboard.h, tests/test_attacks.cpp, tests/test_move.cpp printed nothing (clean).
- Knight milestone committed and pushed: 4cc0010. Verified with git log --oneline -5 (6226104 below it, origin/main == HEAD).
- Lesson: ctest does not rebuild; always run cmake --build before ctest.
- Lesson: a half-right mask passes on one file and fails on its mirror; wraps from the two edge files go to different target files. Only exhaustive comparison against an independent reference finds this.

## Known bugs / open issues
- Run clang-format --dry-run on src/board.h src/board.cpp src/types.h (output not yet seen); empty output closes this.
- NOT_FILE_AB / NOT_FILE_GH are global in bitboard.h (shared for now). Decision: keep unless a later piece shows they should be local to attacks.h; revisit when king/pawn tables exist.
- tests/test_attacks.cpp: the failure message string is built on every iteration even on pass (cheap at 64 squares; build it only on failure if it ever matters).
- tests/test_attacks.cpp: knight_reference is local to this test; reuse or share only if more tables need a coordinate reference.
- Release uses CMake's default -O3. Not yet compared with -O2; decide only with a benchmark (nodes/second).
- Asserts (Board, pack_move, square helpers) are not testable in-process (they abort). Release passing is evidence, not proof (UB can hide at -O3).
- Move has no constructor from a raw uint16_t; add a deliberate explicit one (or named factory) when the transposition table needs to store moves.
- Spare flag codes 0110/0111 are reserved; 0 (a1a1, QUIET) acts as the null/no-move value. Decide a proper NULL_MOVE name later.
- flag_to_string exists only in scratch/move_bits.cpp; move to a move.cpp when needed.
- move.h constants and pack_move are in the global namespace; consider a namespace or making pack_move private later.
- tests/test_move.cpp: static_assert is inside main (move to file scope); loop uses a 2D bool table with magic indices.
- tests/test_board.cpp: per-piece-count check has a generic failure message; duplicate has_piece(Black, King, 60) check; std::string built per check in the empty-board loop.
- NUM_PIECE_TYPES and the enum are not linked automatically; static_assert on King guards the last value.
- piece_on loops over 12 bitboards; a square-to-piece array is the planned upgrade for movegen, to be justified by measurement.
- Magic number 8 in BACK_RANK_ORDER, loops, and file_of/rank_of/is_valid_square; introduce NUM_FILES/NUM_RANKS later.
- Squares are still plain int; a dedicated Square type is planned as its own milestone.

## Design decisions (with reasons)
- Language/standard: C++20, GCC 13+ (or recent Clang), CMake. Warnings -Wall -Wextra -Wpedantic from day one. CMAKE_CXX_EXTENSIONS OFF.
- Project location ~/chess-engine inside WSL2 (not /mnt/c). Environment: Windows 11 + VS Code + Ubuntu on WSL2, 8 GB RAM.
- Dependencies: none until needed. Tests are plain executables registered with CTest (exit 0 = pass).
- Tests use an explicit check helper (never assert). Engine preconditions use assert. static_assert for compile-time checks. A test is not trusted until it has been seen failing.
- Squares are plain int (0..63) for now; a dedicated Square type will be introduced as its own milestone.
- All raw single-square shifts go through square_bb. Set/clear with |= and &= ~, never ^=.
- is_valid_square returns bool so it can later validate untrusted UCI input.
- enum class for Color and PieceType, conversions via to_index(); values contiguous from 0.
- Header vs .cpp rule: tiny hot functions (and anything constexpr) stay in headers so they can inline; cold or bulky functions go in .cpp. Constants used by one .cpp live in that .cpp in an anonymous namespace; shared constants use inline constexpr.
- Naming: PascalCase types, snake_case functions/variables, trailing underscore for private members, UPPER_CASE constants. Prefixes: FLAG_* flag codes, *_BIT test bits, *_SHIFT / *_MASK layout.
- Move is 16 bits and carries only from, to, flags; moving and captured pieces are looked up on the board at make_move time. Flags are independent yes/no bits so "is capture?" and "is promotion?" are single bit tests. QUIET = 0 means "nothing special".
- Move is a wrapper class (not a bare uint16_t): same size, zero runtime cost, but type-safe. Explicit constructor, no implicit conversion from integers.
- Board: private pieces_ so invariants are enforced by member functions; read access via const getters.
- set_start_position resets the board first; may later become set_fen(start_fen).
- Attack tables: non-sliding pieces use precomputed constexpr tables built by a constexpr builder function, stored as inline constexpr std::array, accessed through a small constexpr lookup function with an assert. Geometry constants (files) live in bitboard.h because pawns, king safety and evaluation will also need them. Table builders are verified against an independent coordinate-based reference, never against themselves.
- Layout: src/, tests/, scratch/ (never mixed). Shared code in the engine_core static library (attacks.h is header-only, so no library change). Git: explicit "git add <files>", never "git add .".
- One build directory per configuration. Configure with: cmake -S . -B <dir> -DCMAKE_BUILD_TYPE=<Debug|Release|Sanitize>. Debug for gdb/clangd, Sanitize for correctness runs, Release only for benchmarks.

## Repo state (last commit message, key files)
- Last commit: 4cc0010 "Add knight attack table (constexpr), reference test, file constants". Pushed; origin/main == HEAD. Earlier: d9f5d7a "Update PROGRESS.md: Move representation complete", 6226104 "Add Move: ...", 1de7ed7, 9e12bc2.
- Working tree: clean except this PROGRESS.md update (commit it).
- Key files: CMakeLists.txt, .gitignore, .clangd, .clang-format, PROGRESS.md, src/main.cpp, src/bitboard.h/.cpp, src/types.h, src/board.h, src/board.cpp, src/move.h, src/attacks.h, tests/test_smoke.cpp, tests/test_bitboard.cpp, tests/test_board.cpp, tests/test_move.cpp, tests/test_attacks.cpp. scratch/ (ignored; contains move_bits.cpp, wrap.cpp). Build dirs build-debug/, build-release/, build-san/ (ignored).

## Next step
Phase 3, milestone 2: king attacks. Concept: the king is the knight's simpler cousin (8 one-step neighbours), but the wrap problem changes shape: only the a-file and h-file need masking, and the offsets are +-1, +-7, +-8, +-9. Pending question (answer in own words at the start of the new chat): which 3 of the 8 king offsets can wrap off the a-file, which 3 off the h-file, and which mask goes with each?
