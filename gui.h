#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include "font_types.h"
#include "font_prop14x16.c"

unsigned short *fb;
int scale = 4;

void draw_pixel(int x, int y);

void draw_pixel8(int x, int y);

void draw_wall(int x, int y);

int char_width(font_descriptor_t* fdes, int ch);

void draw_char(int x, int y, font_descriptor_t* fdes, char ch);

void draw_LCD();