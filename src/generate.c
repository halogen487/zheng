#include "board.h"

/*bitboard abs_pinned(struct game* g) {
	bool turn = g->turn;
	square king_square; // find king and sliders
	square rooks[10]; char rook_count = 0; // includes queens
	square bishops[10]; char bishop_count = 0;
	for (int i = 0; i < 10; i++) {rooks[i] = 0;}

	for (int i = 0; i < 64; i++) {
		struct piece curp = *(g->board[i]);
		if (curp.type == 6 && curp.color == turn) {
			king_square = i;
		} else if (curp.color != turn) {
			if (curp.type == 4 || curp.type == 5) {
				rooks[rook_count] = i;
				rook_count++;
			} if (curp.type == 3 || curp.type == 5) {
				bishops[bishop_count] = i;
				bishop_count++;
			}
		}
	}
	bitboard pins = 0;
	struct dir d = {1, 0};
	for (int i = 0; i < 8; i++) { // for each direction
		char friends_found;
		square path[7];
		int dist = 0;
		for (square j = dir_increment(king_square, d); j != 65; j = dir_increment(j, d)) {
			if (g->board[j]->color == turn) { // if friend
				friends_found++;
				if (friends_found > 1) {
					break;
				}
			}
			if (g->board[j]->color != turn && friends_found) { // if enemy after friend
				if ((i % 2 == 0 && square_in_array(rooks, 10, j)) || square_in_array(bishops, 10, j)) { // if straight and rook found
					bitboard pin = 0;
					for (int k = 0; k < dist; k++) { // for each square in path
						pin = (pin | (1 << path[k])); // set path[k]th bit as 1
					}
					pins = pins | pin;
					break;
					}
			}
			path[dist] = j;
			dist++;
		};
		d = rotate_dir(d);
	}
	return pins;
}*/

bitboard enemy_turf(struct game* g) {
	return 0;
}

bool in_bitboard(bitboard b, square sq) {
	bitboard a = 1 << sq;
	return a & b;
}

bool will_check(struct game* g, struct move m) {
	return 0;
}

struct move create_move(square from, square to, enum move_flag flag) {
	struct move new;
	new.from = from;
	new.to = to;
	new.flag = flag;
	return new;
}

int generate_pawn_moves(struct game* g, struct move* buf, square sq) {
	square in_front;
	if (g->turn == 1) {
		in_front = sq + 8;
	} else {
		in_front = sq - 8;
	}
	int move_count = 0;
	
	if (g->turn == 1) {
		square two_away = in_front + 8;
		if (sq >= 8 && sq < 16
		&& g->board[in_front]->color == 0 && g->board[two_away]->color == 0) {
			buf[move_count] = create_move(sq, two_away, dash);
			move_count++;
		}
	} else {
		square two_away = in_front - 8;
		if (sq >= 48 && sq < 56
		&& g->board[in_front]->color == 0 && g->board[two_away]->color == 0) {
			buf[move_count] = create_move(sq, two_away, dash);
			move_count++;
		}
	}
	
	//TODO add enpassant
	
	struct move maybe_promotions[24]; // following moves could be promotions
	int mp_count = 0;

	if (g->board[in_front]->color == 0) { // move forward
		maybe_promotions[mp_count] = create_move(sq, in_front, null);
		mp_count++;
	}
	
	square west_capture = in_front - 1; // captures
	if ((g->board[west_capture]->type != 0 || west_capture == g->enpassant_target)
	&& edge(sq).east_west != -1) {
		maybe_promotions[mp_count] = create_move(sq, west_capture, null);
		mp_count++;
	}
	square east_capture = in_front + 1;
	if ((g->board[east_capture]->type != 0 || east_capture == g->enpassant_target)
	&& edge(sq).east_west != 1) {
		maybe_promotions[mp_count] = create_move(sq, east_capture, null);
		mp_count++;
	}
	
	for (int i = 0; i < mp_count; i++) {
		square from = maybe_promotions[i].from;
		if ((g->turn == 1 && from >= 48 && from < 56)
		|| (g->turn == 0 && from >= 8 && from < 16)) {
			
		}
	}
	
	return 0;
}

int generate_moves(struct game* g, struct move* buf) { // linked list prob better than array
	//bitboard a_pinned = abs_pinned(g);
	bitboard e_turf = enemy_turf(g);
	int move_count = 0;
	char turn = g->turn;
	for (int i = 0; i < 64; i++) {
		struct piece cur_piece = *g->board[i];
		if (cur_piece.color == turn) {
			switch (cur_piece.type) {
				case 1: // pawn
					move_count += generate_pawn_moves(g, &buf[move_count], i);
					break;
			}
		}
	}
	return move_count;
}
