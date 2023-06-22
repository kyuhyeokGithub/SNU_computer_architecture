"""
4190.308 Computer Architecture                                                          Spring 2023

Image blurring (int)

This module implements a function that blurs an image with a filter (integer version)

@author:
    Your Name <your@email.com>

@changes:
    2023/MM/DD Your Name Comment

"""


def blur(image, height, width, channels, kernel_size=5):
    """
    Blurs an image with a kernel and returns the blurred image.

    Args:
        image:        image data (multi-level list)
        height:       image height
        width:        image width
        channels:     number of channels (BGR or BGRA)
        kernel_size:  size of blurring kernel

    Returns:
        A tuple containing the following elements:
        - blurred:    blurred image data
        - bheight:    blurred image height
        - bwidth:     blurred image width
        - bchannels:  blurred image channels

    """

    # TODO
    # Your work goes here

    # For now, we simply copy the input parameters into the output parameters.
    # Fix/adjust once you have implemented your solution.

    bheight = height + 1 - kernel_size
    bwidth = width + 1 - kernel_size
    bchannels = channels
    blurred = []

    weight_Q = 255 // (kernel_size ** 2)
    weight_R = 255 - (kernel_size ** 2 - 1) * weight_Q
    center = kernel_size >> 1

    for h in range(bheight):
        h_temp = []
        for w in range(bwidth):
            w_temp = []
            for c in range(bchannels):
                temp = 0
                for x in range(kernel_size):
                    for y in range(kernel_size):
                        if x!=center or y!=center:
                            temp += image[h+x][w+y][c]
                        else :
                            temp1 = image[h+center][w+center][c]
                temp = temp * weight_Q
                temp += temp1 * weight_R
                temp = temp>>8
                w_temp.append(temp)
            h_temp.append(w_temp)
        blurred.append(h_temp)

    return blurred, bheight, bwidth, bchannels



