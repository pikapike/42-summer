#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int *read_ints(int trash_chars) {
	char *trash;
	trash = (char *) malloc(sizeof(char)*trash_chars);
	read(0, trash, trash_chars);
	int n1 = 0, n2 = 0;
	char number_read;
	read(0, &number_read, 1);
	while ('0' <= number_read && number_read <= '9') {
		n1 = 10*n1+number_read-'0';
		read(0, &number_read, 1);
	}
	read(0, &number_read, 1);
	while ('0' <= number_read && number_read <= '9') {
		n2 = 10*n2+number_read-'0';
		read(0, &number_read, 1);
	}
	read(0, &number_read, 1);
	int *ret_int;
	ret_int = (int *) malloc(sizeof(int)*2);
	ret_int[0] = n1;
	ret_int[1] = n2;
	free(trash);
	return ret_int;
}

int **floodfill(char **board, int bx, int by, char playa) {
	int **flood;
	int **visited;
	int **current;
	int **next;
	current = (int **) malloc(sizeof(int *)*(bx*by+1));
	next = (int **) malloc(sizeof(int *)*(bx*by+1));
	int cur_ptr = 0, next_ptr = 0;
	flood = (int **) malloc(sizeof(int *)*(by+1));
	visited = (int **) malloc(sizeof(int *)*(by+1));
	for (int i = 0; i < by; i++) {
		int *f_row;
		int *v_row;
		f_row = (int *) malloc(sizeof(int)*(bx+1));
		v_row = (int *) malloc(sizeof(int)*(bx+1));
		for (int j = 0; j < bx; j++) {
			if (board[i][j] == '.') {
				f_row[j] = 100000;
				v_row[j] = 0;
			} else if (board[i][j] == playa) {
				f_row[j] = -1;
				v_row[j] = 1;
			} else {
				f_row[j] = -1;
				v_row[j] = 1;
				int *coords;
				coords = (int *) malloc(sizeof(int)*(2+1));
				coords[0] = i; coords[1] = j; coords[2] = '\0';
				current[cur_ptr] = coords; cur_ptr++;
			}
		}
		f_row[bx] = '\0'; v_row[bx] = '\0';
		flood[i] = f_row; visited[i] = v_row;
	}
	visited[by] = NULL; flood[by] = NULL;
	for (int d = 1; cur_ptr != 0; d++) {
		for (int i = 0; i < cur_ptr; i++) {
			for (int a = -1; a < 2; a += 2) {
				for (int b = -1; b < 2; b += 2) {
					int nexty = current[i][0] + (a+b)/2, nextx = current[i][1] + (a-b)/2;
					if (nextx >= 0 && nextx < bx && nexty >= 0 && nexty < by) {
						if (!visited[nexty][nextx]) {
							visited[nexty][nextx] = 1;
							flood[nexty][nextx] = d;
							int *next_pt;
							next_pt = (int *) malloc(sizeof(int)*(2+1));
							next_pt[0] = nexty; next_pt[1] = nextx; next_pt[2] = '\0';
							next[next_ptr] = next_pt; next_ptr++;
						}
					}
				}
			}
			free(current[i]);
		}
		cur_ptr = 0;
		for (int i = 0; i < next_ptr; i++) {
			current[cur_ptr] = next[i]; cur_ptr++;
		}
		next_ptr = 0;
	}
	for (int i = 0; visited[i] != NULL; i++) free(visited[i]);
	return flood;
}

int *placepiece(char **board, int **flood, char **piece, int bx, int by, int px, int py, char playa) {
	int *best_move;
	best_move = (int *) malloc(sizeof(int)*(2+1));
	best_move[2] = '\0';
	int best_val = 70000000;
	int borx = px-1, bory = py-1;
	for (int i = 0; i < by+bory; i++) {
		for (int j = 0; j < bx+borx; j++) {
			int collision = 0, value = 0;
			for (int k = 0; k < py; k++) {
				for (int m = 0; m < px; m++) {
					if (board[i+k][j+m] != '.' && piece[k][m] != '.') {
						collision++;
						if (board[i+k][j+m] != playa) {
							 collision++;
						}
					}
					if (collision < 2 && piece[k][m] == '*') value += flood[i+k-bory][j+m+borx];
				}
			}
			if (collision == 1 && value < best_val) {
				best_move[0] = i-bory; best_move[1] = j-borx;
				best_val = value;
			}
		}
	}
	return best_move;
}

void skip_till_turn() {
	unsigned long rndmcnst = 10000;
	char *trash;
	trash = (char *) malloc(sizeof(char)*10000);
	trash[1] = '7';
	while (trash[1] != 'g') { getline(&trash, &rndmcnst, stdout); write(2, trash, 17); write(2, "\n", 1); }
	return;
}

int main(int argc, char *argv[]) {
	unsigned long rndmcnst = 10000;
	char *trash;
	trash = (char *) malloc(sizeof(char)*10000);
	getline(&trash, &rndmcnst, stdout);
	char *important_line;
	important_line = (char *) malloc(sizeof(char)*10000);
	getline(&important_line, &rndmcnst, stdout);
	int fp = 0;
	if (important_line[23] == 'p' && important_line[24] == 'l' && important_line[25] == 'a') {
		if (important_line[26] == 'y' && (important_line[27] == 'a')) {
			fp = 1;
		}
	}
	free(important_line);
	for (int i = 0; i < 2; i++) { getline(&trash, &rndmcnst, stdout); }
	write(2, "eoie", 4);
	if (!fp) skip_till_turn();
	write(2, "diof", 4);
	while (1) {
		write(2, "jsadd", 5);
		int *blist;
		blist = read_ints(8);
		int bx = blist[1], by = blist[0];
		trash[0] = '7';
		while (trash[0] != '\n') read(1, trash, 1);
		char **board;
		board = (char **) malloc(sizeof(char *)*(by+1));
		for (int i = 0; i < by; i++) {
			char *row;
			row = (char *) malloc(sizeof(char)*(bx+1));
			read(0, trash, 4);
			read(0, row, bx);
			read(0, trash, 1);
			row[bx] = '\0';
			board[i] = row;
		}
		board[by] = NULL;
		int *plist;
		plist = read_ints(6);
		int px = plist[1], py = plist[0];
		char **piece;
		piece = (char **) malloc(sizeof(char *)*(py+1));
		for (int i = 0; i < py; i++) {
			char *row;
			row = (char *) malloc(sizeof(char)*(px+1));
			read(0, row, px);
			read(0, trash, 1);
			row[bx] = '\0';
			piece[i] = row;
		}
		int boundh = by+(py-1)*2, boundl = bx+(px-1)*2;
		char **boundboard; // use board for floodfill heat map, use boundboard for piece placement
		boundboard = (char **) malloc(sizeof(char *)*(boundh+1));
		boundboard[boundh] = NULL;
		for (int i = 0; i < boundh; i++) {
			char *hashrow;
			hashrow = (char *) malloc(sizeof(char)*(boundl+1));
			for (int j = 0; j < boundl; j++) hashrow[j] = '#';
			hashrow[boundl] = '\0';
			boundboard[i] = hashrow;
		}
		for (int i = py-1; i < py-1+by; i++) {
			for (int j = px-1; j < px-1+bx; j++) {
				boundboard[i][j] = board[i-py+1][j-px+1];
			}
		}
		char playa = 'O';
		for (int i = 0; i < by; i++) {
			for (int j = 0; j < bx; j++) {
				if (board[i][j] == 'o') playa = 'X';
			}
		}
		int **boardflood;
		boardflood = floodfill(board, bx, by, playa);
		/*
		for (int i = 0; boardflood[i] != NULL; i++) {
			for (int j = 0; boardflood[i][j] != '\0'; j++) {
				printf("%i ", boardflood[i][j]);
			}
			putchar('\n');
		}*/
		int *move;
		move = placepiece(boundboard, boardflood, piece, bx, by, px, py, playa);
		printf("%i %i\n", move[1], move[0]);
		free(blist);
		free(plist);
		free(move);
		for (int i = 0; board[i] != NULL; i++) {
			free(board[i]);
		}
		free(board);
		for (int i = 0; boundboard[i] != NULL; i++) {
			free(boundboard[i]);
		}
		free(boundboard);
		free(boardflood);
		getline(&trash, &rndmcnst, stdout);
		skip_till_turn();
	}
	
	return 0;
}
