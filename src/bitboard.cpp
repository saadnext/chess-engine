#include "bitboard.h"
#include <iostream>

void print_bb(Bitboard bb) {
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " |";
        for (int file = 0; file < 8; ++file) {
            int square_index = square_of(file, rank);
            if (test_bit(bb, square_index)) {
                std::cout << " 1 |";
            } else {
                std::cout << " . |";
            }
        }
        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
    }
    std::cout << "    a   b   c   d   e   f   g   h\n";
}
