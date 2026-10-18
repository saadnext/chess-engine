# Chess Engine Progress

## Current phase / milestone
Phase 1: Board representation: COMPLETE.
Housekeeping (build configurations, .clangd, formatting check, header hygiene/naming cleanup): COMPLETE.
Phase 2: Move representation: COMPLETE (16-bit encoding, flag layout, Move class, tests). Committed as 6226104.
Phase 3: attack generation.
  Milestone 1: knight attack table: COMPLETE and pushed (4cc0010).
  Milestone 2: king attack table: COMPLETE and pushed (4c89268).
  Milestone 3: pawn attack table: COMPLETE and pushed (40a9f51).
Now: Phase 3, milestone 4: sliding-piece attacks (rook/bishop/queen). Start with the concept of ray attacks and blockers (classical ray approach) before magic bitboards.

## Completed and verified (with how it was verified)
- Phase 0 complete: Git 2.53.0, GCC 15.2.0, CMake 4.2.3, clangd 21.1.8, clang-format 21.1.8 in WSL2 (home /home/saad-dev). Verified with --version commands.
- Local Git repo in ~/chess-engine, branch main, identity configured. GitHub via SSH (ed25519, passphrase-protected) as saadnext; origin = git@github.com:saadnext/chess-engine.git. Verified with ssh -T and successful pushes (latest: 3ceadc8..4c89268).
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
- Knight verification: (1) scratch experiments showing wrap bugs; (2) independent coordinate-based reference matched on all 64 squares; (3) popcount sum == 336; (4) deliberate break NOT_FILE_GH -> ~FILE_H failed squares 0,8,...,56 (a-file only, since a-file wraps onto g AND h while b-file wraps onto h only) and the 336 sum; (5) deliberate break NOT_FILE_AB -> ~FILE_A failed squares 7,15,...,63 (h-file only, since h-file wraps onto a AND b while g-file wraps onto a only); (6) compile-time static_asserts: a1 -> {c2, b3}, e4 -> 8 targets. ctest 5/5 in Debug, Release, Sanitize; zero warnings.
- bitboard.h additions: FILE_A..FILE_H (inline constexpr), NOT_FILE_A, NOT_FILE_H, NOT_FILE_AB, NOT_FILE_GH, file_of, rank_of (with asserts), is_valid_square(file, rank) overload (used by square_of), round-trip static_assert(square_of(file_of(37), rank_of(37)) == 37).
- King attacks (src/attacks.h): make_king_table() builds the vertical pair (sq>>8 | sq<<8), then west = (sq>>1)|(vertical>>1) and east = (sq<<1)|(vertical<<1) (three offsets each: -1,+7,-9 west; +1,-7,+9 east), and masks by DESTINATION: west & NOT_FILE_H (west wraps from the a-file land on the h-file), east & NOT_FILE_A. Vertical needs no mask (unsigned shifts drop bits off the ends, well-defined). KING_ATTACKS inline constexpr; king_attacks(sq) = one lookup with an assert.
- King verification: (1) first version missed the west mask: the a1 static_assert failed at compile time (898 == 770, difference 128 = bit 7 = h1), and with the assert temporarily commented out the runtime test failed on exactly squares 0,8,...,56 (a-file sources) and the 420 sum; (2) fixed; (3) coordinate-based king reference matched on all 64 squares; (4) popcount sum == 420 (4 corners x 3 + 24 edges x 5 + 36 interior x 8); (5) deliberate break (masks swapped) failed squares on files a, b, g, h only plus the 420 sum (a/h sources wrongly keep wraps, b/g sources wrongly lose legal landings; files c-f unaffected); reverted and green; (6) compile-time static_asserts: a1 -> {b1, a2, b2}, e4 -> 8 neighbours. clang-format --dry-run --Werror on src/attacks.h, src/bitboard.h, tests/test_attacks.cpp: empty (clean). ctest 5/5 in Debug, Release, Sanitize. Committed and pushed: 4c89268 (origin/main == HEAD, verified with git log).
- Pawn attacks (src/attacks.h): make_pawn_table() builds std::array<std::array<Bitboard, NUM_SQUARES>, NUM_COLORS>, indexed [color][square] with to_index(Color::White) / to_index(Color::Black). White: (sq<<7) & NOT_FILE_H | (sq<<9) & NOT_FILE_A. Black: (sq>>7) & NOT_FILE_A | (sq>>9) & NOT_FILE_H. Destination masking (each shift wraps onto exactly one far file); left-moving shifts use NOT_FILE_H, right-moving use NOT_FILE_A, for both colors. Black is White mirrored vertically. No rank masks needed: unsigned shifts drop bits off the ends. PAWN_ATTACKS inline constexpr; pawn_attacks(Color, sq) = one lookup with an assert. Captures only (pushes are not attacks). Table is filled for all 64 squares (rank 8 for White and rank 1 for Black give 0).
- Pawn verification: (1) the answer to the pending question (shifts and masks) derived by hand and confirmed against the geometry; (2) independent coordinate-based pawn_reference (2 offsets per color) via the shared piece_reference, matched on all 64 squares for both colors; (3) popcount sum == 98 per color, derived two ways: 14 edge-file squares x 1 + 42 interior squares x 2 = 98, and 7 ranks x 8 files x 2 = 112 minus 14 off-edge = 98; (4) deliberate break (White west shift << 7 changed to >> 7): compile-time static_asserts failed first (131074 == 131072), then with them commented out the runtime test failed on squares 1-61 and 63 (every square except 0 and 62, which matched by coincidence) and the 98 sum: a wrong shift direction fails everywhere, whereas a wrong mask fails on edge files only; reverted and green; (5) compile-time static_asserts: White a2 -> {b3}, Black a7 -> {b6}, White e4 -> {d5, f5}, Black e4 -> {d3, f3}; (6) ctest 5/5 in Debug, Release, Sanitize; zero warnings. clang-format --dry-run --Werror on src/attacks.h and tests/test_attacks.cpp: empty (clean).
- Pawn-table compile-time bug found and fixed: outer array was sized with NUM_COLORS in the loop bound instead of NUM_SQUARES, giving an out-of-range index at constant evaluation; GCC reported it via __glibcxx_assert_fail (libstdc++ assertions enabled). Diagnosis: the error means constant evaluation REACHED a failing assertion, i.e. a real bug in the builder, not a GCC limitation.
- tests/test_attacks.cpp: shared piece_reference(square, span<const int> df, span<const int> dr) (dynamic extent, uses df.size()) used by knight_reference, king_reference and pawn_reference.
- Lesson: ctest does not rebuild; always run cmake --build before ctest.
- Lesson: a half-right mask passes on one file and fails on its mirror; wraps from the two edge files go to different target files. Only exhaustive comparison against an independent reference finds this.
- Lesson: a failing static_assert stops the build, so it cannot show the per-square failure list; temporarily comment it out to see the runtime test, then restore it.
- Lesson: constexpr evaluation is a free UB detector (out-of-bounds, bad shifts become compile errors). Read the "in constexpr expansion of" trace first.
- Lesson: failure pattern is a diagnostic: fails on every square -> wrong shift; fails on edge files only -> wrong mask.
- clang-format --dry-run --Werror on src/board.h, src/board.cpp, src/types.h: empty (clean).

## Known bugs / open issues
- pawn_reference stores offsets as [color][DF/DR][2]; consider two named arrays per color for readability (style only).
- Table builders: loop bounds and table dimensions can drift apart (caused the NUM_COLORS/NUM_SQUARES bug); consider range-for or table.size() as the bound.
- tests/test_attacks.cpp: parameters DF/DR in knight/king references are in constant style; consider df/dr to match piece_reference.
- tests/test_attacks.cpp: failure message strings are built on every iteration even on pass (cheap at 64 squares; build only on failure if it ever matters).
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
- File-mask constants (NOT_FILE_A, NOT_FILE_H, NOT_FILE_AB, NOT_FILE_GH) stay global in bitboard.h: pawns are the second user and pawn-structure evaluation will need them too. Decision closed.
- Wrap masking: knight masks by SOURCE file (wraps land on two files); king and pawns mask by DESTINATION file (each shift wraps onto exactly one far file). Choose per piece by asking where a wrapped landing can end up.
- Pawn attacks are a separate table from pawn pushes: attacks are captures only and are color-dependent, indexed [color][square].
- Layout: src/, tests/, scratch/ (never mixed). Shared code in the engine_core static library (attacks.h is header-only, so no library change). Git: explicit "git add <files>", never "git add .".
- One build directory per configuration. Configure with: cmake -S . -B <dir> -DCMAKE_BUILD_TYPE=<Debug|Release|Sanitize>. Debug for gdb/clangd, Sanitize for correctness runs, Release only for benchmarks.

## Repo state (last commit message, key files)
- Last commit: 40a9f51 "Add pawn attack table (constexpr), pawn reference test". Pushed; origin/main == HEAD. Earlier: 4c89268, 3ceadc8, 4cc0010, d9f5d7a, 6226104.
- Key files: CMakeLists.txt, .gitignore, .clangd, .clang-format, PROGRESS.md, src/main.cpp, src/bitboard.h/.cpp, src/types.h, src/board.h, src/board.cpp, src/move.h, src/attacks.h, tests/test_smoke.cpp, tests/test_bitboard.cpp, tests/test_board.cpp, tests/test_move.cpp, tests/test_attacks.cpp. scratch/ (ignored; contains move_bits.cpp, wrap.cpp). Build dirs build-debug/, build-release/, build-san/ (ignored).

## Next step
Phase 3, milestone 4: sliding-piece attacks. Concept: a rook on an empty board attacks its whole rank and file, but blockers stop the ray, so the attack set depends on occupancy. Pending question (answer in own words at the start of the new chat): why can't a sliding piece use a single table indexed by square alone, the way the knight, king and pawn do?
