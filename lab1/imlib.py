"""
4190.308 Computer Architecture                                                          Spring 2023

Image Library

This library provides functions for reading and writing images in various formats. In particular, 
the library can read and write images in a custom RAW format.

@author:
    Bernhard Egger <bernhard@csap.snu.ac.kr>

@changes:
    2023/02/14 Bernhard Egger created

@license:
    Copyright (c) 2023, Computer Systems and Platforms Laboratory, SNU
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, are permitted
    provided that the following conditions are met:

    - Redistributions of source code must retain the above copyright notice, this list of condi-
      tions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright notice, this list of condi-
      tions and the following disclaimer in the documentation and/or other materials provided with
      the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSE-
    QUENTIAL DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE,  DATA, OR PROFITS; OR BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
    DAMAGE.
"""


"""
CSAP RAW image format specification

The CSAP RAW image format contains raw uncompressed pixel data. The data is 


  MAGIC   4 characters 'C', 'S', 'A', 'P'.
  FORMAT  integer, big endian. Valid values: BGR_FORMAT (3 channels) or BGRA_FORMAT (4 channels).
  Height  integer, little endian. Specifices the height of the image.
  Width   integer, little endian. Specifices the width of the image.
  Data    image data stored as multi-dimensional array, row-major.
"""

import cv2
import numpy as np
import struct

# Constants
MAGIC = 0x43534150 # 'CSAP'
BGR_FORMAT  = 0x4247522d  # 'BGR-'
BGRA_FORMAT = 0x42475241  # 'BGRA'


def read_image(filename):
    """
    Reads a (compressed) image file and returns its pixel data, height, width, and # channels.

    Args:
        filename: A string specifying the filename of the image file.

    Returns:
        A tuple containing the following elements:
        - A multi-level list encoding the image data. Each inner list contains the pixel 
          values for one row of the image.
        - The height of the image (an integer).
        - The width of the image (an integer).
        - The number of channels in the image (either 3 or 4).
    """
    try:
        img = cv2.imread(filename, cv2.IMREAD_UNCHANGED)
        if img is None:
            raise Exception(f"Error: Could not read image file '{filename}'")
    except cv2.error as e:
        raise Exception(f"Error: {e}")

    height, width, num_channels = img.shape[:3]

    # Make sure we have three (BGR) or four (BGRA) channels
    if num_channels not in [3, 4]:
        raise ValueError('Invalid image. Only three or four channel images are supported.')

    # Convert to multi-level list
    image = [[list(img[h, w])[:num_channels] for w in range(width)] for h in range(height)]

    return image, height, width, num_channels



def read_raw_image(filename):
    """
    Reads a RAW image file and returns its pixel data, height, width, and number of channels.

    Args:
        filename: A string specifying the filename of the RAW image file.

    Returns:
        A tuple containing the following elements:
        - A multi-level list encoding the image data. Each inner list contains the pixel 
          values for one row of the image.
        - The height of the image (an integer).
        - The width of the image (an integer).
        - The number of channels in the image (either 3 or 4).
    """
    with open(filename, 'rb') as f:
        # Read the magic number
        magic = struct.unpack('>I', f.read(4))[0]
        if magic != MAGIC:
            #print(magic)
            raise ValueError('Invalid magic number')

        # Read the data format
        format_code = struct.unpack('>I', f.read(4))[0]
        if format_code == BGR_FORMAT:
            num_channels = 3
        elif format_code == BGRA_FORMAT:
            num_channels = 4
        else:
            raise ValueError('Invalid data format')

        # Read the height and width
        height = struct.unpack('<i', f.read(4))[0]
        width = struct.unpack('<i', f.read(4))[0]

        # Read the pixel data
        expected_bytes = height * width * num_channels
        data = f.read(expected_bytes)
        if len(data) != expected_bytes:
            raise ValueError(f"Tried to read {expected_bytes} bytes, but got only {len(data)}")

    # Convert the pixel data to a 3D array
    image = [[[0 for c in range(num_channels)] for w in range(width)] for h in range(height)]
    for h in range(height):
        for w in range(width):
            for c in range(num_channels):
                index = (h * width + w) * num_channels + c
                image[h][w][c] = data[index]

    return image, height, width, num_channels


def write_image(filename, image, height, width, num_channels):
    """
    Saves an image to disk.

    Args:
        filename:     A string specifying the filename of the RAW image file.
        image:        A multi-level list encoding the image data. Each inner list contains the 
                      pixel values for one row of the image.
        height:       The height of the image (an integer).
        width:        The width of the image (an integer).
        num_channels: The number of channels in the image (either 3 or 4).

    Raises:
        ValueError:   If the image data doesn't match the height, width, and number of channels
        IOError:      If there was a problem writing the output file
    """
    # Only 3 and 4 channels are supported
    if num_channels not in [3, 4]:
        raise ValueError('Invalid data format')

    # Make sure dimensions match
    if len(image) != height or len(image[0]) != width or len(image[0][0]) != num_channels:
        raise ValueError('Invalid data shape')

    # Flatten image data into a byte array
    data = bytearray()
    for h in range(height):
        for w in range(width):
            for c in range(num_channels):
                data.append(image[h][w][c])

    # Save the image using OpenCV's imwrite function
    try:
        img = np.frombuffer(data, dtype=np.uint8).reshape(height, width, -1)
        cv2.imwrite(filename, img)
    except Exception as e:
        raise IOError(f"Error writing image to file: {e}")



def write_raw_image(filename, image, height, width, num_channels):
    """
    Saves an image in RAW image file format.

    Args:
        filename:     A string specifying the filename of the RAW image file.
        image:        A multi-level list encoding the image data. Each inner list contains the 
                      pixel values for one row of the image.
        height:       The height of the image (an integer).
        width:        The width of the image (an integer).
        num_channels: The number of channels in the image (either 3 or 4).

    Raises:
        ValueError:   If the image data doesn't match the height, width, and number of channels
        IOError:      If there was a problem writing the output file
    """
    # Only 3 and 4 channels are supported
    if num_channels not in [3, 4]:
        raise ValueError('Invalid data format')

    # Make sure dimensions match
    if len(image) != height or len(image[0]) != width or len(image[0][0]) != num_channels:
        raise ValueError('Invalid data shape')

    # Flatten image data into a byte array
    data = bytearray()
    for h in range(height):
        for w in range(width):
            for c in range(num_channels):
                data.append(image[h][w][c])

    # Write data to file
    with open(filename, 'wb') as f:
        # Write the magic number
        f.write(struct.pack('>I', MAGIC))

        # Write the data format
        f.write(struct.pack('>I', (BGR_FORMAT if num_channels == 3 else BGRA_FORMAT)))

        # Write the height and width
        f.write(struct.pack('<i', height))
        f.write(struct.pack('<i', width))

        # Write the pixel data
        f.write(data)


