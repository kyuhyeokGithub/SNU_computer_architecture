# Read image
import imlib

image_list = ['301_512_3x3_float.raw', '301_512_3x3_int.raw', 'cat_512_7x7_float.raw','cat_512_7x7_int.raw', 'SNU_512_5x5_float.raw', 'SNU_512_5x5_int.raw' ]

for image in image_list:
    print(f"Loading RAW image {image}...")
    o_image, height, width, channels = imlib.read_raw_image(image)
    r_image, r_height, r_width, r_channels = imlib.read_raw_image('../images/'+image)
    a=1
    for h in range(height):
        for w in range(width):
            for c in range(channels):
                d = abs(o_image[h][w][c]-r_image[h][w][c])
                if d!=0:
                    a=0
                if d>1:
                    print(d)
    if a==1:
        print("SAME!!")
    else :
        print("DIFF!!")