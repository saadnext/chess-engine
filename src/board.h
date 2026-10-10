#pragma once
#include "bitboard.h"
#include "types.h"
#include <cassert>
#include <optional>

struct Piece {
    Color color;
    PieceType type;

    bool operator==(const Piece&) const = default;
};

class Board {
  private:
    Bitboard pieces_[NUM_COLORS][NUM_PIECE_TYPES]{};

  public:
    Bitboard pieces(Color color, PieceType type) const {
        return pieces_[index(color)][index(type)];
    }
    void put_piece(Color color, PieceType type, int square_index) {
        assert(is_valid_square(square_index));
        assert(!piece_on(square_index).has_value());
        pieces_[index(color)][index(type)] |= square_bb(square_index);
    }
    void remove_piece(Color color, PieceType type, int square_index) {
        assert(is_valid_square(square_index));
        assert(has_piece(color, type, square_index));
        pieces_[index(color)][index(type)] &= ~square_bb(square_index);
    }
    std::optional<Piece> piece_on(int square_index) const {
        assert(is_valid_square(square_index));
        for (int color_idx = 0; color_idx < NUM_COLORS; ++color_idx) {
            for (int type_idx = 0; type_idx < NUM_PIECE_TYPES; ++type_idx) {
                if (test_bit(pieces_[color_idx][type_idx], square_index)) {
                    return Piece{static_cast<Color>(color_idx), static_cast<PieceType>(type_idx)};
                }
            }
        }
        return std::nullopt;
    }
    bool has_piece(Color color, PieceType type, int square_index) const {
        assert(is_valid_square(square_index));
        return test_bit(pieces_[index(color)][index(type)], square_index);
    }
};
