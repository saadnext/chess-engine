#include "attacks.h"
#include "bitboard.h"
#include <bit>
#include <iostream>
#include <span>
#include <string>

static int failures = 0;
static void check(bool ok, const std::string& what) {
    if (!ok) {
        std::cerr << "FAIL: " << what << "\n";
        ++failures;
    }
}

Bitboard piece_reference(int square_index, std::span<const int, 8> DF, std::span<const int, 8> DR) {
    Bitboard expected_sqs{};
    int file = file_of(square_index);
    int rank = rank_of(square_index);
    for (int j = 0; j < 8; ++j) {
        int f = file + DF[j];
        int r = rank + DR[j];
        if (is_valid_square(f, r)) {
            int square_idx = square_of(f, r);
            expected_sqs |= square_bb(square_idx);
        }
    }
    return expected_sqs;
}

Bitboard knight_reference(int square_index) {
    constexpr int DF[8] = {1, 2, 2, 1, -1, -2, -2, -1};
    constexpr int DR[8] = {2, 1, -1, -2, -2, -1, 1, 2};
    return piece_reference(square_index, DF, DR);
}

Bitboard king_reference(int square_index) {
    constexpr int DF[8] = {1, 1, 1, -1, -1, -1, 0, 0};
    constexpr int DR[8] = {0, 1, -1, 0, 1, -1, 1, -1};
    return piece_reference(square_index, DF, DR);
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
    int total_king_sqs_count = 0;
    for (int sq = 0; sq < NUM_SQUARES; ++sq) {
        Bitboard expected_sqs = king_reference(sq);
        Bitboard king_sqs = king_attacks(sq);
        total_king_sqs_count += std::popcount(king_sqs);
        std::string what = "expected_sqs != king_sqs on this square_index: " + std::to_string(sq);
        check(expected_sqs == king_sqs, what);
    }
    check(total_king_sqs_count == 420, "total_king_sqs_count == 420");
    return failures == 0 ? 0 : 1;
}
