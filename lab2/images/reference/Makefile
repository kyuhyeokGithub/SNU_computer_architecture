# This must point to img2raw.py from part 1 of the term project. 
# Adjust path if necessary.
RAW2PNG=../../../part-1/img2raw.py

RAW=$(wildcard *.raw)
PNG=$(RAW:%.raw=%.png)

all: $(PNG)

%.png: %.raw
	@$(RAW2PNG) $^

clean:
	@rm -f *.png
