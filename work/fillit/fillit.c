#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void print_error() {
	write(1, "error\n", 6);
	return;
}

char **blank_square(int size) {
	char **sqr;
	sqr = (char **) malloc(sizeof(char *)*(size+6+1));
	for (int i = 0; i < size+6; i++) {
		char *row;
		row = malloc(sizeof(char)*(size+6+1));
		for (int j = 0; j < size+6; j++) {
			row[j] = '#';
		}
		if (i >= 3 && i < size+3) {
			for (int j = 3; j < size+3; j++) {
				row[j] = '.';
			}
		}
		row[size+6] = '\0';
		sqr[i] = row;
	}
	sqr[size+6] = NULL;
	return sqr;
}

char **check_fit(char ***tetrominos, char **state, int next_tet) {
	if (tetrominos[next_tet] == NULL) return state;
	int size = 0;
	while (state[size] != NULL) size++;
	size -= 6;
	for (int i = 0; i < size+3; i++) {
		for (int j = 0; j < size+3; j++) { // find way to score the nearness to the top-left corner
			int good = 1;
			for (int k = 0; k < 4; k++) {
				for (int m = 0; m < 4; m++) {
					if (tetrominos[next_tet][k][m] != '.' && state[i+k][j+m] != '.') {
						good = 0;
					}
				}
			}
			if (good) { 
				char **new_state; // set this new_state to state (not directly), and add working tetromino
				new_state = (char **) malloc(sizeof(char *)*(size+6+1));
				for (int k = 0; k < size+6; k++) {
					char *new_row; // deallocate these individual rows too
					new_row = (char *) malloc(sizeof(char)*(size+6+1));
					for (int m = 0; m < size+7; m++) {
						new_row[m] = state[k][m];
					}
					new_state[k] = new_row;
				} 
				new_state[size+6] = NULL; 
				for (int k = 0; k < 4; k++) {
					for (int m = 0; m < 4; m++) {
						if (tetrominos[next_tet][k][m] != '.') {
							new_state[i+k][j+m] = tetrominos[next_tet][k][m];
						}
					}
				} 
				char **recall;
				recall = check_fit(tetrominos, new_state, next_tet+1);
				if (recall != NULL) {
					return recall;
				} 
				for (int k = 0; new_state[k] != NULL; k++) {
					free(new_state[k]);
				}
				free(new_state);
				free(recall); 
			}
		}
	}
	return NULL;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		write(1, "usage: fillit target_file\n", 26);
		return 0;
	}
	int handle = open(argv[1], O_RDONLY);
	int num_tet = 0;
	int indic = 1;
	char *tmp;
	tmp = (char *) malloc(sizeof(char)*21);
	while (indic > 0) {
		indic = read(handle, tmp, 1);
		if (indic > 0) num_tet++;
		indic = read(handle, tmp, 20);
	}
	close(handle);
	if (num_tet == 0) {
		print_error();
		return 0;
	}
	handle = open(argv[1], O_RDONLY);
	char ***tetrominos;
	tetrominos = (char ***) malloc(sizeof(char **)*(num_tet+1));
	for (int i = 0; i < num_tet; i++) {
		char **tetromino;
		tetromino = (char **) malloc(sizeof(char *)*(4+1));
		for (int j = 0; j < 4; j++) {
			char *row;
			row = (char *) malloc(sizeof(char)*(4+1));
			read(handle, row, 5);
			if (row[4] != '\n') {
				print_error();
				return 0;
			}
			row[4] = '\0';
			tetromino[j] = row;
		}
		char *checkreturn;
		checkreturn = (char *) malloc(sizeof(char));
		checkreturn[0] = '\0';
		int checkeof = read(handle, checkreturn, 1);
		if (!((checkreturn[0] == '\n' && i < num_tet-1) || (checkeof == 0 && i == num_tet-1))) {
			print_error();
			return 0;
		}
		tetromino[4] = NULL;
		int *hashtags;
		int num_hashes = 0;
		int num_periods = 0;
		hashtags = (int *) malloc(sizeof(int)*(4+1));
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				char ch = tetromino[j][k];
				if (ch == '#') {
					if (num_hashes == 4) {
						print_error();
						return 0;
					}
					hashtags[num_hashes] = 5*j+k;
					tetromino[j][k] = 'A'+i;
					num_hashes++;
				} else if (ch == '.') {
					num_periods++;
				}
			}
		}
		if (!(num_periods == 12 && num_hashes == 4)) {
			print_error();
			return 0;
		}
		hashtags[4] = '\0';
		int num_adj = 0;
		for (int j = 0; j < 4; j++) {
			for (int k = j; k < 4; k++) {
				int hash_value = hashtags[j]-hashtags[k];
				if (hash_value*hash_value == 1 || hash_value*hash_value == 25) {
					num_adj++;
				}
			}
		}
		if (num_adj < 3) {
			print_error();
			return 0;
		}
		tetrominos[i] = tetromino;
	}
	tetrominos[num_tet] = NULL;
	int min_board = 0;
	while (min_board*min_board < 4*num_tet) min_board++;
	char newl = '\n';
	for (int i = min_board; i < 13; i++) {
		char **ans = check_fit(tetrominos, blank_square(i), 0);
		if (ans != NULL) {
			for (int j = 3; j < i+3; j++) {
				for (int k = 3; k < i+3; k++) {
					write(1, &ans[j][k], 1);
				}
				write(1, &newl, 1);
			}
			break;
		} 
	} 
	return 0;
}
