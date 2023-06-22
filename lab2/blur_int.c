//-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Image blurring (int)
///        This module implements a function that blurs an image with a filter (integer version)
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


struct Image blur_int(struct Image image, int kernel_size)
{
  // TODO
  // Your work goes here
  struct Image bimage;
  int h, w, c, x, y;
  int temp;
  bimage.height = image.height + 1 - kernel_size;
  bimage.width = image.width + 1 - kernel_size;
  bimage.channels = image.channels;

  uint8 weight_Q = 255 / (kernel_size * kernel_size);
  uint8 weight_R = 255 - (kernel_size * kernel_size) * weight_Q;
  int center = kernel_size >> 1;

  bimage.data = (uint8*)malloc(sizeof(uint8*) * bimage.height * bimage.width * bimage.channels);
  for (h=0; h < bimage.height; h++){
    for (w=0; w < bimage.width; w++){
      for (c=0; c < bimage.channels; c++){
        temp = 0;
        for (x=0; x<kernel_size; x++){
          for (y=0; y<kernel_size; y++){
            temp += PIXEL(image, h+x, w+y, c) * weight_Q;
          }
        }
        temp += PIXEL(image, h+center, w+center, c) * weight_R;
        temp = temp >> 8 ;
        bimage.data[INDEX(bimage, h, w, c)] = temp;
      }
    }
  }
  // For now, we simply return the input image.
  // Fix/adjust once you have implemented your solution.
  return bimage;
}
