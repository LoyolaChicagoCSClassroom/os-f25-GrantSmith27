#include "rprintf.h"
#include <stdint.h>

#define video_memory 0xB8000
#define width 80
#define height 25
#define default_color 0x07

static int row=0;
static int column=0;


const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void putc(int ch){
   if (ch == '\n'){
       row++;
     } else {
      int index = term_row * width + column;
      video_memory[index] = (default_color <<8) | (unsigned char)ch;
      column;
      if(column >= width){
        column=0;
        row++;
      }
    }


   if (row >= height){

  scroll();
  row = height -1;

     }
}

void scroll(){
   for (int r =1; r < height, r++){
     for (int c=0; c< width; c++){
      video_memory[(r-1)*width + c] = video_memory[r*width + c];
      }
   }
  for (int c=0; c < width; c++{
    video_memory[(height-1)*width + c] = (default_color <<8 | ' ';
  }
}
