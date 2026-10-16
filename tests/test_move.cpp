#include "move.h"
#include <bitset>
#include <iostream>
#include <string>

static int failures = 0;
static void check(bool ok, std::string what) {
    if (!ok) {
        std::cerr << "FAIL: " << what << "\n";
        ++failures;
    }
}

int main() {
    static_assert(sizeof(Move) == 2);
    Move move_1;
    check(move_1.from() == 0 && move_1.to() == 0 && move_1.flag() == FLAG_QUIET,
          "move_1.from() == 0 && move_1.to() == 0 && move_1.flag() == FLAG_QUIET");
    Move move_2(63, 63, FLAG_PROMOTE_QUEEN_ON_CAPTURE);
    check(move_2.from() == 63 && move_2.to() == 63 &&
              move_2.flag() == FLAG_PROMOTE_QUEEN_ON_CAPTURE,
          "move_2.from() == 63 && move_2.to() == 63 && move_2.flag() == "
          "FLAG_PROMOTE_QUEEN_ON_CAPTURE");
    Move move_3(12, 28, FLAG_DOUBLE_PUSH);
    check(move_3.from() == 12 && move_3.to() == 28 && move_3.flag() == FLAG_DOUBLE_PUSH,
          "move_3.from() == 12 && move_3.to() == 28 && move_3.flag() == FLAG_DOUBLE_PUSH");
    bool expected[16][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0},
                            {0, 1}, {0, 1}, {0, 1}, {0, 1}, {1, 1}, {1, 1}, {1, 1}, {1, 1}};
    for (std::uint16_t fg = 0; fg < 16; ++fg) {
        Move move_i(12, 28, fg);
        std::string what = "Flag: " + std::bitset<4>(fg).to_string() +
                           " | Not expected is_capture or is_promotion";
        check(move_i.is_capture() == expected[fg][0] && move_i.is_promotion() == expected[fg][1],
              what);
    }
    Move test_1(29, 37, FLAG_QUIET);
    Move test_2(29, 37, FLAG_QUIET);
    Move test_3(29, 45, FLAG_QUIET);
    check(test_1 == test_2, "test_1 == test_2");
    check(test_1 != test_3, "test_1 != test_3");
    return failures == 0 ? 0 : 1;
}
