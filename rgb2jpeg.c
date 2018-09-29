#include <stdio.h> 
#include <stdlib.h> 
#include <jpeglib.h> 
#include <setjmp.h> 

#define MIN(x,y) ((x) < (y) ? (x) : (y)) 

int main(int argc, char **argv) {

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

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr; 
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;


  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 100, TRUE);   // Quality = 100
  jpeg_start_compress(&cinfo, TRUE);

  JSAMPROW row_pointer[1];
  int row_stride = cinfo.image_width * 3;

  while (cinfo.next_scanline < cinfo.image_height) {

    unsigned char *row_rgbs = (unsigned char *)calloc(3*cinfo.image_width, sizeof(unsigned char));
    for (int i=0; i < cinfo.image_width * 3; i++) {
      int byte;
      if (scanf("%d", &byte) != 1) {
        fprintf(stderr,"Cannot read RGB value... \n");
	exit(1);
      }
      row_rgbs[i] = (unsigned char)(byte);
    }
    row_pointer[0] = row_rgbs;
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  } 
 
  (void) jpeg_finish_compress(&cinfo);
  fclose(outfile);
  jpeg_destroy_compress(&cinfo);

}
