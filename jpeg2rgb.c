#include <stdio.h> 
#include <stdlib.h> 
#include <jpeglib.h> 
#include <setjmp.h> 

#define MIN(x,y) ((x) < (y) ? (x) : (y)) 

int main(int argc, char **argv) {

  unsigned int max_desired_width;
  if ((argc != 3) || (sscanf(argv[2], "%u", &max_desired_width) != 1)) {
    fprintf(stderr, "Usage: %s <jpg file path> <maximum width>\n", argv[0]);
    exit(1);
  }

  char *file_name = argv[1];
  FILE * infile;
  if ((infile = fopen(file_name, "r")) == NULL) {
    fprintf(stderr, "Could not open file %s for reading", file_name);
    exit(1);
  }

  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr; 
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  int real_width = (int)(cinfo.output_width);
  int real_height = (int)(cinfo.output_height);
  int desired_width = MIN(max_desired_width, real_width);
  double aspect_ratio = (double)(real_width) / (double)(real_height);
  int desired_height = (int)((double)(desired_width) / aspect_ratio);
  int stride_width = (int)(real_width / desired_width);
  int stride_height = (int)(real_height / desired_height);

  desired_width = real_width / stride_width;
  desired_height = real_height / stride_height;

  int row_stride = cinfo.output_width * cinfo.output_components;
  JSAMPARRAY buffer;
  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  unsigned int **real_reds;
  real_reds = (unsigned int**)calloc(real_height, sizeof(unsigned int*));
  for (int i=0; i < real_height; i++) {
    real_reds[i] = (unsigned int*)calloc(real_width, sizeof(unsigned int));
  }
  unsigned int **real_greens;
  real_greens = (unsigned int**)calloc(real_height, sizeof(unsigned int*));
  for (int i=0; i < real_height; i++) {
    real_greens[i] = (unsigned int*)calloc(real_width, sizeof(unsigned int));
  }
  unsigned int **real_blues;
  real_blues = (unsigned int**)calloc(real_height, sizeof(unsigned int*));
  for (int i=0; i < real_height; i++) {
    real_blues[i] = (unsigned int*)calloc(real_width, sizeof(unsigned int));
  }


  for (int row = 0; row < cinfo.output_height; row++) {
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    unsigned char* pixel_row = (unsigned char*)(buffer[0]);
    for(int col = 0; col < cinfo.output_width; col++) {
      real_reds[row][col] = (unsigned int)(*pixel_row++);
      real_greens[row][col] = (unsigned int)(*pixel_row++);
      real_blues[row][col] = (unsigned int)(*pixel_row++);
    }
  }

  (void) jpeg_finish_decompress(&cinfo);
  fclose(infile);

  fprintf(stdout, "%d\n", desired_width);
  fprintf(stdout, "%d\n", desired_height);

  for (int row = 0; row < desired_height; row++) {
    for (int col = 0; col < desired_width; col++) {
      int count = 0;
      unsigned int new_red = 0;
      unsigned int new_green = 0;
      unsigned int new_blue = 0;
      for (int srow=MIN(row * stride_height, real_height); srow < MIN((row+1)*stride_height, real_height); srow++) {
        for (int scol=MIN(col * stride_width, real_width); scol < MIN((col+1)*stride_width, real_width); scol++) {
          new_red += real_reds[srow][scol];
          new_green += real_greens[srow][scol];
          new_blue += real_blues[srow][scol];
          count++;
        }
      }

      new_red = (unsigned int)((double)new_red / (double)count);
      new_green = (unsigned int)((double)new_green / (double)count);
      new_blue = (unsigned int)((double)new_blue / (double)count);

      fprintf(stdout,"%u\n%u\n%u\n", new_red, new_green, new_blue);
    }
  }
}
