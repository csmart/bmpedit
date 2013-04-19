About
-----
bmpedit is licensed under GPLv3 or later, see COPYING file for details.

This small program prints out information about a BMP image and then
manipulates the image in a few simple ways.

In its simplest form, simply run the program and pass in a single option
being the path of a BMP image. The program will then print out relevant
information about the file such as:
 * Image width
 * Image height
 * Image bit depth
 * Compression type
 * Various header details

You can also perform one of two functions on the file, currently either:
 * black and white threshold filter
 * brightness filter

To perform one of those (conflicting) functions, pass the respective
option (-t or -b) and a corresponding value betwen 0.0 and 1.0.

The program will create an output BMP image called "output.bmp" in the
current directory, or you can override this with the -o option.

See the help for more details (-h option).

How to compile
--------------
To compile and run this program you need gcc installed:

On Fedora, run:
  # sudo yum install gcc

On Debian, run:
  # sudo apt-get install gcc

Then from inside the source directory, run:
  # gcc bmpedit.c -o bmpedit

How to run
----------
To view the help, simply run:
  # ./bmpedit -h

To test the program, simply run with path of bmp image:
  # ./bmpedit samples/cup.bmp

To test the black & white filter, simply pass the option with a value, e.g:
  # ./bmpedit -t 0.75 samples/cup.bmp

Similarly test the brightness filter, simply pass the option with a value, e.g:
  # ./bmpedit -b 0.25 samples/cup.bmp

These commands will create a file called "output.bmp" in the current directory,
you can override this with the -o flag and pass a path/filename.

Limitations
-----------
This program should print information on most BMP images, however the filters
are only supported on BMP images that meet the following criteria:
 * 24bit
 * Uncompressed
 * BM header format
 * BITMAPINFOHEADER DIB header format

Obvious improvements to this program would be in all of these areas.

In terms of the code, it performs a manual bitshift to handle little endian
format but should probably use endian.h library to handle this no-matter the
platform.

Testing
-------
This code will certainly have bugs. I have only tested this on a 64 bit Intel 
Linux system with the GCC compiler.

I have however, tested it with a range of BMP (and other format) files to 
ensure that the program exits nicely when it comes up against an unsupported 
format (such as compressed, non-bmp, non-24bit, etc).

I also tested BMPs which had colour information written to the header (this
moves the pixel array offset).

I have tested the sanity of options, including the input names, thresholds 
and added code to error on invalid values such as negative integers.

