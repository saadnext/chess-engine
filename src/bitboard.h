#pragma once
#include <cassert>
#include <cstdint>

using Bitboard = std::uint64_t;
constexpr int NUM_SQUARES = 64;

inline constexpr Bitboard FILE_A = 0x0101010101010101ULL;
inline constexpr Bitboard FILE_B = FILE_A << 1;
inline constexpr Bitboard FILE_C = FILE_A << 2;
inline constexpr Bitboard FILE_D = FILE_A << 3;
inline constexpr Bitboard FILE_E = FILE_A << 4;
inline constexpr Bitboard FILE_F = FILE_A << 5;
inline constexpr Bitboard FILE_G = FILE_A << 6;
inline constexpr Bitboard FILE_H = FILE_A << 7;

inline constexpr Bitboard NOT_FILE_AB = ~(FILE_A | FILE_B);
inline constexpr Bitboard NOT_FILE_GH = ~(FILE_G | FILE_H);

constexpr bool is_valid_square(int square_index) {
    return square_index >= 0 && square_index < NUM_SQUARES;
}

constexpr bool is_valid_square(int file, int rank) {
    return file >= 0 && file < 8 && rank >= 0 && rank < 8;
}

constexpr Bitboard square_bb(int square_index) {
    assert(is_valid_square(square_index));
    return std::uint64_t{1} << square_index;
}

constexpr int square_of(int file, int rank) {
    assert(is_valid_square(file, rank));
    return rank * 8 + file;
}

constexpr int file_of(int square_index) {
    assert(is_valid_square(square_index));
    return square_index % 8;
}
constexpr int rank_of(int square_index) {
    assert(is_valid_square(square_index));
    return square_index / 8;
}
static_assert(square_of(file_of(37), rank_of(37)) == 37);

constexpr bool test_bit(Bitboard bb, int square_index) {
    assert(is_valid_square(square_index));
    return ((bb >> square_index) & 1) != 0;
}

void print_bb(Bitboard bb);
