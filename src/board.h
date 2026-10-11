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

constexpr PieceType back_rank_order[8] = {PieceType::Rook,   PieceType::Knight, PieceType::Bishop,
                                          PieceType::Queen,  PieceType::King,   PieceType::Bishop,
                                          PieceType::Knight, PieceType::Rook};
constexpr char piece_to_char[NUM_COLORS][NUM_PIECE_TYPES] = {
    {'P', 'N', 'B', 'R', 'Q', 'K'},
    {'p', 'n', 'b', 'r', 'q', 'k'},
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
    void set_start_position() {
        clear();
        for (int file = 0; file < 8; ++file) {
            put_piece(Color::White, back_rank_order[file], square_of(file, 0));
            put_piece(Color::Black, back_rank_order[file], square_of(file, 7));
            put_piece(Color::White, PieceType::Pawn, square_of(file, 1));
            put_piece(Color::Black, PieceType::Pawn, square_of(file, 6));
        }
    }
    void clear() { *this = Board{}; }
    void print() const;
};
