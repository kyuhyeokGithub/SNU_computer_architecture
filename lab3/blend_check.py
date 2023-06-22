# Read image
import imlib

image_list = ['images/blended_256.raw']

for image in image_list:
    print(f"Loading RAW image {image}...")
    o_image, height, width, channels = imlib.read_raw_image(image)
    r_image, r_height, r_width, r_channels = imlib.read_raw_image(
        '../part-2/images/301_256_SNU_256_overlay_0.5_int.raw')
    
    l1 = [0]*256
    l2 = [0.00]*256

    for h in range(height):
        for w in range(width):
            for c in range(channels):
                d = abs(o_image[h][w][c]-r_image[h][w][c])
                l1[d]+=1
    
    l = height*width*channels

    for i in range(256):
        if l1[i]!=0:
            print(f'DIFF : {i} // CNT : {l1[i]} // PER : {l1[i]/l :.6f}')
