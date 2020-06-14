#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"
#include "font_prop14x16.c"
font_descriptor_t* fdes = &font_winFreeSystem14x16;
unsigned short *fb;
int size_dead = 0;
int ptr;

void draw_pixel(int x, int y, int color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;
  }
}

void draw_pixel_size(int x, int y, int size, int color) {
  int i, j;
  for (i = 0; i < size; i++){
    for (j = 0; j < size; j++){
      draw_pixel(x-i, y-j, color);
    }
  }
}

int char_width(int ch) {
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

void draw_char(int x, int y, char ch, int size, int color) {
  int w = char_width(ch);
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
          draw_pixel_size(x+size*j, y+size*i, size, color);
        }
        val<<=1;
      }
      ptr++;
    }
  }
}

int main(){

    uint32_t rgb_knobs_value;
    unsigned char *mem_base;
    unsigned char *parlcd_mem_base;
    uint32_t val_line=5;

    long int ttime;
    struct tm *u;
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};

    fb  = (unsigned short *)malloc(320*480*2);
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    /* If mapping fails exit with error code */
    if (mem_base == NULL)
        exit(1);

    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        exit(1);
    parlcd_hx8357_init(parlcd_mem_base);
    for (ptr = 0; ptr < 320*480 ; ptr++) {
        fb[ptr]=0u;
    }
    // char str[] = "DEADLINE"; // 8
    char str[] = "11111111"; // 8
    char *ch = str;
    int x = 20;
    // int x = 49-(standard*36); centr
    for (int i=0; i<8; i++) {
        draw_char(x, 20, *ch, size_dead, 63519);
        x+=size_dead*char_width(*ch)+2;
        ch++;
    }

    printf("Deadline\n");
    printf("%d\n", 23-u->tm_hour);
    printf("%d\n", 60-u->tm_min);

    while (101)
    {
        // current time
        ttime = time (NULL);
        u = localtime(&ttime);
        
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 1227133513;
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 1687308580;
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 843654290;
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 421827145;
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    

    // printf ("Current time: %s\n", ctime (&ttime) );
    // printf("%d\n", u->tm_hour);
    // printf("%d\n", u->tm_min);

    printf("Deadline\n");
    printf("%d\n", 23-u->tm_hour);
    printf("%d\n", 60-u->tm_min);
    return 0;
}