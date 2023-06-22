"""
4190.308 Computer Architecture                                                          Spring 2023

Image blending (int)

This module implements a function that blends two images together (integer version)

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
        alpha:        alpha blending factor (0-255)

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

    bheight   = height
    bwidth    = width
    bchannels = channels
    blended   = []

    # For now, we simply copy the input parameters into the output parameters.
    # Fix/adjust once you have implemented your solution.
    if overlay == 0 :
        beta = 256 - alpha
        for h in range(bheight):
            h_temp = []
            for w in range(bwidth):
                w_temp = []
                temp1 = img1[h][w][3]* beta + img2[h][w][3] *alpha
                A = temp1>>8
                for c in range(3):
                    temp2 = img1[h][w][c]*img1[h][w][3]*beta\
                                       +img2[h][w][c]*img2[h][w][3]*alpha
                    temp = temp2>>16

                    w_temp.append(temp)
                w_temp.append(A)
                h_temp.append(w_temp)
            blended.append(h_temp)

    else :
        for h in range(bheight):
            h_temp = []
            for w in range(bwidth):
                w_temp = []
                temp1 = img1[h][w][3]
                alpha_prime = img2[h][w][3]*alpha
                alpha_prime = alpha_prime>>8
                beta_prime = 256 - alpha_prime
                for c in range(3):
                    temp2 = img1[h][w][c]*beta_prime + img2[h][w][c]*alpha_prime
                    temp2 = temp2>>8
                    w_temp.append(temp2)
                w_temp.append(temp1)
                h_temp.append(w_temp)
            blended.append(h_temp)

    return blended, bheight, bwidth, bchannels



