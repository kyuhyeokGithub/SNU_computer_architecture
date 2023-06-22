//-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Image blurring (float)
///        This module implements a function that blurs an image with a filter (floating-point 
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
#include "blur.h"


struct Image blur_float(struct Image image, int kernel_size)
{
  // TODO
  // Your work goes here
  struct Image bimage;
  int h, w, c, x, y;
  float temp;
  bimage.height = image.height + 1 - kernel_size;
  bimage.width = image.width + 1 - kernel_size;
  bimage.channels = image.channels;

  float weight = (float)1.0/(float)(kernel_size*kernel_size);

  bimage.data = (uint8*)malloc(sizeof(uint8*) * bimage.height * bimage.width * bimage.channels);
  for (h=0; h < bimage.height; h++){
    for (w=0; w < bimage.width; w++){
      for (c=0; c < bimage.channels; c++){
        temp = 0;
        for (x=0; x<kernel_size; x++){
          for (y=0; y<kernel_size; y++){
            temp += (float) PIXEL(image, h+x, w+y, c) / (float) 255.0;
          }
        }
        temp = temp * weight;
        temp = temp * 255.0;
        bimage.data[INDEX(bimage, h, w, c)] = (uint8)(temp);
      }
    }
  }
  // For now, we simply return the input image.
  // Fix/adjust once you have implemented your solution.
  return bimage;
}
