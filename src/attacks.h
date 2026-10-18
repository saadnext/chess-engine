#pragma once
#include "bitboard.h"
#include <array>
#include <cassert>

constexpr std::array<Bitboard, NUM_SQUARES> make_knight_table() {
    std::array<Bitboard, NUM_SQUARES> table{};
    for (int sq_idx = 0; sq_idx < NUM_SQUARES; sq_idx++) {
        Bitboard bb{};
        const Bitboard sq_bb = square_bb(sq_idx);
        bb |= sq_bb << 17; // Up:2,Right:1
        bb |= sq_bb << 15; // Up:2,Left:1
        bb |= sq_bb >> 15; // Down:2,Right:1
        bb |= sq_bb >> 17; // Down:2,Left:1

        bb |= sq_bb << 10; // Up:1,Right:2
        bb |= sq_bb << 6;  // Up:1,Left:2
        bb |= sq_bb >> 6;  // Down:1,Right:2
        bb |= sq_bb >> 10; // Down:1,Left:2

        if ((sq_bb & (FILE_A | FILE_B)) != 0) {
            bb &= NOT_FILE_GH;
        } else if ((sq_bb & (FILE_H | FILE_G)) != 0) {
            bb &= NOT_FILE_AB;
        }
        table[sq_idx] = bb;
    }
    return table;
}

inline constexpr auto KNIGHT_ATTACKS = make_knight_table();

constexpr Bitboard knight_attacks(int square_index) {
    assert(is_valid_square(square_index));
    return KNIGHT_ATTACKS[square_index];
}

static_assert(knight_attacks(0) == (square_bb(10) | square_bb(17))); // a1: c2, b3
static_assert(knight_attacks(28) ==
              (square_bb(22) | square_bb(38) | square_bb(18) | square_bb(34) | square_bb(45) |
               square_bb(43) | square_bb(13) | square_bb(11)));
