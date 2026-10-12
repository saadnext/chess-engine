#include "board.h"
#include <iostream>

namespace {
constexpr PieceType BACK_RANK_ORDER[8] = {PieceType::Rook,   PieceType::Knight, PieceType::Bishop,
                                          PieceType::Queen,  PieceType::King,   PieceType::Bishop,
                                          PieceType::Knight, PieceType::Rook};
constexpr char PIECE_TO_CHAR[NUM_COLORS][NUM_PIECE_TYPES] = {
    {'P', 'N', 'B', 'R', 'Q', 'K'},
    {'p', 'n', 'b', 'r', 'q', 'k'},
};
} // namespace

void Board::print() const {
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " |";
        for (int file = 0; file < 8; ++file) {
            int square_index = square_of(file, rank);
            auto piece = piece_on(square_index);
            if (piece.has_value()) {
                std::cout << " " << PIECE_TO_CHAR[to_index(piece->color)][to_index(piece->type)]
                          << " |";
            } else {
                std::cout << "   |";
            }
        }
        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
    }
    std::cout << "    a   b   c   d   e   f   g   h\n";
}

void Board::set_start_position() {
    clear();
    for (int file = 0; file < 8; ++file) {
        put_piece(Color::White, BACK_RANK_ORDER[file], square_of(file, 0));
        put_piece(Color::Black, BACK_RANK_ORDER[file], square_of(file, 7));
        put_piece(Color::White, PieceType::Pawn, square_of(file, 1));
        put_piece(Color::Black, PieceType::Pawn, square_of(file, 6));
    }
}

void Board::clear() { *this = Board{}; }
