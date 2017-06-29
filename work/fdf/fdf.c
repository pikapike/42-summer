#include "./minilibx_macos/mlx.h"
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

// How to compile: gcc fdf.c -L minilibx_macos -lmlx -framework OpenGL -framework Appkit -o fdf

double angle = 1;
void *canvas;
int scrx = 1500, scry = 1000;
int **pic;
int pic_l, pic_w;
void *core;
int max_z = -1000000, min_z = 1000000;
int scale_height = 1;

/*
int rast_brite(int x1, int y1, int x2, int y2, int xg, int yg) {
	double m = (y1 + y2 + 0.0) / (x1 + x2);
} */

int min(int x, int y) { return (x < y) ? x : y; }
int max(int x, int y) { return (x > y) ? x : y; }

void draw_line(int x1, int y1, int z1, int x2, int y2, int z2) {
	int dx = 0, dy = 0;
	int dirx = (x1 < x2)*2-1, diry = (y1 < y2)*2-1;
	while ((dirx*(x1+dx) < dirx*x2) || (diry*(y1+dy) < diry*y2)) {
        // int cur_pix = mlx_get_pixel(core, canvas, x1+dx, y1+dy);
        int progress = 256*(abs(dx)+abs(dy))/(abs(x2-x1)+abs(y2-y1));
        int height_256 = min(255, (progress*(z2-min_z)+(256-progress)*(z1-min_z))/(max_z-min_z));
        int col_pix =  (0x00010000-0x00000100)*height_256+0x00FF00;
        mlx_pixel_put(core, canvas, x1+dx, y1+dy, col_pix);
        if (dx*(y2-y1)*dirx*diry < dy*(x2-x1)*dirx*diry) { // dy=(y2-y1)/(x2-x1)*dx
			dx += dirx;
		} else {
			dy += diry;
		}
	}
	mlx_pixel_put(core, canvas, x1+dx, y1+dy, 0x00FFFFFF);
	return;
}

int *read_dimensions(char *filename) {
	int buf = open(filename, O_RDONLY);
	if (buf < 0) return NULL;
	char digit = '?';
	int pic_length = 0;
	int last_was_digit = 0;
	while (digit != '\n') {
		if (read(buf, &digit, 1) < 0) return NULL;
		if ('0' <= digit && digit <= '9') { 
			if (!last_was_digit) {
				pic_length++;
				last_was_digit = 1;
			}
		} else { last_was_digit = 0; }
	}
	int pic_weight = 1;
	while (read(buf, &digit, 1) > 0) {
		if (digit == '\n') { pic_weight++; }
	}
	close(buf);
	int *dimensions;
	dimensions = (int *) malloc(sizeof(int)*2);
	dimensions[0] = pic_length; dimensions[1] = pic_weight;
	return dimensions;
}

int **read_file(char *filename, int pic_length, int pic_weight) {
	char digit = '?';
	int buf = open(filename, O_RDONLY);
	int **pic;
	pic = (int **) malloc(sizeof(int *)*(pic_weight+1));
	for (int i = 0; i < pic_weight; i++) {
		int *row;
		row = (int *) malloc(sizeof(int)*(pic_length+1));
		int row_ptr = 0;
		int last_was_digit = 0;
		int num;
        int negate;
		digit = '?';
		while (digit != '\n') {
			read(buf, &digit, 1);
			int decide = 4*(digit == '-')+2*('0' <= digit && digit <= '9') + last_was_digit;
			switch(decide) {
				case 4:
                    negate = 1;
                    num = 0;
                    last_was_digit = 1;
                    break;
				case 1:
                    if (negate) num = -num;
					row[row_ptr] = num;
                    if (num > max_z) max_z = num;
                    if (num < min_z) min_z = num;
					row_ptr++;
					last_was_digit = 0;
					break;
				case 2:
					num = digit-'0';
                    negate = 0;
					last_was_digit = 1;
					break;
				case 3:
					num = 10*num+digit-'0';
					break;
			}
		}
		row[pic_length] = '\0';
		pic[i] = row;
	}
	return pic;
}

int *translate_coords(int x, int y, int z, double angle) {
    int x_to_screenx = round(15*sin(angle)), x_to_screeny = round(15*cos(angle));
    int y_to_screenx = round(15*sin(angle-1.07)), y_to_screeny = round(15*cos(angle-1.07));
    int z_to_screenx = 0, z_to_screeny = -scale_height;
    int orgx = 750, orgy = 500;
    int centx = pic_l/2, centy = pic_w/2;
    int *coords = (int *) malloc(sizeof(int)*2);
    coords[0] = orgx+(x-centx)*x_to_screenx+(y-centy)*y_to_screenx+z*z_to_screenx;
    coords[1] = orgy+(x-centx)*x_to_screeny+(y-centy)*y_to_screeny+z*z_to_screeny;
    return coords;
}

void draw_screen(int **pic, int pic_l, int pic_w) {
    canvas = mlx_new_window(core, scrx, scry, "fdf");
    
    for (int i = 0; i < pic_w-1; i++) {
        for (int j = 0; j < pic_l; j++) {
            int draw_x1 = translate_coords(j, i, pic[i][j], angle)[0];
            int draw_x2 = translate_coords(j, i+1, pic[i+1][j], angle)[0];
            int draw_y1 = translate_coords(j, i, pic[i][j], angle)[1];
            int draw_y2 = translate_coords(j, i+1, pic[i+1][j], angle)[1];
            draw_line(draw_x1, draw_y1, pic[i][j], draw_x2, draw_y2, pic[i+1][j]);
        }
    }
    for (int i = 0; i < pic_w; i++) {
        for (int j = 0; j < pic_l-1; j++) {
            int draw_x1 = translate_coords(j, i, pic[i][j], angle)[0];
            int draw_x2 = translate_coords(j+1, i, pic[i][j+1], angle)[0];
            int draw_y1 = translate_coords(j, i, pic[i][j], angle)[1];
            int draw_y2 = translate_coords(j+1, i, pic[i][j+1], angle)[1];
            draw_line(draw_x1, draw_y1, pic[i][j], draw_x2, draw_y2, pic[i][j+1]);
        }
    }
    return;
}

int on_key(int keycode, void *param) {
    printf("key event %d\n", keycode);
    if (keycode == 53) {
        exit(0);
    } else if (keycode == 123) {
        angle += 0.3;
        mlx_destroy_window(core, canvas);
        draw_screen(pic, pic_l, pic_w);
        mlx_key_hook(canvas, on_key, 0);
    } else if (keycode == 126) {
        scale_height += 1;
        mlx_destroy_window(core, canvas);
        draw_screen(pic, pic_l, pic_w);
        mlx_key_hook(canvas, on_key, 0);
    }
    return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 2) return 0;
	int *dimensions;
	dimensions = read_dimensions(argv[1]);
    if (dimensions == NULL) return 0;
    pic_l = dimensions[0]; pic_w = dimensions[1];
	pic = read_file(argv[1], pic_l, pic_w);
	if (pic == NULL) return 0;
	for (int i = 0; i < pic_w; i++) {
		for (int j = 0; j < pic_l; j++) {
			printf("%i ", pic[i][j]);
		}
    }
    printf("\n");
    printf("%i %i \n", max_z, min_z);
    core = mlx_init();
    draw_screen(pic, pic_l, pic_w);
    mlx_key_hook(canvas, on_key, 0);
    mlx_loop(core);
	return 0;
}
