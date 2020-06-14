#include "font_types.h"
#include "font_prop14x16.c"

void draw_pixel_size(int x, int y, int size, int color);

void draw_wall(int c, int r);

void draw_food(int x, int y);

void draw_snake(int x, int y);

int char_width(font_descriptor_t* fdes, int ch);

void draw_char(int x, int y, font_descriptor_t* fdes, char ch, int size, int color);

void draw_score(int score);

void draw_score_2_snakes(int score);

void draw_time(int sec);
