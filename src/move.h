#pragma once
#include "bitboard.h"
#include <cassert>
#include <cstdint>

// layout
inline constexpr int TO_SHIFT = 6;
inline constexpr int FLAG_SHIFT = 12;

// test bits
inline constexpr std::uint16_t CAPTURE_BIT = 0b0100;
inline constexpr std::uint16_t PROMOTION_BIT = 0b1000;

// mask
inline constexpr std::uint16_t SQUARE_MASK = 0b111111;
inline constexpr std::uint16_t FLAG_MASK = 0b1111;

// flag codes
inline constexpr std::uint16_t FLAG_QUIET = 0b0000;
inline constexpr std::uint16_t FLAG_DOUBLE_PUSH = 0b0001;
inline constexpr std::uint16_t FLAG_CASTLE_KING = 0b0010;
inline constexpr std::uint16_t FLAG_CASTLE_QUEEN = 0b0011;

inline constexpr std::uint16_t FLAG_CAPTURE = 0b0100;
inline constexpr std::uint16_t FLAG_EN_PASSANT = 0b0101;

inline constexpr std::uint16_t FLAG_PROMOTE_KNIGHT = 0b1000;
inline constexpr std::uint16_t FLAG_PROMOTE_BISHOP = 0b1001;
inline constexpr std::uint16_t FLAG_PROMOTE_ROOK = 0b1010;
inline constexpr std::uint16_t FLAG_PROMOTE_QUEEN = 0b1011;

inline constexpr std::uint16_t FLAG_PROMOTE_KNIGHT_ON_CAPTURE = 0b1100;
inline constexpr std::uint16_t FLAG_PROMOTE_BISHOP_ON_CAPTURE = 0b1101;
inline constexpr std::uint16_t FLAG_PROMOTE_ROOK_ON_CAPTURE = 0b1110;
inline constexpr std::uint16_t FLAG_PROMOTE_QUEEN_ON_CAPTURE = 0b1111;

static_assert(FLAG_MASK == 15);
static_assert((CAPTURE_BIT & PROMOTION_BIT) == 0);
static_assert((FLAG_PROMOTE_QUEEN_ON_CAPTURE & CAPTURE_BIT) != 0 &&
              (FLAG_PROMOTE_QUEEN_ON_CAPTURE & PROMOTION_BIT) != 0);
static_assert(((FLAG_MASK << FLAG_SHIFT) | (SQUARE_MASK << TO_SHIFT) | SQUARE_MASK) == 0xFFFF);
static_assert((FLAG_PROMOTE_KNIGHT | CAPTURE_BIT) == FLAG_PROMOTE_KNIGHT_ON_CAPTURE);

constexpr std::uint16_t pack_move(int from, int to, std::uint16_t flag) {
    assert(is_valid_square(from));
    assert(is_valid_square(to));
    assert(flag <= FLAG_MASK);
    return static_cast<std::uint16_t>(from) | (static_cast<std::uint16_t>(to) << TO_SHIFT) |
           (static_cast<std::uint16_t>(flag) << FLAG_SHIFT);
}

class Move {
  private:
    std::uint16_t move_;

  public:
    constexpr Move() : move_(0) {}
    constexpr explicit Move(int from, int to, std::uint16_t flag)
        : move_(pack_move(from, to, flag)) {}
    constexpr int from() const { return move_ & SQUARE_MASK; }
    constexpr int to() const { return (move_ >> TO_SHIFT) & SQUARE_MASK; }
    constexpr std::uint16_t flag() const { return (move_ >> FLAG_SHIFT) & FLAG_MASK; }

    constexpr bool is_capture() const { return ((move_ >> FLAG_SHIFT) & CAPTURE_BIT) != 0; }
    constexpr bool is_promotion() const { return ((move_ >> FLAG_SHIFT) & PROMOTION_BIT) != 0; }
    bool operator==(const Move&) const = default;
};
