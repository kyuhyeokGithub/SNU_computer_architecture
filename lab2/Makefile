# PREFIX=/Users/kyuhyeokseo/CSAP/riscv/bin/riscv32-unknown-linux-gnu-
# CC=$(PREFIX)gcc

# C compile flags
# - performance
CFLAGS=-O2
# - debugging
#CFLAGS=-g



all: blend_driver blur_driver

%.o: %.c
	$(CC) $(CFLAGS) -c $^

blend_driver: blend_driver.o blend_float.o blend_int.o imlib.o
	$(CC) $(CFLAGS) -o $@ $^

blur_driver: blur_driver.o blur_float.o blur_int.o imlib.o
	$(CC) $(CFLAGS) -o $@ $^


clean:
	@rm -f *.o blend_driver blur_driver

