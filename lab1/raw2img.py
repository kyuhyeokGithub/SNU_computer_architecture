#!/usr/bin/env python
"""
4190.308 Computer Architecture                                                          Spring 2023

Image-to-RAW file format converter

This program loads a RAW image from a file provided as a command line argument and saves it
as a PNG image.

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


import sys
import os
import imlib



def main():
    # Make sure at least on image was provided as a command line argument
    if len(sys.argv) < 2:
        print("Usage: python raw2img.py <raw image file> {<raw image file>}")
        sys.exit(1)

    for i in range(1, len(sys.argv)):
        # Filename
        fn = sys.argv[i]

        try:
            print(f"Processing {fn}...")

            # Load raw image
            image, height, width, num_channels = imlib.read_raw_image(fn)

            # Save the image in png format
            fn_base, fn_ext  = os.path.splitext(fn)
            new_fn = fn_base + '.png'
            imlib.write_image(new_fn, image, height, width, num_channels)

            # Status message
            print(f"  Saved {height}x{width}x{num_channels} image to {new_fn}.")

        except Exception as e:
            print(f"Error processing '{fn}': ", e)



if __name__ == '__main__':
    main()
