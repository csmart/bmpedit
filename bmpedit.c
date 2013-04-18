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
#include "bmpedit.h"

/*prototypes*/

/*variables*/
float threshold; //storing the threshold for filter
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
int parse_args(image *img, int argc, char *argv[]){
  printf("argc: %d\n\n",argc);
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
      strncpy(img->output, argv[i+1], 256);
      //testing
      printf("TESTING: output is: %s\n", img->output);
      i++;
    }else if (strcmp(argv[i],"-t") == 0){
      if (atof(argv[i+1]) < 0 || atof(argv[i+1]) > 1.0 ){
        error("Threshold must be between 0.0 and 1.0");
        exit(1);
      }
      threshold = atof(argv[i+1]);
      i++;
    }else{
      strncpy(img->input, argv[argc-1], 256);
      //testing
      printf("TESTING: input is: %s\n", img->input);
    }
  }

  return 0;
}

//open and mmap the input bmp
int open_file(image *img){
  //get size of file for mmap
  struct stat fd_stat;
  if (stat(img->input, &fd_stat) == -1){
    return 1;
  }else{
    img->fd_size = fd_stat.st_size;
  }

  //open the file
  int fd;
  fd = open(img->input, O_RDONLY);
  if (fd == -1){
    close(fd);
    return 1;
  }
  
  //read first two bytes, if not supported file, exit
  read(fd, img->magic_number, 2);
  
  if (strcmp(img->magic_number, "BM") != 0){
    close(fd);
    error("Not a supported file type.");
  }

  //should I check for DIB header?
  
  //memory map the file
  img->fd_data = mmap(NULL, img->fd_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (img->fd_data == MAP_FAILED){
    close(fd);
    return 1;
  }

  close(fd);
  return 0;
}

//get file details
int get_details(image *img){
  //if file is bmp, each int is 4 bytes, width offset 12h, height offset 16h
  //we need to reverse the bits as it's little endian

//  int width, height, bits, file_size, offset, data_size;
  img->width = img->fd_data[0x12] | img->fd_data[0x13] << 8 | img->fd_data[0x14] << 16 | img->fd_data[0x15] << 24;
  img->height = img->fd_data[0x16] | img->fd_data[0x17] << 8 | img->fd_data[0x18] << 16 | img->fd_data[0x19] << 24;
  img->bits = img->fd_data[0x1C] | img->fd_data[0x1D] << 8;
  img->file_size = img->fd_data[0x02] | img->fd_data[0x03] << 8 | img->fd_data[0x04] << 16 | img->fd_data[0x05] << 24;
  img->offset = img->fd_data[0x0A] | img->fd_data[0x0B] << 8 | img->fd_data[0x0C] << 16 | img->fd_data[0x0D] << 24;
  img->data_size = img->fd_data[0x22] | img->fd_data[0x23] << 8 | img->fd_data[0x24] << 16 | img->fd_data[0x25] << 24;
  img->compression = img->fd_data[0x1E] | img->fd_data[0x1F] << 8 | img->fd_data[0x20] << 16 | img->fd_data[0x21] << 24;

  printf("Image width: %dpx\n",img->width);
  printf("Image height: %dpx\n",img->height);
  printf("Image bpp: %d\n",img->bits);
  printf("bmpheader.filesize: %d\n",img->file_size);
  printf("bmpheader.offset: %d\n",img->offset);
  printf("dibheader.datasize: %d\n",img->data_size);
  printf("read until: %lu\n",img->fd_size);
  printf("compression type: %d\n",img->compression);

  //24bit is BGR each 8 bits in value, starting bottom left, going horizontally
  printf("\n\n#####\nthe value of the 1st pixel is: %d %d %d\n", img->fd_data[img->offset+0],img->fd_data[img->offset+1],img->fd_data[img->offset+2]);
  printf("\n\n#####\nthe value of the 2nd pixel is: %d %d %d\n", img->fd_data[img->offset+3],img->fd_data[img->offset+4],img->fd_data[img->offset+5]);
  printf("\n\n#####\nthe value of the 3rd pixel is: %d %d %d\n", img->fd_data[img->offset+6],img->fd_data[img->offset+7],img->fd_data[img->offset+8]);
  printf("\n\n#####\nthe value of the 4th pixel is: %d %d %d\n", img->fd_data[img->offset+9],img->fd_data[img->offset+10],img->fd_data[img->offset+11]);
  printf("\n\n#####\nthe value of the 5th pixel is: %d %d %d\n", img->fd_data[img->offset+12],img->fd_data[img->offset+13],img->fd_data[img->offset+14]);
  printf("\n\n#####\nthe value of the 6th pixel is: %d %d %d\n", img->fd_data[img->offset+15],img->fd_data[img->offset+16],img->fd_data[img->offset+17]);


  return 0;
}

//create output file, mmap, memcpy from input, modifications will be written at close
int write_file(image *img){
  //open the file
  fd_w = open(img->output, O_RDWR|O_CREAT|O_TRUNC, 00660);
  if (fd_w == -1){
    close(fd_w);
    return 1;
  }

  //truncate the new file with the size of input
  int fd_w_trunc = truncate(img->output,img->fd_size);
  if (fd_w_trunc != 0){
    close(fd_w);
    return 1;
  }

  //mmap
  img->fd_data_w = mmap(NULL, img->fd_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_w, 0);
  if (img->fd_data_w == MAP_FAILED){
    close(fd_w);
    return 1;
  }
  
  //copy input to output
  memcpy(img->fd_data_w, img->fd_data, img->fd_size);

  return 0;
}

//run the filter process on the file
int filter(image *img){

  //brightness with sliding scale 0.0 = darkest, 1.0 = brightest, 0.5 = normal
  /*
  brightness of 0.0 would be 100% darker
  brightness of 0.25 would be 50% darker
  brightness of 0.5 would be 0% brighter
  brightness of 0.75 would be 50% brighter
  brightness of 1.0 would be 100% brighter
  */
  
  int brightness,scaling_factor;

  scaling_factor = ((int)(threshold*100)-50) * 2;
  brightness = 255 * scaling_factor / 100;

  int j;
  for (j=0;j<((480*640)*3);j++){
    int new_value = img->fd_data_w[(img->offset+j)]+brightness;
    if (new_value >= 255){
      img->fd_data_w[(img->offset+j)] = 255;
    }else if (new_value < 0){
      img->fd_data_w[(img->offset+j)] = 0;
    }else{
      img->fd_data_w[(img->offset+j)] = new_value;
    }
  }

  return 0;
}

//main function
int main(int argc, char *argv[]){
  //set up struct to hold data for our image, pointer to pass to functions
  image img = {.output = "output.bmp"};
  image *p_img;
  p_img = &img;

  //parse all arguments
  if (parse_args(p_img, argc, argv)){
    exit(0);
  }
  
  //ensure we have an input file
  if (strcmp(img.input,"") == 0){
    error(BMP_ERROR);
  }

  //try to mmap the file
  if (open_file(p_img)){
    error("Problem loading file.");
  }

  //print the details of the file
  if (get_details(p_img)){
    error("Problem looking up the details of the file.");
  }

  //exit if we have a compressed bmp (zero is uncompressed)
  if (img.compression){
    error("Sorry, cannot handle compressed images.");
  }

  //try to mmap the output file
  if (write_file(p_img)){
    error("Problem writing to output file.");
  }

  //run filter
  filter(p_img);

  //run other

  //testing - do more stuff
  printf("\n\nWe're doing stuff..\n\n");

  //no need to unmap memory as the process is about to terminate anyway
  int fd_munmap = munmap(img.fd_data,img.fd_size);
  if (fd_munmap == -1){
    error("Could not unmap file in memory.");
  }

  //probably should unmap the output file as we want to close the fd
  int fd_w_munmap = munmap(img.fd_data_w,img.fd_size);
  if (fd_w_munmap == -1){
    error("Could not unmap output file in memory.");
  }

  //I'm not sure when I need to close this fd, can I modify fd_data_w after I've closed it?
  close(fd_w);
  
  return 0;
}


