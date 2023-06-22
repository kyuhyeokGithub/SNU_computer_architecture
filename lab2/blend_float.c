//-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Image blending (float)
///        This module implements a function that blends two images together filter (floating-point
///        version). Note that CPython uses the 'double' data type for floating point numbers, 
///        so one may observe small differences in the output if the 'float' data type is used.
///
/// @author Your Name <your@email.com>
///
/// @section changelog Change Log
/// 2023/MM/DD Your Name Comment
///
//-------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "blend.h"


struct Image blend_float(struct Image img1, struct Image img2, int overlay, double alpha)
{
  if (img1.channels != 4) abort();


  // Initialize blended image
  struct Image blended = {
    .height   = img1.height,
    .width    = img1.width,
    .channels = img1.channels
  };
  blended.data = malloc(blended.height*blended.width*blended.channels*sizeof(uint8));
  if (blended.data == NULL) abort();

  float temp1, temp2;
  float a1, a2;
  int h, w, c, x, y;
  
  // TODO
  // Your work goes here

  if (overlay == 0)
  {
    float beta = (float) 1.0 - (float) alpha;
    for (h=0; h < blended.height; h++){
      for (w=0; w < blended.width; w++){
        a1 = (float)PIXEL(img1, h, w, 3)/(float)255.0;
        a2 = (float)PIXEL(img2, h, w, 3)/(float)255.0;
        temp1 = a1 * beta + a2 * (float) alpha;
        temp1 = (float)255.0*temp1;
        blended.data[INDEX(blended, h, w, 3)] = (uint8) temp1;
        for (c=0; c < 3; c++){
          temp2 = ((float)PIXEL(img1, h, w, c)/(float)255.0) * a1 * beta + ((float)PIXEL(img2, h, w, c)/(float)255.0) * a2 * (float) alpha;
          temp2 = temp2 * 255.0;
          blended.data[INDEX(blended, h, w, c)] = (uint8) temp2;
        }
      }
    }
  } 
  else 
  {
    float alpha_prime, beta_prime ;
    for (h=0; h < blended.height; h++){
      for (w=0; w < blended.width; w++){
        a1 = (float)PIXEL(img1, h, w, 3)/(float)255.0;
        a2 = (float)PIXEL(img2, h, w, 3)/(float)255.0;
        alpha_prime = a2 * (float) alpha;
        beta_prime = (float) 1.0 - alpha_prime;
        
        for (c=0; c < 3; c++){
          temp2 = ((float)PIXEL(img1, h, w, c)/(float)255.0)*beta_prime + ((float)PIXEL(img2, h, w, c)/(float)255.0)*alpha_prime;
          temp2 = temp2 * 255.0;
          blended.data[INDEX(blended, h, w, c)] = (uint8)temp2;
        }
        a1 = a1 * (float)255.0;
        blended.data[INDEX(blended, h, w, 3)] = (uint8)a1;
      }
    }
  }

  return blended;
}
