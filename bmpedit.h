typedef struct {
  char input[256]; //input filename
  char output[256]; //output filename
  unsigned long fd_size; //total size of the image
  unsigned char *fd_data; //array to hold image data
  unsigned char *fd_data_w; //array for writing out modified image
  char magic_number[2]; //magic number of the file
  int width; //width of image in pixels
  int height; //height of image in pixels
  unsigned int bits; //bitdepth of image
  unsigned int file_size; //size of image, as per header
  unsigned int offset; //offset of bmpheader
  unsigned int data_size; //size of image data, as per header
  unsigned int compression; //the compression being used
} image;
