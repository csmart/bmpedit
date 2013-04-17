typedef struct {
  char input[256];
  char output[256];
  unsigned long fd_size;
  unsigned char *fd_data;
  unsigned char *fd_data_w;
  char magic_number[2];
  int width;
  int height;
  int bits;
  int file_size;
  int offset;
  int data_size;
} image; //maybe we create it here, seeing as we only ever have one image..
