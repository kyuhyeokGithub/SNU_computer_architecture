//-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Image blending (int)
///        This module implements a function that blends two images together filter (integer
///        vector + pointer version)
///
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2023/05/02 Bernhard Egger created
///
/// @section license_section License
/// Copyright (c) 2023, Computer Systems and Platforms Laboratory, SNU
//-------------------------------------------------------------------------------------------------

#include "imlib.h"
#include "blend.h"

// Locations of RAW image data in simulator memory
#define IMG1_RAW (void*)0x80180000
#define IMG2_RAW (void*)0x80200000
#define OUT_RAW  (void*)0x80280000

struct Image img1;
struct Image img2;
struct Image blended;

#define ALPHA 0x80      // <<< alpha blending factor. Valid range: 0x00 - 0xff

int main(int argc, char *argv[])
{
  int res;

  // Initialize the two images
  if ((res = get_raw_image(&img1, IMG1_RAW)) < 0) return 0x10 - res;
  if ((res = get_raw_image(&img2, IMG2_RAW)) < 0) return 0x20 - res;

  // The output image struct is not initialized except for its data pointer which is set
  // to the data area in the RAW image data of OUT_RAW
  blended.data = OUT_RAW + 16; // data begins after header (16 bytes)

  // And...action!
  if ((res = blend_asm(&blended, &img1, &img2, 1, 0x80)) < 0) return 0x30 - res;

  // Write result to RAW image data
  if ((res = set_raw_image(&blended, OUT_RAW)) < 0) return 0x40 - res;

  return 0;
}

