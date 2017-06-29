#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

char **get_paths(char **environ) {
    int index = 0;
    while (strncmp(environ[index], "PATH", 4) != 0) {
        index++;
    }
    int counter = 0;
    while (environ[index][counter+5] != '\0') counter++;
    char *pathbreak = (char *) malloc(sizeof(char) * (counter + 1));
    for (int i = 5; i <= counter+5; i++) {
        pathbreak[i] = environ[index][i];
    }
    char **paths = strsplit(pathbreak, ':');
    return paths;
}

int run_command(char *path, char *args[], char *envars[], char **paths) {
    char *path_run = NULL;
    for (int i = 0; paths[i] != NULL; i++) {
        char *catpath;
        strcpy(catpath, paths[i]);
        strcat(catpath, path);
        if (access(catpath, X_OK) == 0) {
            path_run = catpath;
        }
    }
    if (path_run == NULL) {return 0;} // error
    execve(path_run, args, envars);
    return 0;
}

int main(int argc, char *argv[], char **environ) {
	int done = 0;	
    char **paths = get_paths(environ);
	while (!done) {
		write(1, "$>", 2);
        unsigned long wut = 1000;
        char *line = (char *) malloc(sizeof(char) * (wut+1));
        getline(&line, &wut, stdin);
		char **tokens = strsplit(line, ' ');
        
        int size = 0;
        while (tokens[size] != NULL) size++;
        char *args[size-1];
        for (int i = 0; i < size-1; i++) args[i] = tokens[i+1];
        
        size = 0;
        while (environ[size] != NULL) size++;
        char *envars[size-1];
        for (int i = 0; i < size-1; i++) envars[i] = environ[i+1];
        
		run_command(tokens[0], args, envars, paths);
	}
}
