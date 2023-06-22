//-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Image blurring driver
///        This program loads a RAW image, blurs it using library functions, then stores the image 
///        back to disk in RAW format.
///
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2023/04/02 Bernhard Egger created
///
/// @section license_section License
/// Copyright (c) 2023, Computer Systems and Platforms Laboratory, SNU
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification, are permitted
/// provided that the following conditions are met:
///
/// - Redistributions of source code must retain the above copyright notice, this list of condi-
///   tions and the following disclaimer.
/// - Redistributions in binary form must reproduce the above copyright notice, this list of condi-
///   tions and the following disclaimer in the documentation and/or other materials provided with
///   the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
/// IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY
/// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
/// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSE-
/// QUENTIAL DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
/// LOSS OF USE,  DATA, OR PROFITS; OR BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON ANY THEORY OF
/// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
/// DAMAGE.
//-------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libgen.h>

#include "imlib.h"
#include "blur.h"

enum BlurType { btFloat, btInt };

struct Arguments {
  enum BlurType type;
  char *kernel;
  char *image;
  char *output;
};


/// @brief Print program syntax and exit. Does not return.
///
/// @param msg optional error/informational message.
void syntax(char *msg)
{
  if (msg) printf("%s\n\n", msg);

  printf("Usage: blur_driver [-h] [--type {int,float}] [--kernel {3x3,5x5,7x7}] "
                            "[--output OUTPUT] image\n"
         "\n"
         "Positional arguments:\n"
         "  image                       The image to blur\n"
         "\n"
         "Options:\n"
         "  -h/--help                   Show this help message and exit\n"
         "  -t/--type {int,float}       Computation type (default: float)\n"
         "  -k/--kernel {3x3,5x5,7x7}   Kernel size (default: 3x3)\n"
         "  -o/--output OUTPUT          Force name of output image\n");

  exit(EXIT_FAILURE);
}


/// @brief Parse arguments
///
/// @param argc number of command line arguments
/// @param argv command line arguments
/// @retval struct Argument parsed command line arguments
struct Arguments parse_arguments(int argc, char *argv[])
{
  struct Arguments args = { .type = btFloat, .kernel = "3x3", .image = NULL, .output = NULL };

  for (int i=1; i<argc; i++) {
    if (!strcmp("--type", argv[i]) || !strcmp("-t", argv[i])) {
      if (++i == argc) syntax("Missing argument after '--type'.");
      char *opt = argv[i];
      if (!strcmp("float", opt)) args.type = btFloat;
      else if (!strcmp("int", opt)) args.type = btInt;
      else syntax("Invalid option to '--type'");
    } else
    if (!strcmp("--kernel", argv[i]) || !strcmp("-k", argv[i])) {
      if (++i == argc) syntax("Missing argument after '--kernel'.");
      char *opt = argv[i];
      if (strcmp("3x3", opt) && strcmp("5x5", opt) && strcmp("7x7", opt)) {
        syntax("Invalid option to '--kernel'.");
      }
      args.kernel = opt;
    } else
    if (!strcmp("--output", argv[i]) || !strcmp("-o", argv[i])) {
      if (++i == argc) syntax("Missing argument after '--output'.");
      args.output = argv[i];
    } else
    if (!strcmp("--help", argv[i]) || !strcmp("-h", argv[i])) {
      syntax(NULL);
    } else {
      if (args.image == NULL) args.image = argv[i];
      else syntax("Too many images or unknown option.");
    }
  }

  if (args.image == NULL) syntax("No image file provided.");

  return args;
}

/// @brief Split a basename (filename.ext) into filename and extension at the last '.'.
///        Basename is assumed to not contain any path delimiters. Warning: modifies basename!
///
/// @param[in/out] basename basename (filename.ext)
/// @param[out] ext pointer to string to hold extension
void splitext(char *basename, char **ext)
{
  *ext = NULL;

  // assumption: basename is not NULL and does not contain a path
  char *p = basename;
  while (*p != '\0') {
    if (*p == '.') *ext = p+1;
    p++;
  }

  // split basename into filename and extension by removing the last '.'
  if (*ext) *(*ext-1) = '\0';
}


int main(int argc, char *argv[])
{
  struct Arguments args;
  int kernel_size;
  struct Image image, blurred;
  char *bfn;
  // Parse command line arguments
  args = parse_arguments(argc, argv);

  // Extract arguments
  kernel_size = args.kernel[0] - '0';

  // Read image
  printf("Loading RAW image %s...\n", args.image);
  image = read_raw_image(args.image);
  printf("  Image dimensions %d x %d x %d\n", image.height, image.width, image.channels);


  // Call blur function
  printf("Blurring image (kernel size: %s, type: %s)...\n", 
         args.kernel, args.type == btFloat ? "float" : "int" );

  clock_t t_start = clock();
  if (args.type == btFloat) {
    blurred = blur_float(image, kernel_size);
  } else {
    blurred = blur_int(image, kernel_size);
  }
  clock_t t_stop = clock();
  printf("  Elapsed time: %.6f seconds\n", ((float)(t_stop-t_start))/CLOCKS_PER_SEC);


  // Construct output filename
  if (args.output == NULL) {
    char *out, *dn, *bn, *ext;
    out = strdup(args.image); dn = strdup(dirname(out));  free(out);
    out = strdup(args.image); bn = strdup(basename(out)); free(out);
    splitext(bn, &ext);

    size_t bfn_size = strlen(dn)+strlen(bn)+32;
    bfn = calloc(bfn_size, sizeof(char));
    snprintf(bfn, bfn_size, "%s/%s_%s_%s.raw", 
             dn, bn, args.kernel, args.type == btFloat ? "float" : "int" );

    free(dn);
    free(bn);
  } else {
    size_t bfn_size = strlen(args.output)+8;
    bfn = calloc(bfn_size, sizeof(char));
    snprintf(bfn, bfn_size, "%s.raw", args.output);
  }


  // Save blurred RAW image
  printf("Saving result (%d x %d x %d)...\n", blurred.height, blurred.width, blurred.channels);
  printf("  Saving as %s\n", bfn);
  write_raw_image(bfn, blurred);


  // Cleanup
  free(bfn);
  free(image.data);
  free(blurred.data);


  // That's all, folks!
  return EXIT_SUCCESS;
}
