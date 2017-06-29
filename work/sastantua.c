#include <unistd.h>

int ft_putchar(char c) {
	write(1, &c, 1);
	return 0;
}

void sastantua(int size) {
	int x_size = 7;
	int increment = 8;
	for (int i = 0; i < size-1; i++) {
		if (i % 2 == 0) {
			increment += 4;
		} else {
			increment += 2;
		}
		x_size += increment;
	}
	int y_size = (size+2)*(size+3)/2-3;
	int mid = (x_size-1)/2;
	int door_width = (size-1)/2;
	int width = 1;
	int level = 3;
	int sublevel = 0;
	for (int i = 0; i < y_size; i++) {
		for (int j = 0; j < x_size; j++) {
			if (size >= 5 && j-mid == door_width - 1 && y_size-i==door_width+1) {
				ft_putchar('$');
			} else if (door_width*door_width >= (mid-j)*(mid-j) && y_size-i < 2*door_width+2) {
				ft_putchar('|');
			} else if ((width*width > (mid-j)*(mid-j))) {
				ft_putchar('*');
			} else if (mid-j == width) {
				ft_putchar('/');
			} else if (j-mid == width) {
				ft_putchar('\\');
			} else {
				ft_putchar(' ');
			}
		}
		ft_putchar('\n');
		width++;
		sublevel++;
		if (sublevel >= level) {
			width += (level+1)/2;
			sublevel = 0;
			level++;
		}
	} 
	return;
}

int main(int argc, char* argv[]) {
	if (argc > 1) {
		int size = *argv[1]-48;
		sastantua(size);
	}
	return 0;
}

