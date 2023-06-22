help:
	@echo "Copy the RAW files from the image directory of part 1 into this directory."

clean:
	@rm -f *_?x?_{float,int}* *_{overlay,merge}_*

do:
	$.../blur_driver --type int --kernel 3x3 --output cat_512_3x3_int cat_512.raw
	$.../blur_driver --type float --kernel 3x3 --output cat_512_3x3_float cat_512.raw
	$.../blur_driver --type int --kernel 5x5 --output cat_512_5x5_int cat_512.raw
	$.../blur_driver --type float --kernel 5x5 --output cat_512_5x5_float cat_512.raw
	