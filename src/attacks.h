#pragma once
#include "bitboard.h"
#include "types.h"
#include <array>
#include <cassert>

constexpr std::array<Bitboard, NUM_SQUARES> make_king_table() {
    std::array<Bitboard, NUM_SQUARES> table{};
    for (int sq_idx = 0; sq_idx < NUM_SQUARES; sq_idx++) {
        const Bitboard sq_bb = square_bb(sq_idx);
        const Bitboard vertical = (sq_bb >> 8) | (sq_bb << 8);
        const Bitboard west = (sq_bb >> 1) | (vertical >> 1);
        const Bitboard east = (sq_bb << 1) | (vertical << 1);
        table[sq_idx] = (west & NOT_FILE_H) | (east & NOT_FILE_A) | vertical;
    }
    return table;
}

inline constexpr auto KING_ATTACKS = make_king_table();

constexpr Bitboard king_attacks(int square_index) {
    assert(is_valid_square(square_index));
    return KING_ATTACKS[square_index];
}
static_assert(king_attacks(0) == (square_bb(1) | square_bb(8) | square_bb(9)));
static_assert(king_attacks(28) == (square_bb(27) | square_bb(29) | square_bb(20) | square_bb(36) |
                                   square_bb(19) | square_bb(21) | square_bb(35) | square_bb(37)));

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

constexpr std::array<std::array<Bitboard, NUM_SQUARES>, NUM_COLORS> make_pawn_table() {
    std::array<std::array<Bitboard, NUM_SQUARES>, NUM_COLORS> table{};
    for (int sq_idx = 0; sq_idx < NUM_SQUARES; sq_idx++) {
        // for white pawns
        const Bitboard sq_bb = square_bb(sq_idx);
        const Bitboard w_west = sq_bb << 7;
        const Bitboard w_east = sq_bb << 9;
        table[to_index(Color::White)][sq_idx] = (w_west & NOT_FILE_H) | (w_east & NOT_FILE_A);
        // for black pawns
        const Bitboard b_west = sq_bb >> 7;
        const Bitboard b_east = sq_bb >> 9;
        table[to_index(Color::Black)][sq_idx] = (b_west & NOT_FILE_A) | (b_east & NOT_FILE_H);
    }
    return table;
}

inline constexpr auto PAWN_ATTACKS = make_pawn_table();

constexpr Bitboard pawn_attacks(Color color, int square_index) {
    assert(is_valid_square(square_index));
    return PAWN_ATTACKS[to_index(color)][square_index];
}
static_assert(pawn_attacks(Color::White, 8) == square_bb(17));
static_assert(pawn_attacks(Color::Black, 48) == square_bb(41));
static_assert(pawn_attacks(Color::White, 28) == (square_bb(37) | square_bb(35)));
static_assert(pawn_attacks(Color::Black, 28) == (square_bb(19) | square_bb(21)));
