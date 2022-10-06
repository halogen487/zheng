#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "board.h"

bool square_in_array(square arr[], int length, square sq) {
	for (int i = 0; i < length; i++) {
		if (arr[i] == sq) {
			return true;
		}
	}
	return false;
}

square coord(char file, char rank) { // rank, file -> square # from 0-63
	return rank * 8 + file;
}

square scoord(char rf[2]) {
	char file = toupper(rf[0]) - 65;
	char rank = strtol(&rf[1], NULL, 10) - 1;
	return coord(file, rank);
}

square dir_increment(square sq, struct dir d) { // increment a square in given dir
	struct dir edge_dir = edge(sq);
	if (edge_dir.north_south != 0 || edge_dir.east_west != 0) {
		return 65;
	}
	sq += d.east_west;
	sq += (d.north_south * 8 );
	return sq;
}

struct dir rotate_dir(struct dir d) { // clockwise
	signed char ns = d.north_south;
	signed char ew = d.east_west;
	switch (ns) { // must be a better way to do this somehow
		case 1:
			switch (ew) {
				case 1:
					ns--;
				case 0:
				case -1:
					ew++;
					break;
			}
			break;
		case 0:
			switch (ew) {
				case 1:
					ns--;
				case -1:
					ns++;
			}
			break;
		case -1:
			switch (ew) {
				case 1:
				case 0:
					ew--;
				case -1:
					ns++;
			}
			break;
	}
	d.north_south = ns;
	d.east_west = ew;
	return d;
}

struct dir edge(square sq) { // test if square is on edge of board, return dir of edge
	struct dir d;
	d.north_south = 0;
	d.east_west = 0;
	if (sq < 8) {
		d.north_south = -1;
	} else if (sq >= 56) {
		d.north_south = 1;
	} if (sq % 8 == 0) {
		d.east_west = -1;
	} else if (sq % 8 == 7) {
		d.east_west = 1;
	}
	return d;
}

struct game* create_game(char* fen_string) {

	struct game* ng = malloc(sizeof(struct game));
	// parse fen
	char* fen_field_save;
	char* fen_field = strtok_r(fen_string, " ", &fen_field_save);
	{
		char* rank_save;
		char* rank = strtok_r(fen_field, "/", &rank_save);
		int rank_no = 7;
		while (rank != NULL) { // for each rank
			int file_no = 0;
			for (int i = 0; i < strlen(rank); i++) { // for each character
				if (isdigit(rank[i])) {
					int empties = strtol(&rank[i], NULL, 10);
					for (int i = 0; i < empties; i++) {
						struct piece* new_piece = malloc(sizeof(struct piece));
						new_piece->color = 0;
						new_piece->type = 0;
						ng->board[coord(file_no + i, rank_no)] = new_piece;
					}
					file_no += empties;
				} else {
					struct piece* new_piece = malloc(sizeof(struct piece));
					new_piece->color = (bool)islower(rank[i]) + 1;
					switch (toupper(rank[i])) {
						case 'P':
							new_piece->type = 1;
							break;
						case 'N':
							new_piece->type = 2;
							break;
						case 'B':
							new_piece->type = 3;
							break;
						case 'R':
							new_piece->type = 4;
							break;
						case 'Q':
							new_piece->type = 5;
							break;
						case 'K':
							new_piece->type = 6;
							break;
					}
					ng->board[coord(file_no, rank_no)] = new_piece;
					file_no++;
				}
			}
			rank = strtok_r(NULL, "/", &rank_save);
			rank_no--;
		}
	}

	// turn color
	fen_field = strtok_r(NULL, " ", &fen_field_save);
	if (strcmp(fen_field, "w") == 0) {
		ng->turn = 1;
	} else {
		ng->turn = 2;
	}

	// castle availability
	fen_field = strtok_r(NULL, " ", &fen_field_save);
	ng->wk_castle = 0;
	ng->wq_castle = 0;
	ng->bk_castle = 0;
	ng->bq_castle = 0;
	if (strcmp(fen_field, "-") != 0) {
		if (strchr(fen_field, 'K') != NULL) {
			ng->wk_castle = 1;
		} if (strchr(fen_field, 'Q') != NULL) {
			ng->wq_castle = 1;
		} if (strchr(fen_field, 'k') != NULL) {
			ng->bk_castle = 1;
		} if (strchr(fen_field, 'q') != NULL) {
			ng->bq_castle = 1;
		}
	}

	// en passant
	fen_field = strtok_r(NULL, " ", &fen_field_save);
	if (fen_field[0] == '-') {
		ng->enpassant_target = 65;
	} else {
		printf("%s\n", fen_field);
		ng->enpassant_target = scoord(fen_field);
	}

	return ng;
}

void destroy_game(struct game* g) {
	for (int i = 0; i < 64; i++) {
		free(g->board[i]);
	}
	free(g);
}

void kill_piece(struct piece* p) {
	p->color = 0;
	p->type = 0;
}

void make_move(struct game* g, struct move m) {
	// swap pieces, nullify captured one
	struct piece* mover = g->board[m.from];
	struct piece* captured = g->board[m.to];
	g->board[m.from] = captured;
	g->board[m.to] = mover;
	unsigned short dead = 0;
	kill_piece(g->board[m.from]);
	if (m.flag != 1) {
		g->enpassant_target = 0;
		switch (m.flag) {
			case 0: // dash
				if (g->turn) {
					g->enpassant_target = m.from - 8;
				} else {
					g->enpassant_target = m.from + 8;
				}
				break;
		}
	} else { // en passant
		if (g->turn) {
			g->board[m.to + 8];
		} else {

		}
	}
	g->turn = !g->turn;
}

void unmake_move(struct game* g, struct move* m) {}

void print_game(struct game* g) {
	if (g->turn) {
		printf(" Black");
	} else {
		printf(" White");
	}
	printf(" to move\n");
	printf("┌─────────────────┐ \n");
	for (int i = 56; i >= 0; i++) {
		char symbol = '.';
		if (i % 8 == 0) {
			printf("│");
		}
		switch (g->board[i]->type) {
			case 1:
				symbol = 'P';
				break;
			case 2:
				symbol = 'N';
				break;
			case 3:
				symbol = 'B';
				break;
			case 4:
				symbol = 'R';
				break;
			case 5:
				symbol = 'Q';
				break;
			case 6:
				symbol = 'K';
				break;
		}
		if (g->board[i]->color - 1) {
			symbol = tolower(symbol);
		}
		printf(" %c", symbol);
		if (i % 8 == 7) {
			i -= 16;
			printf(" │\n");
		}
	}
	printf("└─────────────────┘\n");
}
