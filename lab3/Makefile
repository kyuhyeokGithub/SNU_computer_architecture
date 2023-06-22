PREFIX=/Users/kyuhyeokseo/CSAP/riscv/bin/riscv32-unknown-linux-gnu-
CC=$(PREFIX)gcc
OD=$(PREFIX)objdump

ARCH        = -march=rv32g -mabi=ilp32d -static
CFLAGS      = $(ARCH) -Og
AFLAGS      = $(ARCH)
LDFLAGS     = -T./link.ld -nostdlib -nostartfiles

SIMULATOR   = pyrisc-csap/snurisc5.py

all: compile

compile: blend_pyrisc

run: blend_pyrisc
	$(SIMULATOR) -dma 0x80100000 -dms 0x200000 -i 0x80180000 0x80000 images/301_256.raw -i 0x80200000 0x80000 images/SNU_256.raw -o 0x80280000 0x80000 images/blended_256.raw -l 5 -c 0 $^
	
convert:
	python ../part-1/raw2img.py images/blended_256.raw
	

asm:
	$(OD) -d ../part-3/blend_vint.o

%.o: %.c
	$(CC) $(CFLAGS) -c $^


%.o: %.s
	$(CC) $(AFLAGS) -c $^


blend_pyrisc: startup.o blend_pyrisc.o imlib_pyrisc.o blend_vasm.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


clean:
	@rm -f *.o blend_pyrisc images/blended.png


mrproper: clean
	@rm -rf images/blended.* pyrisc-csap/__pycache__/ 
