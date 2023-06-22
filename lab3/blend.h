#ifndef __BLEND_H__
#define __BLEND_H__

#include "imlib.h"


/// @brief Alpha-blends two images of equal size using floating-point math. The image data must 
///        contain an alpha channel, i.e., img1/2.channels must be four.
///
/// @param img1 background image. Must have four channels.
/// @param img2 foreground image. Must have four channels and be of the same dimension as img1.
/// @param mode blending mode: 0: merge mode, 1: overlay mode.
/// @param alpha blending parameter (0.0 - 1.0).
/// @retval struct Image blended image
struct Image blend_float(struct Image img1, struct Image img2, int mode, double alpha);


/// @brief Alpha-blends two images of equal size using fixed-point 8-bit math. The image data must 
///        contain an alpha channel, i.e., img1/2.channels must be four.
///
/// @param img1 background image. Must have four channels.
/// @param img2 foreground image. Must have four channels and be of the same dimension as img1.
/// @param mode blending mode: 0: merge mode, 1: overlay mode.
/// @param alpha blending parameter (0 - 256).
/// @retval struct Image blended image
struct Image blend_int(struct Image img1, struct Image img2, int mode, int alpha);


/// @brief Alpha-blends two images of equal size using fixed-point 8-bit vector math. The image
///        data must contain an alpha channel, i.e., img1/2.channels must be four.
///
/// @param img1 background image. Must have four channels.
/// @param img2 foreground image. Must have four channels and be of the same dimension as img1.
/// @param mode blending mode. Must be 1 (overlay)
/// @param alpha blending parameter (0 - 256).
/// @retval struct Image blended image
struct Image blend_vector(struct Image img1, struct Image img2, int mode, int alpha);


/// @brief Alpha-blends two images of equal size using fixed-point 8-bit vector math. The image 
///        data must contain an alpha channel, i.e., img1/2.channels must be four.
///        The parameter @a blended holds a pointer to a struct holding the blended image. 
///        Height, width, and channel fields must be initialized. Data points to a memory area
///        large enough to hold the blended image data.
///
/// @param blended result image (pre-allocated).
/// @param img1 background image. Must have four channels.
/// @param img2 foreground image. Must have four channels and be of the same dimension as img1.
/// @param mode blending mode. Must be 1 (overlay)
/// @param alpha blending parameter (0 - 256).
/// @retval 0 on success
/// @retval -1 overlay != 1
/// @retval -2 img1 or img2 do not have four channels
/// @retval -3 output parameter @a blended NULL or blended->data NULL
int blend_asm(struct Image *blended, struct Image *img1, struct Image *img2, int mode, int alpha);


#endif // __BLEND_H__
