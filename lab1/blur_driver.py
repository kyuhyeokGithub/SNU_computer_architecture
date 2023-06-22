#!/usr/bin/env python
"""
4190.308 Computer Architecture                                                          Spring 2023

Image blurring driver

This program loads a RAW image, blurs it using library functions, then stores the image back to
disk in RAW format.

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

import argparse
import os
import sys
import time
import imlib
import blur_float
import blur_int


def main():
    """
    Loads a RAW image and calls either blur_float(...) or blur_int(...) to blur the image before
    storing it back into a new RAW output image.
    """

    # Parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("--type", "-t", choices=['int', 'float'],
                        default='float', help="Computation type (default: float)")
    parser.add_argument("--kernel", "-k", choices=['3x3', '5x5', '7x7'],
                        default='3x3', help="Kernel size")
    parser.add_argument("--raw", "-r", action="store_true", 
                        help="Save result as RAW file (default)")
    parser.add_argument("--png", "-p", action="store_true", help="Save result as PNG file")
    parser.add_argument("--output", "-o", default="", help="Force name of output image")
    parser.add_argument("image", type=str, help="The image to blur")
    args = parser.parse_args()

    # Extract arguments
    kernel_size = int(args.kernel.split("x")[0])

    # Read image
    print(f"Loading RAW image {args.image}...")
    image, height, width, channels = imlib.read_raw_image(args.image)
    print(f"  Image dimensions {height} x {width} x {channels}\n")

    # Call blur function
    print(f"Blurring image (kernel size: {args.kernel}, type: {args.type})...")
    t_start = time.time()
    if args.type == "float":
        blurred, bheight, bwidth, bchannels = blur_float.blur(image, height, width, channels,
                                                              kernel_size)
    else:
        # image[h][w][c]
        blurred, bheight, bwidth, bchannels = blur_int.blur(image, height, width, channels,
                                                            kernel_size)
    t_stop = time.time()
    print(f"  Elapsed time: {t_stop-t_start:.6f} seconds\n")


    # Construct output filename
    if args.output == "":
        dn = os.path.dirname(args.image)
        fn, ext = os.path.splitext(os.path.basename(args.image))
        bfn = f"{dn+'/' if dn!='' else ''}{fn}_{args.kernel}_{args.type}"
    else:
        bfn = args.output


    # Save blurred RAW image
    print(f"Saving result ({bheight} x {bwidth} x {bchannels})...")
    if args.raw or not args.png:
        bfnraw = bfn + ".raw"
        print(f"  Saving as {bfnraw}")
        imlib.write_raw_image(bfnraw, blurred, bheight, bwidth, bchannels)


    # Convert to PNG if desired
    if args.png:
        bfnpng= bfn + ".png"
        print(f"  Saving as {bfnpng}")
        imlib.write_image(bfnpng, blurred, bheight, bwidth, bchannels)



if __name__ == '__main__':
    main()
