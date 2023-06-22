//-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Image blending driver
///        This program loads two RAW images, blends them together using library functions, and
///        stores the result back to disk in RAW format.
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
#include "blend.h"

enum BlurType { btFloat, btInt };
enum BlurMode { bmOverlay, bmMerge };

struct Arguments {
  enum BlurType type;
  enum BlurMode mode;
  double alpha;
  char *image1;
  char *image2;
  char *output;
};


/// @brief Print program syntax and exit. Does not return.
///
/// @param msg optional error/informational message.
void syntax(char *msg)
{
  if (msg) printf("%s\n\n", msg);

  printf("Usage: blend_driver [-h] [--type {int,float}] [--mode {overlay,merge}] [--alpha ALPHA] "
                             "[--output OUTPUT] image1 image2\n"
         "\n"
         "Positional arguments:\n"
         "  image1                      The background image\n"
         "  image2                      The image to blend or merge\n"
         "\n"
         "Options:\n"
         "  -h/--help                   Show this help message and exit\n"
         "  -t/--type {int,float}       Computation type (default: float)\n"
         "  -m/--mode {overlay,merge}   Blending mode (default: overlay)\n"
         "  -a/--alpha ALPHA            Alpha value (0.0 - 1.0, default: 0.5)\n"
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
  struct Arguments args = { 
    .type = btFloat, .mode = bmOverlay, .alpha = 0.5,
    .image1 = NULL, .image2 = NULL, .output = NULL
  };

  for (int i=1; i<argc; i++) {
    if (!strcmp("--type", argv[i]) || !strcmp("-t", argv[i])) {
      if (++i == argc) syntax("Missing argument after '--type'.");
      char *opt = argv[i];
      if (!strcmp("float", opt)) args.type = btFloat;
      else if (!strcmp("int", opt)) args.type = btInt;
      else syntax("Invalid option to '--type'");
    } else
    if (!strcmp("--mode", argv[i]) || !strcmp("-m", argv[i])) {
      if (++i == argc) syntax("Missing argument after '--mode'.");
      char *opt = argv[i];
      if (!strcmp("overlay", opt)) args.mode = bmOverlay;
      else if (!strcmp("merge", opt)) args.mode = bmMerge;
      else syntax("Invalid option to '--mode'");
    } else
    if (!strcmp("--alpha", argv[i]) || !strcmp("-a", argv[i])) {
      if (++i == argc) syntax("Missing argument after '--alpha'.");
      char *endptr;
      args.alpha = strtod(argv[i], &endptr);
      if (*endptr != '\0') syntax("Invalid float after '--alpha'.");
    } else
    if (!strcmp("--output", argv[i]) || !strcmp("-o", argv[i])) {
      if (++i == argc) syntax("Missing argument after '--output'.");
      args.output = argv[i];
    } else
    if (!strcmp("--help", argv[i]) || !strcmp("-h", argv[i])) {
      syntax(NULL);
    } else {
      if (args.image1 == NULL) args.image1 = argv[i];
      else if (args.image2 == NULL) args.image2 = argv[i];
      else syntax("Too many images or unknown option.");
    }
  }

  if (args.image2 == NULL) syntax("Please provide two images files.");

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
  int mode;
  struct Image image1, image2, blended;
  char *bfn;

  // Parse command line arguments
  args = parse_arguments(argc, argv);

  // Extract and check validity of arguments
  if ((args.alpha < 0.0) || (args.alpha > 1.0)) {
    syntax("Invalid alpha value. Value must be between 0.0 and 1.0.");
  }

  mode = args.mode == bmOverlay ? 1 : 0;


  // Read images
  printf("Loading RAW images %s and %s...\n", args.image1, args.image2);
  image1 = read_raw_image(args.image1);
  image2 = read_raw_image(args.image2);


  // Check that dimensions match and an alpha channel is present
  if ((image1.height != image2.height) || (image1.width != image2.width)) {
    printf("Image dimension mismatch\n"
           "  %s: %dx%d\n"
           "  %s: %dx%d\n",
           args.image1, image1.height, image1.width,
           args.image2, image2.height, image2.width);
    exit(EXIT_FAILURE);
  }
  if ((image1.channels != 4) || (image2.channels != 4)) {
    printf("Missing alpha channel\n"
           "  %s: %s alpha channel\n"
           "  %s: %s alpha channel\n",
           args.image1, image1.channels == 4 ? "has" : "no",
           args.image2, image2.channels == 4 ? "has" : "no");
    exit(EXIT_FAILURE);
  }
  printf("  Image dimensions %d x %d x %d\n", image1.height, image1.width, image1.channels);


  // Call blend function
  printf("Blending images (mode: %s, type: %s, alpha: %g)...\n", 
         args.mode == bmOverlay ? "overlay" : "merge", args.type == btFloat ? "float" : "int",
         args.alpha);

  clock_t t_start = clock();
  if (args.type == btFloat) {
    blended = blend_float(image1, image2, mode, args.alpha);
  } else {
    blended = blend_int(image1, image2, mode, (int)(args.alpha*255));
  }
  clock_t t_stop = clock();
  printf("  Elapsed time: %.6f seconds\n", ((float)(t_stop-t_start))/CLOCKS_PER_SEC);


  // Construct output filename
  if (args.output == NULL) {
    char *out, *dn1, *dn2, *bn1, *bn2, *ext1, *ext2;
    out = strdup(args.image1); dn1 = strdup(dirname(out));  free(out);
    out = strdup(args.image1); bn1 = strdup(basename(out)); free(out);
    splitext(bn1, &ext1);

    out = strdup(args.image2); dn2 = strdup(dirname(out));  free(out);
    out = strdup(args.image2); bn2 = strdup(basename(out)); free(out);
    splitext(bn2, &ext2);

    size_t bfn_size = strlen(dn1)+strlen(bn1)+strlen(bn2)+32;
    bfn = calloc(bfn_size, sizeof(char));
    snprintf(bfn, bfn_size, "%s/%s_%s_%s_%.2g_%s.raw", 
             dn1, bn1, bn2, args.mode == bmOverlay ? "overlay" : "merge", 
             args.alpha, args.type == btFloat ? "float" : "int" );

    free(dn1); free(dn2);
    free(bn1); free(bn2);
  } else {
    size_t bfn_size = strlen(args.output)+8;
    bfn = calloc(bfn_size, sizeof(char));
    snprintf(bfn, bfn_size, "%s.raw", args.output);
  }


  // Save blurred RAW image
  printf("Saving result (%d x %d x %d)...\n", blended.height, blended.width, blended.channels);
  printf("  Saving as %s\n", bfn);
  write_raw_image(bfn, blended);


  // Cleanup
  free(bfn);
  free(image1.data);
  free(image2.data);
  free(blended.data);


  // That's all, folks!
  return EXIT_SUCCESS;
}
