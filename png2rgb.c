#include <stdio.h> 
#include <stdlib.h> 
#include <libpng16/png.h> 

#define MIN(x,y) ((x) < (y) ? (x) : (y))


int main(int argc, char **argv) {

  unsigned int max_desired_width;
  if ((argc != 3) || (sscanf(argv[2], "%u", &max_desired_width) != 1)) {
    fprintf(stderr, "Usage: %s <png file path> <maximum width>\n", argv[0]);
    exit(1);
  }

  FILE *infile = fopen(argv[1], "rb");
  if (!infile) {
    fprintf(stderr,"Cannot read file '%s'\n", argv[1]);
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

  // Do the scaling
  int desired_width = MIN(max_desired_width, w);
  double aspect_ratio = (double)(w) / (double)(h);
  int desired_height = (int)((double)(desired_width) / aspect_ratio);
  int stride_width = (int)(w / desired_width);
  int stride_height = (int)(h / desired_height);

  desired_width = w / stride_width;
  desired_height = h / stride_height;

  fprintf(stdout, "%d\n", desired_width);
  fprintf(stdout, "%d\n", desired_height);

  for (int row = 0; row < desired_height; row++) {
    for (int col = 0; col < desired_width; col++) {
      int count = 0;
      unsigned int new_red = 0;
      unsigned int new_green = 0;
      unsigned int new_blue = 0;
      for (int srow=MIN(row * stride_height, h); srow < MIN((row+1)*stride_height, h); srow++) {
        for (int scol=MIN(col * stride_width, w); scol < MIN((col+1)*stride_width, w); scol++) {
          new_red += pixels[srow*w*bpp + scol*bpp];
          new_green += pixels[srow*w*bpp + scol*bpp + 1];
          new_blue += pixels[srow*w*bpp + scol*bpp + 2];
          count++;
        }
      }

      new_red = (unsigned int)((double)new_red / (double)count);
      new_green = (unsigned int)((double)new_green / (double)count);
      new_blue = (unsigned int)((double)new_blue / (double)count);

      fprintf(stdout,"%u\n%u\n%u\n", new_red, new_green, new_blue);
    }
  }

  fclose(infile);

  // memory leaks
  exit(0);
}

  

