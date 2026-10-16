# Chess Engine Progress

## Current phase / milestone
Phase 1: Board representation: COMPLETE.
Housekeeping (build configurations, .clangd, formatting check, header hygiene/naming cleanup): COMPLETE.
Phase 2: Move representation: COMPLETE (16-bit encoding, flag layout, Move class, tests). Committed as 6226104.
Now: Phase 3: attack generation. First milestone: knight attack table (precomputed lookup per square).

## Completed and verified (with how it was verified)
- Phase 0 complete: Git 2.53.0, GCC 15.2.0, CMake 4.2.3, clangd 21.1.8, clang-format 21.1.8 in WSL2 (home /home/saad-dev). Verified with --version commands.
- Local Git repo in ~/chess-engine, branch main, identity configured. GitHub via SSH (ed25519) as saadnext; origin = git@github.com:saadnext/chess-engine.git. Verified with ssh -T and successful pushes.
- Minimal CMake project (C++20, extensions off, -Wall -Wextra -Wpedantic, compile_commands export). Verified: zero-warning build.
- CTest smoke test. Verified: passed, and failed with a deliberate break (reverted).
- .clang-format (LLVM base, 4 spaces, column limit 100, pointers left-aligned), .cache/ and scratch/ ignored.
- .clangd points CompilationDatabase at build-debug (commit c7ad234). Verified: clangd hover shows signature of square_of in board.cpp.
- Bitboard basics (src/bitboard.h/.cpp): Bitboard = uint64_t; little-endian rank-file mapping (a1=0, h8=63); square_bb, test_bit, square_of (constexpr, assert preconditions); is_valid_square, NUM_SQUARES; print_bb. Verified with tests/test_bitboard.cpp via CTest, deliberate breaks caught and reverted, UBSan shift experiment, assert vs NDEBUG behavior.
- src/types.h: enum class Color and PieceType, constexpr to_index() helpers, NUM_COLORS, NUM_PIECE_TYPES. Verified with static_asserts (and a deliberately wrong one failing).
- src/board.h / src/board.cpp (in engine_core): Board with private pieces_ bitboards, inline hot functions, cold code (print, set_start_position, clear) in board.cpp. Verified with tests/test_board.cpp (empty board, put/remove, has_piece, piece_on, start position counts and spot checks); seen failing with a deliberate break, reverted and green.
- popcount idiom bb & (bb - 1) derived; std::popcount (C++20, <bit>) used in tests.
- Build configurations, one directory each: build-debug (Debug), build-release (Release: -O3 -DNDEBUG), build-san (Sanitize: -O1 -g -fsanitize=address,undefined). Verified by reading real compile lines. Sanitizer seen catching a planted bug.
- Header hygiene and naming cleanup (cold code to board.cpp, UPPER_CASE constants, index() renamed to_index()), commit 9e12bc2. Verified: rebuilt and ctest 3/3 in all configurations, grep and git diff --stat checked.
- Move encoding (designed from requirements, not copied): 16 bits = [flags:4 | to:6 | from:6]. 14 move kinds, 2 spare codes (0110, 0111). Flags are independent yes/no bits: bit 3 = promotion (PROMOTION_BIT 0b1000), bit 2 = capture (CAPTURE_BIT 0b0100), low 2 bits = "which". Codes: QUIET 0000, DOUBLE_PUSH 0001, CASTLE_KING 0010, CASTLE_QUEEN 0011, CAPTURE 0100, EN_PASSANT 0101, PROMOTE N/B/R/Q 1000-1011, PROMOTE-capture N/B/R/Q 1100-1111. Promotion piece = (flag & 3) + 1 (matches PieceType Knight=1..Queen=4). Verified in scratch/move_bits.cpp: pack/unpack output for 10 sample moves and all 16 codes, is_capture/is_promotion truth table matched by hand.
- Corruption experiment: pack_move(12, 64, FLAG_QUIET) fires the assert in Debug; with -DNDEBUG it silently yields To=0 and flag DOUBLE_PUSH (bit 12 set). Confirms asserts are the only guard in Release.
- src/move.h: inline constexpr layout/mask/test-bit/flag constants, static_asserts (FLAG_MASK == 15; test bits disjoint; QUEEN_ON_CAPTURE has both bits; layout covers exactly 0xFFFF; KNIGHT | CAPTURE_BIT == KNIGHT_ON_CAPTURE), constexpr pack_move (asserts via is_valid_square and flag <= FLAG_MASK), class Move (private move_, constexpr default ctor = 0, explicit constexpr ctor (from, to, flag), from/to/flag/is_capture/is_promotion, defaulted operator==). Layout static_assert seen failing with FLAG_SHIFT = 11 (32767 == 65535), reverted to 12.
- tests/test_move.cpp via CTest: default move, (63,63,PROMOTE_QUEEN_ON_CAPTURE), asymmetric (12,28,DOUBLE_PUSH), is_capture/is_promotion truth table over all 16 codes with per-code failure message, operator== and !=, static_assert(sizeof(Move) == 2). Seen failing with a deliberate break (expected[0][1] changed to 1), reverted; final run green: ctest 4/4 in Debug, Release and Sanitize.
- clang-format --dry-run on src/move.h and tests/test_move.cpp printed nothing (already formatted). Move commit 6226104 contains src/move.h, tests/test_move.cpp, CMakeLists.txt.
- Lesson: ctest does not rebuild; always run cmake --build before ctest.

## Known bugs / open issues
- Run clang-format --dry-run on src/board.h src/board.cpp src/types.h (output not yet seen); empty output closes this.
- Commit 6226104 is local; push to origin (origin/main was at 1de7ed7) once confirmed.
- Release uses CMake's default -O3. Not yet compared with -O2; decide only with a benchmark (nodes/second).
- Asserts (Board, pack_move) are not testable in-process (they abort). Release passing is evidence, not proof (UB can hide at -O3).
- Move has no constructor from a raw uint16_t; add a deliberate, explicit one (or a named factory) when the transposition table needs to store moves.
- Spare flag codes 0110/0111 are reserved; 0 (a1a1, QUIET) acts as the null/no-move value. Decide a proper NULL_MOVE name later.
- flag_to_string exists only in scratch/move_bits.cpp; move it to a move.cpp (cold debug code) when needed for debugging output.
- move.h constants and pack_move are in the global namespace; consider a namespace or making pack_move a private detail of Move later.
- tests/test_move.cpp: static_assert is inside main (move to file scope); check() takes std::string by value (use const std::string&, or build the message only on failure); loop uses a 2D bool table with magic indices.
- tests/test_board.cpp: per-piece-count check has a generic failure message; duplicate has_piece(Black, King, 60) check; std::string built per check in the empty-board loop.
- NUM_PIECE_TYPES and the enum are not linked automatically; static_assert on King guards the last value.
- piece_on loops over 12 bitboards; a square-to-piece array is the planned upgrade for movegen, to be justified by measurement.
- Magic number 8 in BACK_RANK_ORDER and loops; introduce NUM_FILES/NUM_RANKS later.

## Design decisions (with reasons)
- Language/standard: C++20, GCC 13+ (or recent Clang), CMake. Warnings -Wall -Wextra -Wpedantic from day one. CMAKE_CXX_EXTENSIONS OFF.
- Project location ~/chess-engine inside WSL2 (not /mnt/c). Environment: Windows 11 + VS Code + Ubuntu on WSL2, 8 GB RAM.
- Dependencies: none until needed. Tests are plain executables registered with CTest (exit 0 = pass).
- Tests use an explicit check helper (never assert). Engine preconditions use assert. static_assert for compile-time checks. A test is not trusted until it has been seen failing.
- Squares are plain int (0..63) for now; a dedicated Square type will be introduced when attack/move generation starts, as its own milestone.
- All raw single-square shifts go through square_bb. Set/clear with |= and &= ~, never ^=.
- is_valid_square returns bool so it can later validate untrusted UCI input.
- enum class for Color and PieceType, conversions via to_index(); values contiguous from 0.
- Header vs .cpp rule: tiny hot functions (and anything constexpr) stay in headers so they can inline; cold or bulky functions go in .cpp. Constants used by one .cpp live in that .cpp in an anonymous namespace; shared constants use inline constexpr.
- Naming: PascalCase types, snake_case functions/variables, trailing underscore for private members, UPPER_CASE constants. Constants grouped by prefix: FLAG_* for flag codes, *_BIT for test bits, *_SHIFT / *_MASK for layout.
- Move is 16 bits and carries only from, to, flags; moving and captured pieces are looked up on the board at make_move time (no redundancy). Flags are independent yes/no bits, not a counter, so "is capture?" and "is promotion?" are single bit tests. Flag codes are compared with ==, test bits are used with &. QUIET = 0 means "nothing special".
- Move is a wrapper class (not a bare uint16_t): same size (sizeof == 2), zero runtime cost, but passing a square where a move is expected is a compile error. Constructor is explicit and there is no implicit conversion from integers.
- Board: private pieces_ so invariants are enforced by member functions; read access via const getters.
- set_start_position resets the board first; may later become set_fen(start_fen).
- Layout: src/, tests/, scratch/ (never mixed). Shared code in the engine_core static library. Git: explicit "git add <files>", never "git add .".
- One build directory per configuration. Configure with: cmake -S . -B <dir> -DCMAKE_BUILD_TYPE=<Debug|Release|Sanitize>. Debug for gdb/clangd, Sanitize for correctness runs, Release only for benchmarks.

## Repo state (last commit message, key files)
- Last commit: 6226104 "Add Move: 16-bit encoding with flags, constexpr Move class, tests" (local; not yet pushed, origin/main = 1de7ed7 "Update PROGRESS.md: header cleanup complete"). Earlier: 9e12bc2 "Move cold Board code to board.cpp; rename constants and to_index". Working tree clean (git status --short printed nothing) before this PROGRESS.md update.
- Key files: CMakeLists.txt (includes test_move target), .gitignore, .clangd, .clang-format, PROGRESS.md, src/main.cpp, src/bitboard.h/.cpp, src/types.h, src/board.h, src/board.cpp, src/move.h, tests/test_smoke.cpp, tests/test_bitboard.cpp, tests/test_board.cpp, tests/test_move.cpp. scratch/ (ignored; contains move_bits.cpp). Build dirs build-debug/, build-release/, build-san/ (ignored).

## Next step
Phase 3: attack generation. First concept: why "what does a knight on square X attack?" should be a single table lookup, and how to derive the 64 attack bitboards with shifts and masks (including the file-wrap problem at the board edges). Pending question: in one sentence, what must be precomputed so that a knight's attacks from any square are one lookup?
