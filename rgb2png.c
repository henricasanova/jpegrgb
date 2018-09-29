#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <png.h>



int main(int argc, char *argv[])
{
  // Make sure that the output filename argument has been provided
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <jpg file path>\n", argv[0]);
    exit(1);
  }

  char *file_name = argv[1];
  FILE *outfile;
  if ((outfile = fopen(file_name, "w")) == NULL) {
    fprintf(stderr, "Could not open file %s for writing", file_name);
    exit(1);
  }


  int width;
  int height;
  if ((scanf("%d", &width) != 1) || (scanf("%d", &height) != 1)) {
    fprintf(stderr,"Cannot read in width and height\n");
    fclose(outfile);
    exit(1);
  }

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
  png_set_IHDR(png_ptr, info_ptr, width, height,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  // Allocate memory for one row (3 bytes per pixel)
  png_bytep row_buffer = (png_bytep) malloc(3 * width * sizeof(png_byte));


  // Write image data
  for (int row=0; row < height; row++) {
      for (int col=0; col < width; col++) {
         int bytes[3];
         if ((scanf("%d", &(bytes[0])) != 1) || (bytes[0] < 0) || (bytes[0] > 255)  ||
             (scanf("%d", &(bytes[1])) != 1) || (bytes[1] < 0) || (bytes[1] > 255)  ||
             (scanf("%d", &(bytes[2])) != 1) || (bytes[2] < 0) || (bytes[2] > 255)) {
           fprintf(stderr,"Invalid RGB value\n");
           exit(1); 
         }
         row_buffer[col * 3 + 0] = (png_byte)(bytes[0]);
         row_buffer[col * 3 + 1] = (png_byte)(bytes[1]);
         row_buffer[col * 3 + 2] = (png_byte)(bytes[2]);
      }
      png_write_row(png_ptr, row_buffer);
  }

  // End write
  png_write_end(png_ptr, NULL);
  free(row_buffer);
  fclose(outfile);

  exit(0);
}

