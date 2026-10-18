#include "attacks.h"
#include "bitboard.h"
#include "types.h"
#include <bit>
#include <cstddef>
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

Bitboard piece_reference(int square_index, std::span<const int> df, std::span<const int> dr) {
    Bitboard expected_sqs{};
    int file = file_of(square_index);
    int rank = rank_of(square_index);
    for (std::size_t j = 0; j < df.size(); ++j) {
        int f = file + df[j];
        int r = rank + dr[j];
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

Bitboard pawn_reference(Color color, int square_index) {
    // clang-format off
    constexpr int OFFSETS_PER_COLOR[NUM_COLORS][2][2] = {
        { //color white 
            {1,-1}, // DF
            {1,1}  // DR
        },
        { // color black
            {1,-1},  // DF
            {-1,-1} // DR
        }
    };
    // clang-format on
    return piece_reference(square_index, OFFSETS_PER_COLOR[to_index(color)][0],
                           OFFSETS_PER_COLOR[to_index(color)][1]);
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

    int w_total_pawn_sqs_count = 0;
    int b_total_pawn_sqs_count = 0;
    for (int sq = 0; sq < NUM_SQUARES; ++sq) {
        // for white pawns
        Bitboard w_expected_sqs = pawn_reference(Color::White, sq);
        Bitboard w_pawn_sqs = pawn_attacks(Color::White, sq);
        w_total_pawn_sqs_count += std::popcount(w_pawn_sqs);
        std::string what =
            "w_expected_sqs != w_pawn_sqs on this square_index: " + std::to_string(sq);
        check(w_expected_sqs == w_pawn_sqs, what);
        // for black pawns
        Bitboard b_expected_sqs = pawn_reference(Color::Black, sq);
        Bitboard b_pawn_sqs = pawn_attacks(Color::Black, sq);
        b_total_pawn_sqs_count += std::popcount(b_pawn_sqs);
        what = "b_expected_sqs != b_pawn_sqs on this square_index: " + std::to_string(sq);
        check(b_expected_sqs == b_pawn_sqs, what);
    }
    check(w_total_pawn_sqs_count == 98, "w_total_pawn_sqs_count == 98");
    check(b_total_pawn_sqs_count == 98, "b_total_pawn_sqs_count == 98");

    return failures == 0 ? 0 : 1;
}
