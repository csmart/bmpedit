/*
   Copyright 2013 "Christopher Smart" <u3227509@anu.edu.au>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
   */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

/*prototypes*/

/*variables*/
char input[256]; //storing the input from args[]
char output[256] = "out.bmp"; //default to this name for output bmp
float threshold; //storing the threshold for filter
unsigned long fd_size; //storing size of file
unsigned char *fd_data; //for reading
unsigned char *fd_data_w; //for writing
int fd_w; //output file descriptor

/*constants*/
#define USAGE_STR "\
  This program does simple edits of BMP image files. When the program\n\
  runs it first prints out the width and the height of the input image\n\
  within the BMP file. Once this is done a filter (or sequence of\n\
  filters) are applied to the image. The resulting image is also stored\n\
  using BMP format into an output file."

#define OPTIONS_STR "\
  -o [FILE]         Sets the output file (defaults to \"out.bmp\").\n\
  -t [0.0-1.0]      Applies this (rounded) value to threshold filter.\n\
  -h                Displays this usage message."

#define BMP_ERROR "\
Please pass a BMP file to load.\nSee \'bmpedit -h\' for more information."

/*functions*/
//print usage
void usage(void){
  printf("\nUsage: bmpedit [OPTIONS...] [input.bmp]\n\n");
  printf("DESCRIPTION:\n%s\n\n\n", USAGE_STR);
  printf("OPTIONS:\n%s\n\n", OPTIONS_STR);
}

//print errors
void error(char msg[]) {
   fprintf(stderr,"Error: %s\n",msg);
   exit(1);
}

//parse arguments
int parse_args(int argc, char *argv[]){
  if (argc < 2){
    error(BMP_ERROR);
    return 1;
  }
  int i;
  for (i=1;i<argc;i++){
    if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0){
      usage();
      return 1;
    }else if (strcmp(argv[i],"-o") == 0){
      strncpy(output, argv[i+1], 256);
      //testing
      printf("TESTING: output is: %s\n", output);
      i++;
    }else if (strcmp(argv[i],"-t") == 0){
      if (atof(argv[i+1]) < 0 || atof(argv[i+1]) > 1.0 ){
        error("Threshold must be between 0.0 and 1.0");
        exit(1);
      }
      threshold = atof(argv[i+1]);
      i++;
    }else{
      strncpy(input, argv[argc-1], 256);
      //testing
      printf("TESTING: input is: %s\n", input);
    }
  }

  return 0;
}

//open and mmap the input bmp
int open_file(char input[]){
  //get size of file for mmap
  struct stat fd_stat;
  if (stat(input, &fd_stat) == -1){
    return 1;
  }else{
    fd_size = fd_stat.st_size;
  }

  //testing
  printf("TESTING: size of file is: %lu\n",fd_size);
  
  //open the file
  int fd;
  fd = open(input, O_RDONLY);
  if (fd == -1){
    close(fd);
    return 1;
  }
  
  //read first two bytes, if not supported file, exit
  char magic_number[2];
  read(fd, magic_number, 2);
    
  if (strcmp(magic_number, "BM") != 0){
    close(fd);
    error("Not a supported file type.");
  }
  
  //should I check for DIB header?
  
  //memory map the file
  fd_data = mmap(NULL, fd_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (fd_data == MAP_FAILED){
    close(fd);
    return 1;
  }

  close(fd);
  return 0;
}

//print file details
int get_details(){
  //if file is bmp, each int is 4 bytes, width offset 12h, height offset 16h
  //we need to reverse the bits as it's little endian

  int width, height, bits, file_size, offset, data_size;
  width = fd_data[0x12] | fd_data[0x13] << 8 | fd_data[0x14] << 16 | fd_data[0x15] << 24;
  printf("Image width: %dpx\n",width);

  height = fd_data[0x16] | fd_data[0x17] << 8 | fd_data[0x18] << 16 | fd_data[0x19] << 24;
  printf("Image height: %dpx\n",height);
  
  bits =  fd_data[0x1C] | fd_data[0x1D] << 8;
  printf("Image bpp: %d\n",bits);

  file_size =  fd_data[0x02] | fd_data[0x03] << 8 | fd_data[0x04] << 16 | fd_data[0x05] << 24;
  printf("bmpheader.filesize: %d\n",file_size);

  offset =  fd_data[0x0A] | fd_data[0x0B] << 8 | fd_data[0x0C] << 16 | fd_data[0x0D] << 24;
  printf("bmpheader.offset: %d\n",offset);

  data_size =  fd_data[0x22] | fd_data[0x23] << 8 | fd_data[0x24] << 16 | fd_data[0x25] << 24;
  printf("dibheader.datasize: %d\n",data_size);

  printf("read until: %lu\n",fd_size);

  return 0;
}

//create output file, mmap, memcpy from input, modifications will be written at close
int write_file(char output[]){
  //open the file
  fd_w = open(output, O_RDWR|O_CREAT|O_TRUNC, 00660);
  if (fd_w == -1){
    close(fd_w);
    return 1;
  }

  //truncate the new file with the size of input
  int fd_w_trunc = truncate(output,fd_size);
  if (fd_w_trunc != 0){
    close(fd_w);
    return 1;
  }

  //mmap
  fd_data_w = mmap(NULL, fd_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_w, 0);
  if (fd_data_w == MAP_FAILED){
    close(fd_w);
    return 1;
  }
  
  //copy input to output
  memcpy(fd_data_w, fd_data, fd_size);

  return 0;
}

//run the filter process on the file
int filter(){

  //reminder:
  //might need a struct to hold info for the file, cause I need to access things like res for filter..

  printf("TESTING: you're in the filter function: %f\n\n",threshold);
  return 0;
}

//main function
int main(int argc, char *argv[]){
  //parse all arguments
  if (parse_args(argc, argv)){
    exit(0);
  }
  
  //ensure we have an input file
  if (strcmp(input,"") == 0){
    error(BMP_ERROR);
  }

  //try to mmap the file
  if (open_file(input)){
    error("Problem loading file.");
  }

  //print the details of the file
  if (get_details()){
    error("Problem looking up the details of the file.");
  }

  //try to mmap the output file
  if (write_file(output)){
    error("Problem writing to output file.");
  }

  //run filter
  filter();

  //run other

  //testing - do more stuff
  printf("\n\nWe're doing stuff..\n\n");

  //no need to unmap memory as the process is about to terminate anyway
  int fd_munmap = munmap(fd_data,fd_size);
  if (fd_munmap == -1){
    error("Could not unmap file in memory.");
  }

  //probably should unmap the output file as we want to close the fd
  int fd_w_munmap = munmap(fd_data_w,fd_size);
  if (fd_w_munmap == -1){
    error("Could not unmap output file in memory.");
  }

  //I'm not sure when I need to close this fd, can I modify fd_data_w after I've closed it?
  close(fd_w);
  
  return 0;
}


