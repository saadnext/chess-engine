#pragma once
#include <cassert>
#include <cstdint>

using Bitboard = std::uint64_t;

constexpr Bitboard square_bb(int square_index) {
    assert(square_index >= 0 && square_index < 64);
    return std::uint64_t{1} << square_index;
}
constexpr int square_of(int file, int rank) {
    assert(file >= 0 && file < 8);
    assert(rank >= 0 && rank < 8);
    return rank * 8 + file;
}
constexpr bool test_bit(Bitboard bb, int square_index) {
    assert(square_index >= 0 && square_index < 64);
    return ((bb >> square_index) & 1) != 0;
}

void print_bb(Bitboard bb);
