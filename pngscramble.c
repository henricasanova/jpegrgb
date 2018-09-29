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
    fprintf(stderr, "Usage: %s < scramble | descramble > <input png file path> <output png file path>\n", argv[0]);
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
  if ((infile = fopen(infile_name, "rb")) == NULL) {
    fprintf(stderr, "Could not open file %s for reading", infile_name);
    exit(1);
  }

  char *outfile_name = argv[3];
  FILE *outfile;
  if ((outfile = fopen(outfile_name, "wb")) == NULL) {
    fprintf(stderr, "Could not open file %s for writing", outfile_name);
    exit(1);
  }

  unsigned char sig[8];
  if (fread(sig, 1, sizeof(sig), infile) < 8) {
    fclose(infile);
    fprintf(stderr,"Invalid PNG file\n");
    exit(1);
  }
  if (png_sig_cmp(sig, 0, 8)) {
    fclose(infile);
    fprintf(stderr,"Invalid PNG file\n");
    exit(1);
  }



  // Read PNG file
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png == NULL) {
    fclose(infile);
    fprintf(stderr,"Cannot allocate read struct\n");
    exit(1);
  }

  png_infop info = png_create_info_struct(png);
  if (info == NULL) {
    png_destroy_read_struct(&png, NULL, NULL);
    fclose(infile);
    fprintf(stderr,"Cannot allocate info struct\n");
    exit(1);
  }

  // pass open file to png struct
  png_init_io(png, infile);

  // skip signature bytes (we already read those)
  png_set_sig_bytes(png, sizeof(sig));

  // get image information
  png_read_info(png, info);


  png_uint_32 w = png_get_image_width(png, info);
  png_uint_32 h = png_get_image_height(png, info);

  // set least one byte per channel
  if (png_get_bit_depth(png, info) < 8) {
      png_set_packing(png);
  }

  // if transparency, convert it to alpha
  if (png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
  }

  switch(png_get_color_type(png, info)) {


    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGBA:
        break;

    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
    case PNG_COLOR_TYPE_PALETTE:
    default:
      png_destroy_read_struct(&png, &info, NULL);
      fclose(infile);
      fprintf(stderr,"Cannot determine color type\n");
      exit(1);
  }

  png_uint_32 bpp = png_get_rowbytes(png, info) / w;

  png_set_interlace_handling(png);

  png_read_update_info(png, info);

  // allocate pixel buffer
  unsigned char *pixels = (unsigned char*)calloc(w*h*bpp, sizeof(unsigned char));

  // setup array with row pointers into pixel buffer
  png_bytep rows[h];
  unsigned char *p = pixels;
  for(int i = 0; i < h; i++) {
      rows[i] = p;
      p += w * bpp;
  }

  // read all rows (data goes into 'pixels' buffer)
  png_read_image(png, rows);

  png_read_end(png, NULL);

  png_destroy_read_struct(&png, &info, NULL);



  // Apply the scrambling/unscrambling

  /*
    321098765432109876543210
    rrrrrrrrggggggggbbbbbbbb

    rotate rrrrrrrrgggggggg 5 positions to the right
    rotate bbbbbbbb 6 positions to the left

     if 2nd bit of gggggggg == 0 then flip all bits in bbbbbbbb
    if 3rd bit of bbbbbbbb == 0 then flip all bits in rrrrrrrr
   */

  unsigned char **row_buffers = (unsigned char **)calloc(h, sizeof(unsigned char *));
  for (int i=0; i < h; i++) {
    row_buffers[i] = (unsigned char *)calloc(3*w, sizeof(unsigned char));
  }

  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
        unsigned char red   = rows[row][col * bpp + 0];
        unsigned char green = rows[row][col * bpp + 1];
        unsigned char blue  = rows[row][col * bpp + 2];

        //printf("------------------------------------------\n");
        //printf("r=%d g=%d b=%d\n",red, green, blue);
        //print_binary("original ", (red << 16) + (green << 8) + blue);

        unsigned char target_red;
        unsigned char target_green;
        unsigned char target_blue;
  
        if (scramble) {

          unsigned short rg = ((green) + (red << 8));
          //print_binary("green+red ", (unsigned int)rg);
          rg = _rotr_short(rg, 3);
          //print_binary("rotate -> ", (unsigned int)rg);
  
          blue = _rotl_byte((unsigned char) blue, 6);
          if (!(rg & 64)) {
              blue = blue ^ 255;
          }
          if (!(blue & 32)) {
              rg = rg ^ (255 << 8);
          }
          unsigned int threebytes = (rg << 8) + blue;
  
          target_red = (rg >> 8) & 255;
          target_green = rg & 255;
          target_blue = blue;

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

          target_red = (rg >> 8) & 255;
          target_green = (rg) & 255;
          target_blue = blue;
          
        }

        unsigned int threebytes = ((unsigned int)target_red << 16) + ((unsigned int)target_green << 8) + (unsigned int)target_blue;
        //print_binary("final: ",threebytes);
        //printf("r=%d g=%d b=%d\n",target_red, target_green, target_blue);

        row_buffers[row][col * 3 + 0] = target_red;
        row_buffers[row][col * 3 + 1] = target_green;
        row_buffers[row][col * 3 + 2] = target_blue;
      }
    }

    // Save file

  {
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  png_bytep row = NULL;
	
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose(outfile);
    fprintf(stderr, "Cannot allocate write struct\n");
    exit(1);
  }

  // Initialize info structure
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr,  NULL);
    fclose(outfile);
    fprintf(stderr, "Cannot allocated info struct\n"); 
    exit(1);
  }

  // Pass open file to png struct
  png_init_io(png_ptr, outfile);

  // Write header 
  png_set_IHDR(png_ptr, info_ptr, w, h,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  // Write image data
  for (int row=0; row < h; row++) {
      png_write_row(png_ptr, row_buffers[row]);
  }

  // End write
  png_write_end(png_ptr, NULL);
  fclose(outfile);
  }

  exit(0);
}

