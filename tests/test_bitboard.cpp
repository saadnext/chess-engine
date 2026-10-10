#include "bitboard.h"
#include <iostream>

static int failures = 0;
static void check(bool ok, const char* what) {
    if (!ok) {
        std::cerr << "FAIL: " << what << "\n";
        ++failures;
    }
}

int main() {
    Bitboard bb = 0;
    bb |= square_bb(28); // setting e4
    check(square_bb(0) == 1, "square_bb(0) == 1");
    check(square_bb(28) == 0x10000000ULL, "square_bb(28) == 0x10000000ULL");
    check(square_bb(63) == 0x8000000000000000ULL, "square_bb(63) == 0x8000000000000000ULL");

    check(test_bit(bb, 28), "test_bit(bb, 28)");
    check(!test_bit(bb, 27), "!test_bit(bb, 27)");
    check(!test_bit(bb, 29), "!test_bit(bb, 29)");
    check(!test_bit(bb, 63), "!test_bit(bb, 63)");

    check(square_of(4, 3) == 28, "square_of(4,3) == 28");
    check(square_of(0, 0) == 0, "square_of(0,0) == 0");
    check(square_of(7, 0) == 7, "square_of(7,0) == 7");
    check(square_of(7, 7) == 63, "square_of(7,7) == 63");

    check(is_valid_square(0), "is_valid_square(0)");
    check(is_valid_square(63), "is_valid_square(63)");
    check(!is_valid_square(64), "is_valid_square(64)");
    check(!is_valid_square(-1), "!is_valid_square(-1)");
    return failures == 0 ? 0 : 1;
}
