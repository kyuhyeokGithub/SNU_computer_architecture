"""
4190.308 Computer Architecture                                                          Spring 2023

Image blending (float)

This module implements a function that blends two images together (floating point version)

@author:
    Your Name <your@email.com>

@changes:
    2023/MM/DD Your Name Comment

"""



def blend(img1, img2, height, width, channels, overlay, alpha):
    """
    Alpha-blends two images of size heightxwidth. The image data must contain an alpha
    channel, i.e., 'channels' must be four

    Args:
        img1:         image 1 data (multi-level list), BGRA
        img2:         image 2 data (multi-level list), BGRA
        height:       image height
        width:        image width
        channels:     number of channels (must be 4)
        overlay:      if 1, overlay the second image over the first
                      if 0, merge the two images
        alpha:        alpha blending factor (0.0-1.0)

    Returns:
        A tuple containing the following elements:
        - blended:    blended image data (multi-level list), BGRA
        - bheight:    blended image height (=height)
        - bwidth:     blended image width (=width)
        - bchannels:  blended image channels (=channels)

    """

    if channels != 4:
        raise ValueError('Invalid number of channels')


    # TODO
    # Your work goes here

    # For now, we simply copy the input parameters into the output parameters.
    # Fix/adjust once you have implemented your solution.
    bheight = height
    bwidth = width
    bchannels = channels
    blended = []

    # For now, we simply copy the input parameters into the output parameters.
    # Fix/adjust once you have implemented your solution.
    if overlay == 0:
        beta = 1 - alpha
        for h in range(bheight):
            h_temp = []
            for w in range(bwidth):
                w_temp = []
                A1 = (img1[h][w][3]/255)
                A2 = (img2[h][w][3]/255)
                A = A1 * beta + A2 * alpha
                for c in range(3):
                    temp = (img1[h][w][c] * A1 * beta + img2[h][w][c] * A2 * alpha)/255
                    w_temp.append(int(temp*255))
                w_temp.append(int(A*255))
                h_temp.append(w_temp)
            blended.append(h_temp)

    else:
        for h in range(bheight):
            h_temp = []
            for w in range(bwidth):
                w_temp = []
                temp1 = img1[h][w][3]/255
                temp2 = img2[h][w][3]/255
                alpha_prime = temp2 * alpha
                beta_prime = 1 - alpha_prime
                for c in range(3):
                    b = img1[h][w][c]/255
                    a = img2[h][w][c]/255
                    temp = b * beta_prime + a * alpha_prime
                    w_temp.append(int(temp*255))
                w_temp.append(int(temp1*255))
                h_temp.append(w_temp)
            blended.append(h_temp)

    return blended, bheight, bwidth, bchannels

