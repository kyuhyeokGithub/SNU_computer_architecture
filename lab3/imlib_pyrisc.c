//-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Image library
///        This module provides functions to convert RAW image data into an Image struct and vice
///        versa. Primarily intended for use with pre-loaded image data (RISC-V simulator).
///
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2023/05/02 Bernhard Egger created
///
/// @section license_section License
/// Copyright (c) 2023, Computer Systems and Platforms Laboratory, SNU
//-------------------------------------------------------------------------------------------------

#include "imlib.h"

#ifndef NULL
#define NULL     (void*)0
#endif

static uint32 MAGIC       = 0x50415343;
static uint32 BGR_FORMAT  = 0x2d524742;
static uint32 BGRA_FORMAT = 0x41524742;

int get_raw_image(struct Image *img, void *data)
{
  if (img == NULL) return -1;

  int *hdr = (int*)data;

  if (hdr[0] != MAGIC) return -2;
  if (hdr[1] == BGR_FORMAT) {
    img->channels = 3;
  } else if (hdr[1] == BGRA_FORMAT) {
    img->channels = 4;
  } else {
    return -3;
  }

  img->height = hdr[2];
  img->width = hdr[3];
  img->data = (uint8*)&hdr[4];

  return 0;
}

int set_raw_image(struct Image *img, void *data)
{
  if (img == NULL) return -1;
  if (img->data == NULL) return -2;

  int *hdr = (int*)data;

  hdr[0] = MAGIC;
  if (img->channels == 3) {
    hdr[1] = BGR_FORMAT;
  } else if (img->channels == 4) {
    hdr[1] = BGRA_FORMAT;
  } else {
    return -3;
  }

  hdr[2] = img->height;
  hdr[3] = img->width;

  if ((void*)&hdr[4] != img->data) return -4;

  return 0;

}
