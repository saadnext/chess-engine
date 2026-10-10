#pragma once
#include <cassert>
#include <cstdint>

using Bitboard = std::uint64_t;
constexpr int NUM_SQUARES = 64;

constexpr bool is_valid_square(int square_index) {
    return square_index >= 0 && square_index < NUM_SQUARES;
}

constexpr Bitboard square_bb(int square_index) {
    assert(is_valid_square(square_index));
    return std::uint64_t{1} << square_index;
}
constexpr int square_of(int file, int rank) {
    assert(file >= 0 && file < 8);
    assert(rank >= 0 && rank < 8);
    return rank * 8 + file;
}
constexpr bool test_bit(Bitboard bb, int square_index) {
    assert(is_valid_square(square_index));
    return ((bb >> square_index) & 1) != 0;
}

void print_bb(Bitboard bb);
