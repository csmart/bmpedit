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
char input[] = "";
char output[] = "out.bmp";
float threshold;
long fd_size;
char *fd_data;

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

#define BMP_ERROR "Please pass a BMP file to load.\nSee \'bmpedit -h\' for more information."

#define BUFFER 512

/*functions*/
void usage(void){
  printf("\nUsage: bmpedit [OPTIONS...] [input.bmp]\n\n");
  printf("DESCRIPTION:\n%s\n\n\n", USAGE_STR);
  printf("OPTIONS:\n%s\n\n", OPTIONS_STR);
}

void error(char msg[]) {
   fprintf(stderr,"Error: %s\n",msg);
   exit(1);
}

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
      printf("output is: %s\n", output);
      strcpy(output, argv[i+1]);
      printf("output is now: %s\n", output);
      i++;
    }else if (strcmp(argv[i],"-t") == 0){
      if (atof(argv[i+1]) < 0 || atof(argv[i+1]) > 1.0 ){
        error("Threshold must be between 0.0 and 1.0");
        exit(1);
      }
      printf("threshold is: %.1f\n", threshold);
      threshold = atof(argv[i+1]);
      printf("threshold is now: %.1f\n", threshold);
      i++;
    }else{
      printf("input is: %s\n", input);
      strcpy(input, argv[argc-1]);
      printf("input is: %s\n", input);
    }
  }
  return 0;
}

int open_file(char input[], char **fd_data){
  //get size of file for mmap
  struct stat fd_stat;
  if (stat(input, &fd_stat) == -1){
    return 1;
  }else{
    fd_size = fd_stat.st_size;
  }

  printf("size of file is: %lu\n",fd_stat.st_size);
  
  //open the file
  int fd = open(input, O_RDONLY);
  if (fd == -1){
    close(fd);
    return 1;
  }
  
  //memory map the file
  fd_data = mmap(NULL, fd_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (fd_data == MAP_FAILED){
    close(fd);
    return 1;
  }
  
  close(fd);
  return 0;
}

int main(int argc, char *argv[]){
  //parse all arguments
  if (parse_args(argc, argv)){
    exit(0);
  }
  
  //ensure we have an input file
  if (strcmp(input,"") == 0){
    error(BMP_ERROR);
  }

  //try to mmap the file, pass address of fd_data so we can edit inside function
  if (open_file(input, &fd_data)){
    error("Problem loading file.");
  }

  //read and print out size of mmap'd file

  //do more stuff
  printf("\n\nWe're doing stuff..\n\n");

  //no need to unmap memory as the process is about to terminate anyway
  int fd_munmap = munmap(fd_data,fd_size);
  return 0;
}





