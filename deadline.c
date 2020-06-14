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
int size_dead = 6;
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

    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value = 16711680; //red
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;

    val_line = 4;
    double p = 5;
    for (int i=0; i<10; i++) {
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
        val_line += pow(2, p);
        p+=3;
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }


    char str[] = "DEADLINE"; // 8
    char *ch = str;
    int x = 30;
    // int x = 49-(standard*36); centr
    for (int i=0; i<8; i++) {
        draw_char(x, 35, *ch, size_dead, 63488);
        x+=size_dead*char_width(*ch)+2;
        ch++;
    }

    // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }

    ttime = time (NULL);
    u = localtime(&ttime);

    while (1)
    {
        for (ptr = 24000; ptr < 480*320 ; ptr++) {
            fb[ptr] = 0;
        }
        // current time
        ttime = time (NULL);
        u = localtime(&ttime);
        int min = 60-u->tm_min;
        int sec = 60-u->tm_sec;
        x = 200;
        if (min==0){
            draw_char(x, 130, '0', 6, 63488); // size_score = 4;
        }
        char str1[3] = "0";
        int idx = 0;
        while (min!=0)
        {
            str1[idx] = min % 10 + '0';
            min /= 10;
            idx++;
        }
        for (int i = idx-1; i >= 0; i--){
            draw_char(x, 130, str1[i], 6, 63488); // size_score = 4;
            x+=6*char_width(str1[i])+2; // size_score = 4;
        }

        x = 200;
        if (sec==0){
            draw_char(x, 230, '0', 6, 63488); // size_score = 4;
        }
        char str2[3] = "0";
        int idx2 = 0;
        while (sec!=0)
        {
            str2[idx2] = sec % 10 + '0';
            sec /= 10;
            idx2++;
        }
        for (int i = idx2-1; i >= 0; i--){
            draw_char(x, 230, str2[i], 6, 63488); // size_score = 4;
            x+=6*char_width(str2[i])+2; // size_score = 4;
        }

        
        // draw LCD
        parlcd_write_cmd(parlcd_mem_base, 0x2c);
        for (ptr = 0; ptr < 480*320 ; ptr++) {
            parlcd_write_data(parlcd_mem_base, fb[ptr]);
        }
    }
    return 0;
}