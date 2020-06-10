#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>

#include "snake.h"

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"
#include "font_prop14x16.c"

unsigned short *fb;
int scale = 4;

void draw_pixel(int x, int y) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = 0x1f<<11;
  }
}

void draw_pixel8(int x, int y) {
  int i, j;
  for (i = 0; i < scale; i++){
    for (j = 0; j < scale; j++){
      draw_pixel(x+i, y+i);
    }
  }
}

int char_width(font_descriptor_t* fdes, int ch) {
  int width = 0;
  if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) {
    ch -= fdes->firstchar;
    if (!fdes->width) {
      width = fdes->maxwidth;
    } else {
      width = fdes->width[ch];
    }
  }
  return width;
}

void draw_char(int x, int y, font_descriptor_t* fdes, char ch) {
  int w = char_width(fdes, ch);
  if (w > 0) {
    const font_bits_t *ptr;
    if (fdes->offset) {
      ptr = &fdes->bits[fdes->offset[ch-fdes->firstchar]];
      ptr = fdes->bits + fdes->offset[ch-fdes->firstchar];
    } else {
      int bw = (fdes->maxwidth+15)/16;
      ptr = fdes->bits + (ch-fdes->firstchar)*bw*fdes->height;
    }
    printf("Znak %c na %i, %i, sirka %i\n", ch, x, y, w);
    int i, j;
    for (i = 0; i < fdes->height; i++){
      font_bits_t val = *ptr;
      for (j = 0; j < w; j++){
        if ((val&0x8000) != 0) {
          draw_pixel8(x+scale*j, y+scale*i);
        }
        val<<=1;
      }
      ptr++;
    }
  }
}



int main() {

    unsigned char *mem_base;
    unsigned char *parlcd_mem_base;
    uint32_t val_line=5;
    uint32_t rgb_knobs_value;
    int i;
    int ptr;

    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt); 
    newt = oldt; 
    newt.c_lflag &= ~(ICANON); 
    newt.c_cc[VMIN] = 0;
    newt.c_cc[VTIME] = 0;
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    fb  = (unsigned short *)malloc(320*480*2);
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);


    snake_t snake;
    desk_t desk;
    food_t food;

    desk = initDesk(100, 100, 0, 0);
    snake = initSnake(100, 100, 5, 2, 1);
    food = initFood(7, 7);

    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};
    while (1) {

        drawDesk(&desk, &snake, &food);
        moveSnake(&snake);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }

    /* If mapping fails exit with error code */
    if (mem_base == NULL)
        exit(1);

    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value = 16711935; //pink

    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;

    // LED Line
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 10 * 1000 * 1000};
    val_line = 15;
    // val_line = 1227133513;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    for (i=0; i<30; i++) {
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
        val_line<<=1;
        //printf("LED val 0x%x\n", val_line);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    
    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        exit(1);
    parlcd_hx8357_init(parlcd_mem_base);
    
    font_descriptor_t* fdes = &font_winFreeSystem14x16;
    for (ptr = 0; ptr < 320*480 ; ptr++) {
        fb[ptr]=0u;
    }

    parlcd_write_cmd(parlcd_mem_base, 0x2c);
        for (ptr = 0; ptr < 480*320 ; ptr++) {
            parlcd_write_data(parlcd_mem_base, fb[ptr]);
        }

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;

}