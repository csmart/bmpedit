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

/*prototypes*/

/*variables*/
char output[] = "out.bmp";
float threshold;

/*constants*/
#define USAGE_STR "This program does simple edits of BMP image files. When the program\n\
runs it first prints out the width and the height of the input image\n\
within the BMP file. Once this is done a filter (or sequence of\n\
filters) are applied to the image. The resulting image is also stored\n\
using BMP format into an output file."

#define OPTIONS_STR "  -o FILE      Sets the output file (defaults to \"out.bmp\").\n\
  0.0-1.0      Apply this value to threshold filter.\n\
  -h           Displays this usage message."

/*functions*/

//usage
void usage(void){
  printf("\nUsage: bmpedit [OPTIONS...] [input.bmp]\n\n");
  printf("DESCRIPTION:\n%s\n\n\n", USAGE_STR);
  printf("OPTIONS:\n%s\n\n", OPTIONS_STR);
}

//error

//help

int main(int argc, char *argv[]){

  //parse args
  if (argc < 2){
    printf("%s: missing file operand\nTry '%s --help' for more information.\n",argv[0],argv[0]);
    exit(1);
  }
  int i;
  for (i=1;i<argc;i++){
      if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0){
        usage();
        exit(0);
      }else if (strcmp(argv[i],"-o") == 0){
        printf("output file set to: %s\n", argv[i+1]);
        printf("output is: %s\n", output);
        strcpy(output, argv[i+1]);
        printf("output is now: %s\n", output);
        i++;
      }else if (strcmp(argv[i],"-t") == 0){
        printf("threshold set to: %.1f\n", atof(argv[i+1]));
        printf("output is: %f\n", threshold);
        threshold = atof(argv[i+1]);
        printf("output is now: %f\n", threshold);
        i++;
      }else{
        usage();
        exit(1);
     }
  }
    return 0;
}
