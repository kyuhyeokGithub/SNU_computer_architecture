# Read image
import imlib

image_list = ['301_512_SNU_512_merge_0.5_int.raw',
              '301_512_SNU_512_overlay_0.1_float.raw',
              '301_512_SNU_512_overlay_0.1_int.raw',
              '301_512_SNU_512_overlay_0.5_float.raw',
              '301_512_SNU_512_overlay_0.5_int.raw']

for image in image_list:
    print(f"Loading RAW image {image}...")
    o_image, height, width, channels = imlib.read_raw_image(image)
    r_image, r_height, r_width, r_channels = imlib.read_raw_image('../images/reference/'+image)
    a=1
    for h in range(height):
        for w in range(width):
            for c in range(channels):
                d = abs(o_image[h][w][c]-r_image[h][w][c])
                if d>2:
                    a=0
                    print(d)
    if a==1:
        print("SAME!!")
    else :
        print("DIFF!!")