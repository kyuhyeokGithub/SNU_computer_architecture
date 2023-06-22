#!/usr/bin/env python
"""
4190.308 Computer Architecture                                                          Spring 2023

Image blending driver

This program loads two images in RAW format, blends them together using library functions, and 
stores the result back to disk in RAW format.

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
import blend_float
import blend_int



def main():
    """
    Loads two RAW images and calls either blend_float(...) or blend_int(...) to blend the two
    images. Stores the blended image back into a new RAW output image.
    """

    # Parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("--type", "-t", choices=['int', 'float'],
                        default='float', help="Computation type (default: float)")
    parser.add_argument("--mode", "-m", choices=['overlay', 'merge'], 
                        default='overlay', help="Blending mode (default: overlay)")
    parser.add_argument("--alpha", "-a", type=float, 
                        default=0.5, help="Alpha value (0.0 - 1.0, default: 0.5)")
    parser.add_argument("--raw", "-r", action="store_true", 
                        help="Save result as RAW file (default)")
    parser.add_argument("--png", "-p", action="store_true", help="Save result as PNG file")
    parser.add_argument("--output", "-o", default="", help="Force name of output image")
    parser.add_argument("image1", type=str, help="The background image")
    parser.add_argument("image2", type=str, help="The image to blend or merge")
    args = parser.parse_args()


    # Extract and check validity of arguments
    if args.alpha  < 0.0 or args.alpha > 1.0:
        print(f"Invalid alpha value {args.alpha}. Value must be between 0.0 and 1.0.")
        parser.print_help()
        exit(1)

    mode = 1 if args.mode == "overlay" else 0


    # Read images
    print(f"Loading RAW images {args.image1} and {args.image2}...")
    image1, height1, width1, channels1 = imlib.read_raw_image(args.image1)
    image2, height2, width2, channels2 = imlib.read_raw_image(args.image2)


    # Check that dimensions match and an alpha channel is present
    if height1 != height2 or width1 != width2:
        print(f"Image dimension mismatch\n"
              f"  {args.image1}: {height1}x{width1}\n"
              f"  {args.image2}: {height2}x{width2}")
        exit(1)
    if channels1 != 4 or channels2 != 4:
        print(f"Missing alpha channel\n"
              f"  {args.image1}: {'has' if channels1==4 else 'no'} alpha channel\n"
              f"  {args.image2}: {'has' if channels2==4 else 'no'} alpha channel")
        exit(1)
    print(f"  Image dimensions {height1} x {width1} x {channels1}\n")


    # Call blend function
    print(f"Blending images (mode: {args.mode}, type: {args.type}, alpha: {args.alpha})...")
    t_start = time.time()
    if args.type == "float":
        blended, bheight, bwidth, bchannels = blend_float.blend(image1, image2, height1, width1, 
                                                                channels1, mode, args.alpha)
    else:
        blended, bheight, bwidth, bchannels = blend_int.blend(image1, image2, height1, width1, 
                                                              channels1, mode, int(args.alpha*255))
    t_stop = time.time()
    print(f"  Elapsed time: {t_stop-t_start:.6f} seconds\n")


    # Construct output filename
    if args.output == "":
        dn = os.path.dirname(args.image1)
        fn1, ext1 = os.path.splitext(os.path.basename(args.image1))
        fn2, ext2 = os.path.splitext(os.path.basename(args.image2))
        bfn = f"{dn+'/' if dn!='' else ''}{fn1}_{fn2}_{args.mode}_{args.alpha}_{args.type}"
    else:
        bfn = args.output


    # Save blended RAW image
    print(f"Saving result ({bheight} x {bwidth} x {bchannels})...")
    if args.raw or not args.png:
        bfnraw = bfn + ".raw"
        print(f"  Saving as {bfnraw}")
        imlib.write_raw_image(bfnraw, blended, bheight, bwidth, bchannels)


    # Convert to PNG if desired
    if args.png:
        bfnpng= bfn + ".png"
        print(f"  Saving as {bfnpng}")
        imlib.write_image(bfnpng, blended, bheight, bwidth, bchannels)



if __name__ == '__main__':
    main()
