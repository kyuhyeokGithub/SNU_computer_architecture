#ifndef __BLUR_H__
#define __BLUR_H__

#include "imlib.h"


/// @brief Blurs an image with a kernel using floating-point math and returns the blurred image.
///
/// @param image image to blur.
/// @param kernel_size size of kernel. Valid values: 3 (3x3), 5 (5x5), and 7 (7x7 kernel).
/// @retval struct Image blurred image
struct Image blur_float(struct Image image, int kernel_size);


/// @brief Blurs an image with a kernel using fixed-point math and returns the blurred image.
///
/// @param image image to blur.
/// @param kernel_size size of kernel. Valid values: 3 (3x3), 5 (5x5), and 7 (7x7 kernel).
/// @retval struct Image blurred image
struct Image blur_int(struct Image image, int kernel_size);


#endif // __BLUR_H__
