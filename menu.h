#include "font_types.h"
// #include "snake.h"

void draw_pixel(int x, int y, int color);

void draw_EndGame(unsigned short *fb1, int score, int retry, int quit);

void draw_Menu(unsigned short *fb1, int standard, int demo, int exit);

void draw_speed_ctrl(unsigned short *fb1, int slow, int medium, int fast);

void drawDesk(desk_t *desk, snake_t *snake, food_t *food, int sec, unsigned short *fb1);

void drawDesk_2_snakes(desk_t *desk, snake_t *snake, snake_t *snake2, food_t *food, int sec, unsigned short *fb1);