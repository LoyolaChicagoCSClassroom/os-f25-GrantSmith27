#include "rprintf.h"
#include <stdint.h>

#define width 80
#define height 25
#define default_color 0x07

//each text cell is 2 bytes, ASCII and COLOR
typedef struct termbuf{
   unsigned char ASCII;
   unsigned char COLOR;
} termbuf;

//starting address for VRAM
#define video_memory ((volatile termbuf*)0xB8000) 

//cursor
static int row=0;
static int column=0;

//forward declarations for putc
static void scroll(void);
static void newLine(void);

//writing one character advances the cursor
int putc(int ch) {
    if (ch == '\r') { 
      column = 0; return ch; 
    }
    if (ch == '\n') { 
      newline();
      return ch;
    }

    //store the ASCII and color at the current row and column
    video_memory[row * width + column].ASCII = (unsigned char)ch;
    video_memory[row * width + column].COLOR = default_color;

   //move cursor the right right until it needs to go to the next row
    if (++column >= width) newline();
    return ch;
}

//scrolling up shifts all the rows up one and clears the bottom
static void scroll(void) {
    for (int r = 1; r < height; r++) {
        for (int c = 0; c < width; c++) {
            video_memory[(r - 1) * width + c] = video_memory[r * width + c];
        }
    }
    for (int c = 0; c < width; c++) {
        video_memory[(height - 1) * width + c].ASCII = ' ';
        video_memory[(height - 1) * width + c].COLOR = default_color;
    }
}
// go to start of the next line if more space is needed 
static void newline(void) {
    column = 0;
    row++;
    if (row >= height) {
        scroll();
        row = height - 1;
    }
}

static unsigned get_cpl(void) {
    unsigned cs;
    __asm__ __volatile__("mov %%cs, %0" : "=r"(cs));
    return cs & 0x3;

void main(void) {
 //test
  esp_printf(putc, "Hello World!\r\n");
  //execution level demo. 23 lines so it can be displayed at the top
  esp_printf(putc, "Execution level: %u\r\n", get_cpl());
   for (int i = 0; i < 23; i++) {
        //one  line per loop. ends the line and moves to next row
         esp_printf(putc, "Scrolling %d\r\n", i);
    }
while (1) { }
}

