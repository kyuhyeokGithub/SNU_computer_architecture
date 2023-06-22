RESOLUTION=256 512 1024
PNG=$(foreach res, $(RESOLUTION),$(wildcard *_$(res).png))
RAW=$(PNG:%.png=%.raw)

all: $(RAW)

%.raw: %.png
	@../img2raw.py $^

clean:
	@rm -f *.raw *int.png *float.png

do:
	$.python ../blur_driver.py --type int --kernel 3x3 --output cat512_3x3_int cat_512.raw
	$.python ../blur_driver.py --type float --kernel 3x3 --output cat_512_3x3_float cat_512.raw
	$.python ../blur_driver.py --type int --kernel 5x5 --output cat_512_5x5_int cat_512.raw
	$.python ../blur_driver.py --type float --kernel 5x5 --output cat_512_5x5_float cat_512.raw
	