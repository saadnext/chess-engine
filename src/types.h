#pragma once

constexpr int NUM_COLORS = 2;
constexpr int NUM_PIECE_TYPES = 6;

enum class Color : int { White, Black };
enum class PieceType : int { Pawn, Knight, Bishop, Rook, Queen, King };

constexpr int index(Color color) { return static_cast<int>(color); }
constexpr int index(PieceType type) { return static_cast<int>(type); }
