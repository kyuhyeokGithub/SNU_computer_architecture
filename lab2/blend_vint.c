//-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Image blending (int)
///        This module implements a function that blends two images together filter (integer
///        version)
///
/// @author Your Name <your@email.com>
///
/// @section changelog Change Log
/// 2023/MM/DD Your Name Comment
///
//-------------------------------------------------------------------------------------------------

//#include <stdio.h>
//#include <stdlib.h>
#include "blend.h"
#include "vector_math.h"


struct Image blend_int(struct Image img1, struct Image img2, int overlay, int alpha)
{
  if (img1.channels != 4) abort();


  // Initialize blended image
  struct Image blended = {
    .height   = img1.height,
    .width    = img1.width,
    .channels = img1.channels
  };
  blended.data = (uint8*)malloc(blended.height*blended.width*blended.channels*sizeof(uint8));
  

  int temp1, temp2;
  int h, w, c, x, y;
  int alpha_prime, beta_prime ;
  // TODO
  // Your work goes here

  if (overlay == 0)
  {
    int beta = 256 - alpha;
    for (h=0; h < blended.height; h++){
      for (w=0; w < blended.width; w++){
        temp1 = PIXEL(img1, h, w, 3) * beta + PIXEL(img2, h, w, 3) * alpha;
        temp1 = temp1 >> 8;
        blended.data[INDEX(blended, h, w, 3)] = (uint8) temp1;
        for (c=0; c < 3; c++){
          temp2 = PIXEL(img1, h, w, c) * PIXEL(img1, h, w, 3) * beta + PIXEL(img2, h, w, c) * PIXEL(img2, h, w, 3) * alpha;
          temp2 = temp2 >> 16;
          blended.data[INDEX(blended, h, w, c)] = (uint8)temp2;
        }
      }
    }
  } 
  else 
  {
    for (h=0; h < blended.height; h++){
      for (w=0; w < blended.width; w++){
        temp1 = PIXEL(img1, h, w, 3);
        alpha_prime = PIXEL(img2, h, w, 3) * alpha;
        alpha_prime = alpha_prime>>8;
        beta_prime = 256 - alpha_prime;
        for (c=0; c < 3; c++){
          temp2 = PIXEL(img1, h, w, c) * beta_prime + PIXEL(img2, h, w, c) * alpha_prime;
          temp2 = temp2 >> 8;
          blended.data[INDEX(blended, h, w, c)] = (uint8)temp2;
        }
        blended.data[INDEX(blended, h, w, 3)] = (uint8)temp1;
      }
    }
  }
  return blended;
}
