#include <stdio.h>
#include "board.h"
#include "move.h"
#include "generate.h"

int main(int argc, char** argv) {
	char starting_fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	struct game* the_game = create_game(starting_fen);
	struct move e4 = {scoord("e2"), scoord("e4")};
	make_move(the_game, e4);
	print_game(the_game);
	struct move moves[321];
	int move_count = generate_moves(the_game, &potential_moves[0]);
	for (int i = 0; i < move_count; i++) {
		printf("%d. %s%s %d", i + 1, moves[i].from, moves[i].to)
	}
	return 0;
}
