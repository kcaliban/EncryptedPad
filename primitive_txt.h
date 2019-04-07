#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

#define ALLOWED_CHARS   5000
#define QUIT_KEY        KEY_F(5)

int get_pos(int, int, int[ALLOWED_CHARS]);
int primitive_txt(char[ALLOWED_CHARS]);
