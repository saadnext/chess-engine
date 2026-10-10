#include "bitboard.h"
#include <iostream>
int main() {
    Bitboard bb = 0;
    bb |= square_bb(28);
    bb |= square_bb(0);
    std::cout << "Chess engine v0.1\n";
    print_bb(bb);
    return 0;
}
