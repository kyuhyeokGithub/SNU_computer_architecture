//-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Image Library
///        This library provides functions for reading and writing images in various formats. 
///        In particular, the library can read and write images in a custom RAW format.
///
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2023/02/14 Bernhard Egger created
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imlib.h"


static uint8 MAGIC[4] = { 'C', 'S', 'A', 'P' };
static uint8 BGR_FORMAT[4] = { 'B', 'G', 'R', '-' };
static uint8 BGRA_FORMAT[4] = { 'B', 'G', 'R', 'A' };


void panic(char *message, int errorno)
{
  char *error = NULL;
  if (errorno !=0) error = strerror(errorno);

  fprintf(stderr, "%s%s%s\n",
          message ? message : "",
          message && error ? ": ":"",
          error ? error : "");

  exit(EXIT_FAILURE);
}


struct Image read_raw_image(char *filename)
{
  FILE *f;
  struct Image img = { NULL, -1, -1, -1 };

  // Open file
  if ((f = fopen(filename, "rb")) == NULL) panic("Cannot open file", errno);

  // Read magic number
  uint8 magic[4];
  if (fread(&magic, sizeof(magic), 1, f) < 1) panic("Cannot read magic", errno);
  if (*(int*)magic != *(int*)MAGIC) {
    char msg[64];
    snprintf(msg, sizeof(msg), "Invalid magic number: %08x (expected %08x).\n", 
             *(int*)magic, *(int*)MAGIC);
    panic(msg, 0);
  }

  // Read data format
  uint8 format[4];
  if (fread(&format, sizeof(format), 1, f) < 1) panic("Cannot read image format", errno);
  if (*(int*)format == *(int*)BGR_FORMAT) {
    img.channels = 3;
  } else if (*(int*)format == *(int*)BGRA_FORMAT) {
    img.channels = 4;
  } else {
    char msg[64];
    snprintf(msg, sizeof(msg), "Invalid data format: %08x.\n", *(int*)format);
    panic(msg, 0);
  }

  // Read height and width
  uint8 h[4], w[4];
  if (fread(h, sizeof(h), 1, f) < 1) panic("Cannot read image height", errno);
  if (fread(w, sizeof(w), 1, f) < 1) panic("Cannot read image width", errno);
  img.height = h[3] << 24 | h[2] << 16 | h[1] << 8 | h[0];
  img.width  = w[3] << 24 | w[2] << 16 | w[1] << 8 | w[0];

  // Allocate memory for image data
  int img_size = img.height * img.width * img.channels;
  if ((img.data = malloc(sizeof(uint8)*img_size)) == NULL) {
    panic("Failed to allocate memory for image", errno);
  }

  // Read pixel data
  if (fread(img.data, sizeof(uint8), img_size, f) < img_size) {
    panic("Cannot read image data", errno);
  }

  // Clean up and return
  fclose(f);

  return img;
}


void write_raw_image(char* filename, struct Image img)
{
  FILE *f;

  // Run a few checks
  if (img.data == NULL) panic("No image data.", 0);

  // Only 3 and 4 channels are supported
  if ((img.channels < 3) || (4 < img.channels)) panic("Invalid data format.", 0);

  // Write data to file
  if ((f = fopen(filename, "wb")) == NULL) panic("Cannot open file", errno);

  // Write magic number and data format (big endian)
  if (fwrite(MAGIC, sizeof(MAGIC), 1, f) < 1) panic("Cannot write magic to file", errno);
  uint8 *format = (img.channels == 3 ? BGR_FORMAT : BGRA_FORMAT);
  if (fwrite(format, sizeof(BGR_FORMAT), 1, f) < 1) panic("Cannot write format to file", errno);

  // Write height and width (little endian)
  uint8 h[4] = {img.height, img.height>>8, img.height>>16, img.height>>24};
  uint8 w[4] = {img.width,  img.width >>8, img.width >>16, img.width >>24};
  if (fwrite(h, sizeof(h), 1, f) < 1) panic("Cannot write image height", errno);
  if (fwrite(w, sizeof(w), 1, f) < 1) panic("Cannot write image width", errno);

  // Write pixel data
  int img_size = img.height * img.width * img.channels;
  if (fwrite(img.data, sizeof(uint8), img_size, f) < img_size) {
    panic("Cannot write image data", errno);
  }

  // Clean up and return
  fclose(f);
}


