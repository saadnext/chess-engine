#include "bitboard.h"
#include "board.h"
#include "types.h"
#include <iostream>
#include <string>
static int failures = 0;
static void check(bool ok, const char* what) {
    if (!ok) {
        std::cerr << "FAIL: " << what << "\n";
        ++failures;
    }
}
constexpr const char* type_idx_to_string[NUM_PIECE_TYPES] = {"Pawn", "Knight", "Bishop",
                                                             "Rook", "Queen",  "King"};
int main() {
    Board board;
    for (int color_idx = 0; color_idx < NUM_COLORS; ++color_idx) {
        for (int type_idx = 0; type_idx < NUM_PIECE_TYPES; ++type_idx) {
            std::string str = "board.pieces(Color::";
            str += (color_idx == 0 ? "White" : "Black");
            str += ", PieceType::";
            str += type_idx_to_string[type_idx];
            str += ") == 0ULL";
            const char* what = str.c_str();
            check(board.pieces(static_cast<Color>(color_idx), static_cast<PieceType>(type_idx)) ==
                      0,
                  what);
        }
    }

    board.put_piece(Color::White, PieceType::Pawn, 28);
    check(board.pieces(Color::White, PieceType::Pawn) == square_bb(28),
          "board.pieces(Color::White, PieceType::Pawn) == square_bb(28)");
    board.remove_piece(Color::White, PieceType::Pawn, 28);
    check(board.pieces(Color::White, PieceType::Pawn) == 0ULL,
          "board.pieces(Color::White, PieceType::Pawn) == 0ULL");

    board.put_piece(Color::White, PieceType::Knight, 1); // White knight on b1;
    check(!board.has_piece(Color::Black, PieceType::Knight, 1),
          "!board.has_piece(Color::Black, PieceType::Knight, 1)");
    check(board.has_piece(Color::White, PieceType::Knight, 1),
          "board.has_piece(Color::White, PieceType::Knight, 1)");
    board.put_piece(Color::Black, PieceType::King, 60); // Black king on e8;
    check(board.has_piece(Color::Black, PieceType::King, 60),
          "board.has_piece(Color::Black, PieceType::King, 60)");
    check(!board.has_piece(Color::White, PieceType::King, 60),
          "board.has_piece(Color::White, PieceType::King, 60)");

    auto piece_b1 = board.piece_on(1);
    auto piece_e8 = board.piece_on(60);
    auto piece_a1 = board.piece_on(0);
    check(piece_b1.has_value() && *piece_b1 == Piece{Color::White, PieceType::Knight},
          "piece_b1.has_value() && *piece_b1 == Piece{Color::White,PieceType::Knight}");
    check(piece_e8.has_value() && *piece_e8 == Piece{Color::Black, PieceType::King},
          "piece_e8.has_value() && *piece_e8 == Piece{Color::Black,PieceType::King}");
    check(!piece_a1.has_value(), "!piece_a1.has_value()");
    board.remove_piece(Color::White, PieceType::Knight, 1);
    piece_b1 = board.piece_on(1);
    check(!piece_b1.has_value(), "!piece_b1.has_value()");

    check(!board.has_piece(Color::White, PieceType::Knight, 1),
          "!board.has_piece(Color::White, PieceType::Knight, 1)");

    return failures == 0 ? 0 : 1;
}
