#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

typedef unsigned char square;
typedef unsigned long bitboard;



struct dir {
	signed char north_south;
	signed char east_west;
};

struct piece {
	char color; // 1 for white
	char type; // 1 for pawn
};

enum move_flag {
	null,
	dash,
	enpassant,
	castle,
	promo_knight,
	promo_bishop,
	promo_rook,
	promo_queen
};

struct move {
	square from;
	square to;
	enum move_flag flag;
};

struct game {
	struct piece* board[64];
	char turn;
	bool wk_castle;
	bool wq_castle;
	bool bk_castle;
	bool bq_castle;
	square enpassant_target; // 65 for none
	char halfmove_clock;
	int fullmove_no;
};

square coord(char file, char rank);
square scoord(char rf[2]);
square dir_increment(square sq, struct dir d);
struct dir rotate_dir(struct dir d);
struct dir edge(square sq);
bool square_in_array(square arr[], int length, square sq);

struct game* create_game(char* fen_string);
void destroy_game(struct game* g);
void make_move(struct game* g, struct move m);
void unmake_move(struct game* g, struct move* m);
void move_str(struct move m, char* buf);
void print_game(struct game* g);

#endif
