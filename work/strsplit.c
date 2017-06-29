#include <stdio.h>
#include <stdlib.h>

char** strsplit(char* const s, char c) {
	int num_str = 1;
	for (int i = 0; s[i] != '\0'; i++) {
		if (s[i] == c) {
			num_str++;
		}
	}
	int *lengths;
	lengths = (int *) malloc(sizeof(int)*(num_str+1));
	int cnt = 0;
	int ptr = 0;
	for (int i = 0; s[i] != '\0'; i++) {
		if (s[i] == c) {
			lengths[ptr] = cnt;
			ptr++;
			cnt = 0;
		} else {
			cnt++;
		}
	}
	lengths[ptr] = cnt;
	lengths[num_str] = '\0';
	char **out;
	out = (char **) malloc(sizeof(char *)*(num_str+1));
	ptr = 0;
	for (int pt = 0; pt < num_str; pt++) {
		char *tmp;
		tmp = (char *) malloc(sizeof(char)*(lengths[pt]+1));
		for (int i = 0; i < lengths[pt]; i++) {
			tmp[i] = s[ptr];
			ptr++;
		}
		ptr++;
		tmp[lengths[pt]] = '\0';
		out[pt] = tmp;
	}
	out[num_str] = NULL;
	return out;
}

int main(int argc, char *argv[]) {
	if (argc > 1) {
		char **test;
		test = strsplit(argv[1], ',');
		for (int i = 0; test[i] != '\0'; i++) {
			for (int j = 0; test[i][j] != '\0'; j++) {
				putchar(test[i][j]);
			}
			putchar('\n');
		}
	}
	return 0;
}
