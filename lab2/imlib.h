//-------------------------------------------------------------------------------------------------
// 4190.308 Computer Architecture                                                       Spring 2023
//
/// @file
/// @brief Image Library
///        This library provides functions for reading and writing images in the CSAP RAW format.
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

#ifndef __IMLIB_H__
#define __IMLIB_H__

/// @brief Compute the offset of a specific pixel in an image. No range checks.
///
/// @param img Image struct
/// @param y   y coordinate of pixel
/// @param x   x coordinate of pixel
/// @param c   channel number
/// @retval int offset of pixel in image data
#define INDEX(img, y, x, c) ((y) * img.width + (x)) * img.channels + (c)

/// @brief Access (read/write) a specific pixel in an image. No range checks.
///        Similar to img[y][x][c] in Python.
///
/// Use as follows:
///   struct Image img;
///   ...
///   PIXEL(img, y, x, c) = value;      // store operation
///
///   value = PIXEL(img, 0, 0, 0);      // load operation
///
/// @param img Image struct
/// @param y   y coordinate of pixel
/// @param x   x coordinate of pixel
/// @param c   channel number
/// @retval none if used as a store operation
/// @retval char img[if used as a store operation
#define PIXEL(img, y, x, c) img.data[INDEX(img, y, x, c)]

typedef unsigned char uint8;

struct Image {
    uint8 *data;
    int height;
    int width;
    int channels;
};

/// @brief Reads a RAW image file and returns its pixel data, height, width, and number of 
///        channels in an Image struct. The function aborts in case of any error.
///
/// @param filename path to file
/// @retval struct Image image
struct Image read_raw_image(char *filename);


/// @brief Saves an image in RAW image file format. The function aborts in case of any error.
///
/// @param filename path to file
/// @param struct Image image
void write_raw_image(char *filename, struct Image img);

#endif // __IMLIB_H__
