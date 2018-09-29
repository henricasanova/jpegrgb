#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <png.h> 
#include <setjmp.h> 

#define MIN(x,y) ((x) < (y) ? (x) : (y)) 

unsigned short _rotr_short(unsigned short value, int shift) {
    if (shift == 16)
      return value;
    return (value >> shift) | (value << (16 - shift));
}

unsigned short _rotl_short(unsigned short value, int shift) {
    if (shift == 16)
      return value;
    return (value << shift) | (value >> (16 - shift));
}

unsigned short _rotl_byte(unsigned char value, int shift) {
    if (shift == 8)
      return value;
    return (value << shift) | (value >> (8 - shift));
}

unsigned short _rotr_byte(unsigned char value, int shift) {
    if (shift == 8)
      return value;
    return (value >> shift) | (value << (8 - shift));
}



void print_binary(char *msg, unsigned int number)
{
    printf("%s ", msg);
    unsigned int mask = 1 << 31;
    for (int i=0; i < 32; i++) {
        if (i % 8 == 0) putc(' ', stdout);
        putc((number & mask) ? '1' : '0', stdout);
        mask = mask >> 1;
    }
    putc('\n', stdout);
}

int main(int argc, char **argv) {

  if (argc != 4) {
    fprintf(stderr, "Usage: %s < scramble | descramble > <input jpg file path> <output jpg file path>\n", argv[0]);
    exit(1);
  }

  char scramble;

  if (!strcmp(argv[1], "scramble")) {
      scramble = 1;
  } else if (!strcmp(argv[1], "descramble")) {
      scramble = 0;
  } else {
    fprintf(stderr, "Invalid first argument (should be 'scramble' or 'descramble')\n");
    exit(1);
  }

  char *infile_name = argv[2];
  FILE *infile;
  if ((infile = fopen(infile_name, "r")) == NULL) {
    fprintf(stderr, "Could not open file %s for reading", infile_name);
    exit(1);
  }

  char *outfile_name = argv[3];
  FILE *outfile;
  if ((outfile = fopen(outfile_name, "w")) == NULL) {
    fprintf(stderr, "Could not open file %s for writing", outfile_name);
    exit(1);
  }

#if 0
    // Apply the scrambling/unscrambling
  
    /*
      321098765432109876543210
      rrrrrrrrggggggggbbbbbbbb
  
      rotate rrrrrrrrgggggggg 5 positions to the right
      rotate bbbbbbbb 6 positions to the left
  
      if 2nd bit of gggggggg == 0 then flip all bits in bbbbbbbb
      if 3rd bit of bbbbbbbb == 0 then flip all bits in rrrrrrrr
    */

    for (int row = 0; row < real_height; row++) {
      for (int col = 0; col < real_width; col++) {
        unsigned int red = real_reds[row][col];
        unsigned int green = real_greens[row][col];
        unsigned int blue = real_blues[row][col];

        printf("------------------------------------------\n");
        printf("r=%d g=%d b=%d\n",red, green, blue);
        print_binary("original ", (red << 16) + (green << 8) + blue);
  
        if (scramble) {

          unsigned short rg = ((green) + (red << 8));
          //print_binary("green+red ", (unsigned int)rg);
          rg = _rotr_short(rg, 3);
          //print_binary("rotate -> ", (unsigned int)rg);
  
          blue = _rotl_byte(blue, 6);
  
          if (!(rg & 64)) {
              blue = blue ^ 255;
          }
  
          if (!(blue & 32)) {
              rg = rg ^ (255 << 8);
          }
  
          unsigned int threebytes = (rg << 8) + blue;
  
  
          real_reds[row][col] = (threebytes >> 16) & 255;
          real_greens[row][col] = (threebytes >> 8) & 255;
          real_blues[row][col] = threebytes & 255;

        } else {
          // Apply the descrambling
          if (!(blue & 32)) {
              red = red ^ 255;
          }
          if (!(green & 64)) {
              blue = blue ^ 255;
          }
          blue = _rotr_byte(blue, 6);

          unsigned short rg = ((green) + (red << 8));
          rg = _rotl_short(rg, 3);

          real_reds[row][col] = (rg >> 8) & 255;
          real_greens[row][col] = (rg) & 255;
          real_blues[row][col] = blue;
          
        }

        unsigned threebytes = (real_reds[row][col] << 16) + (real_greens[row][col] << 8) + real_blues[row][col];
        print_binary("final: ",threebytes);
        printf("r=%d g=%d b=%d\n",real_reds[row][col], real_greens[row][col], real_blues[row][col]);
      }
    }

#endif



}