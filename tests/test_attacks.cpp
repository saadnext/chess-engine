#include "attacks.h"
#include <bit>
#include <iostream>
#include <string>

static int failures = 0;
static void check(bool ok, const std::string& what) {
    if (!ok) {
        std::cerr << "FAIL: " << what << "\n";
        ++failures;
    }
}

Bitboard knight_reference(int square_index) {
    constexpr int DF[8] = {1, 2, 2, 1, -1, -2, -2, -1};
    constexpr int DR[8] = {2, 1, -1, -2, -2, -1, 1, 2};
    Bitboard expected_jumps{};
    int file = file_of(square_index);
    int rank = rank_of(square_index);
    for (int j = 0; j < 8; ++j) {
        int f = file + DF[j];
        int r = rank + DR[j];
        if (f >= 0 && f < 8 && r >= 0 && r < 8) {
            int square_idx = square_of(f, r);
            expected_jumps |= square_bb(square_idx);
        }
    }
    return expected_jumps;
}

int main() {
    int total_knight_jumps_count = 0;
    for (int sq = 0; sq < NUM_SQUARES; ++sq) {
        Bitboard expected_jumps = knight_reference(sq);
        Bitboard knight_jumps = knight_attacks(sq);
        total_knight_jumps_count += std::popcount(knight_jumps);
        std::string what =
            "expected_jumps != knight_jumps on this square_index: " + std::to_string(sq);
        check(expected_jumps == knight_jumps, what);
    }
    check(total_knight_jumps_count == 336, "total_knight_jumps_count == 336");
    return failures == 0 ? 0 : 1;
}
