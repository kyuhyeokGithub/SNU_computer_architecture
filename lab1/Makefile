help:
	@echo "Copy the RAW files from the image directory of part 1 into this directory."

clean:
	@rm -f *_?x?_{float,int}* *_{overlay,merge}_*

do:
	$.../blur_driver --type int --kernel 3x3 --output cat_1024_3x3_int cat_1024.raw
	$.../blur_driver --type float --kernel 3x3 --output cat_1024_3x3_float cat_1024.raw
	$.../blur_driver --type int --kernel 5x5 --output cat_1024_5x5_int cat_1024.raw
	$.../blur_driver --type float --kernel 5x5 --output cat_1024_5x5_float cat_1024.raw
	$.../blur_driver --type int --kernel 7x7 --output cat_1024_7x7_int cat_1024.raw
	$.../blur_driver --type float --kernel 7x7 --output cat_1024_7x7_float cat_1024.raw
	$.../blend_driver --type int --mode merge --alpha 0.5 --output 301_256_SNU_256_merge_0.5_int 301_256.raw SNU_256.raw
	$.../blend_driver --type float --mode merge --alpha 0.5 --output 301_256_SNU_256_merge_0.5_float 301_256.raw SNU_256.raw
	$.../blend_driver --type int --mode merge --alpha 0.5 --output 301_1024_SNU_1024_merge_0.5_int 301_1024.raw SNU_1024.raw
	$.../blend_driver --type float --mode merge --alpha 0.5 --output 301_1024_SNU_1024_merge_0.5_float 301_1024.raw SNU_1024.raw
	$.../blend_driver --type int --mode overlay --alpha 0.5 --output 301_256_SNU_256_overlay_0.5_int 301_256.raw SNU_256.raw
	$.../blend_driver --type float --mode overlay --alpha 0.5 --output 301_256_SNU_256_overlay_0.5_float 301_256.raw SNU_256.raw
	$.../blend_driver --type int --mode overlay --alpha 0.5 --output 301_1024_SNU_1024_overlay_0.5_int 301_1024.raw SNU_1024.raw
	$.../blend_driver --type float --mode overlay --alpha 0.5 --output 301_1024_SNU_1024_overlay_0.5_float 301_1024.raw SNU_1024.raw
