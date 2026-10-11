#include "board.h"
#include <iostream>

void Board::print() const {
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " |";
        for (int file = 0; file < 8; ++file) {
            int square_index = square_of(file, rank);
            auto piece = piece_on(square_index);
            if (piece.has_value()) {
                std::cout << " " << piece_to_char[index(piece->color)][index(piece->type)] << " |";
            } else {
                std::cout << "   |";
            }
        }
        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
    }
    std::cout << "    a   b   c   d   e   f   g   h\n";
}
