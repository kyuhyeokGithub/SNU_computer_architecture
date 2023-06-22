#!/usr/bin/env python3

#==========================================================================
#
#   The PyRISC Project
#
#   SNURISC5: A 5-stage Pipelined RISC-V ISA Simulator (IF-ID-EX-MM-WB)
#
#   The main program for the 5-stage pipelined RISC-V ISA simulator.
#
#   Jin-Soo Kim
#   Systems Software and Architecture Laboratory
#   Seoul National University
#   http://csl.snu.ac.kr
#
#==========================================================================

import argparse
import sys

from consts import *
from isa import *
from components import *
from program import *
from datapath import *
from control import *


#--------------------------------------------------------------------------
#   Configurations
#--------------------------------------------------------------------------

# Memory configurations
#   IMEM: 0x80000000 - 0x8000ffff (64KB)
#   DMEM: 0x80010000 - 0x8001ffff (64KB)

IMEM_START  = WORD(0x80000000)      # IMEM: 0x80000000 - 0x8000ffff (64KB)
IMEM_SIZE   = WORD(64 * 1024)
DMEM_START  = WORD(0x80010000)      # DMEM: 0x80010000 - 0x8001ffff (64KB)
DMEM_SIZE   = WORD(64 * 1024)


#--------------------------------------------------------------------------
#   SNURISC5: Target machine to simulate
#--------------------------------------------------------------------------

class SNURISC5(object):

    def __init__(self, imem_start=IMEM_START, imem_size=IMEM_SIZE,
                       dmem_start=DMEM_START, dmem_size=DMEM_SIZE):

        stages = [ IF(), ID(), EX(), MM(), WB() ]
        self.ctl = Control()
        Pipe.set_stages(self, stages, self.ctl)

        self.rf = RegisterFile()
        self.alu = ALU()
        self.imem = Memory(imem_start, imem_size, WORD_SIZE)
        self.dmem = Memory(dmem_start, dmem_size, WORD_SIZE)
        self.adder_brtarget = Adder()
        self.adder_pcplus4 = Adder()

        print(f"SnuRISC-V\n"
              f"  architecture:          {BITWIDTH} bit\n"
              f"  pipeline stages:       {len(stages)}\n"
              f"\n"
              f"  instruction memory:    {imem_start:08x} - {imem_start+imem_size-1:08x}"
              f" ({imem_size} bytes)\n"
              f"  data memory:           {dmem_start:08x} - {dmem_start+dmem_size-1:08x}"
              f" ({dmem_size} bytes)\n")

    def run(self, entry_point):
        Pipe.run(entry_point)


#--------------------------------------------------------------------------
#   Utility functions for command line parsing
#--------------------------------------------------------------------------

def parse_args(args):

    # Parse command line
    parser = argparse.ArgumentParser(usage='%(prog)s --help for more information', 
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("--log", "-l", type=int, default=Log.level, help='''\
sets the desired log level (default: %(default)s)
 0: logging disabled
 1: dumps registers at the end of the execution
 2: dumps registers and memory at the end of the execution
 3: 2 + shows instructions retired from the WB stage
 4: 3 + shows all the instructions in the pipeline
 5: 4 + shows full information for each instruction
 6: 5 + dumps registers for each cycle
 7: 6 + dumps data memory for each cycle''')
    parser.add_argument("--cycle", "-c", type=int, default=0,
        help="shows logs after cycle m (default: %(default)s, only effective for log level 3 or higher)")
    parser.add_argument("--input", "-i", action="append", 
        nargs=3, metavar=("address", "maxsize", "filename"),
        help="Load file to the indicated address before execution. Aborts of the file is larger than maxsize.")
    parser.add_argument("--output", "-o", action="append", 
        nargs=3, metavar=("address", "size", "filename"),
        help="Save the memory from address to address+size-1 to a file.")
    parser.add_argument("--imem-addr", "-ima", type=lambda x: int(x, 0), default=IMEM_START,
        help="Set start address of instruction memory. Default: %(default)08x.")
    parser.add_argument("--imem-size", "-ims", type=lambda x: int(x, 0), default=IMEM_SIZE,
        help="Set size of instruction memory. Default: %(default)08x.")
    parser.add_argument("--dmem-addr", "-dma", type=lambda x: int(x, 0), default=DMEM_START,
        help="Set start address of data memory. Default: %(default)08x.")
    parser.add_argument("--dmem-size", "-dms", type=lambda x: int(x, 0), default=DMEM_SIZE,
        help="Set size of data memory. Default: %(default)08x.")
    parser.add_argument("filename", type=str, help="RISC-V executable file name")

    args = parser.parse_args()

    # Argument checks
    if args.log < 0 or args.log > Log.MAX_LOG_LEVEL:
        print("Invalid log level {args.log}. Valid range: 0 .. {Log.MAX_LOG_LEVEL}")
        parser.print_help()
        exit(1)

    if ((args.imem_addr < args.dmem_addr and args.imem_addr + args.imem_size > args.dmem_addr) or
        (args.dmem_addr < args.imem_addr and args.dmem_addr + args.dmem_size > args.imem_addr)):
        print("Instruction and data memory must not overlap.")
        print(f"  Instruction memory: {args.imem_addr:08x} - {args.imem_addr+args.imem_size:08x}")
        print(f"         Data memory: {args.dmem_addr:08x} - {args.dmem_addr+args.dmem_size:08x}")
        exit(1)

    # Set arguments
    Log.level = args.log
    Log.start_cycle = args.cycle

    return args


def load_file(cpu, adr_str, maxsize_str, filename):
    try:
        address = int(adr_str, 0)
        maxsize = int(maxsize_str, 0)

        with open(filename, 'rb') as f:
            data = bytearray(f.read())

            if len(data) > maxsize:
                raise ValueError(f"Data of {filename} larger than maximum allowed size ({maxsize})")

            cpu.dmem.copy_to(address, data)

    except ValueError:
        print(f"Invalid data types in input parameter {adr_str} {maxsize_str} {filename}. "
               "Expected types are int int string.")
        raise
    except Exception as e:
        print(f"Error loading data into memory: {e.args[0]}")
        raise


def save_file(cpu, adr_str, size_str, filename):
    try:
        address = int(adr_str, 0)
        size = int(size_str, 0)

        data = cpu.dmem.copy_from(address, size)

        with open(filename, 'wb') as f:
            f.write(data)

    except ValueError:
        print(f"Invalid data types in output parameter {adr_str} {size_str} {filename}. "
               "Expected types are int int string.")
        raise
    except Exception as e:
        print(f"Error saving data to file: {e.args[0]}")
        raise



#--------------------------------------------------------------------------
#   Simulator main
#--------------------------------------------------------------------------

def main():

    # Parse arguments
    args = parse_args(sys.argv[1:])

    # Instantiate CPU instance with H/W components
    cpu = SNURISC5(args.imem_addr, args.imem_size, args.dmem_addr, args.dmem_size)

    # Make program instance
    prog = Program()

    # Load the program and get its entry point
    entry_point = prog.load(cpu, args.filename)
    if not entry_point:                     # abort if no entry point found
        sys.exit()

    # Load input files
    if args.input:
        for item in args.input:
            load_file(cpu, item[0], item[1], item[2])

    # Execute program
    cpu.run(entry_point)

    # Save output files
    if args.output:
        for item in args.output:
            save_file(cpu, item[0], item[1], item[2])

    # Show statistics
    Stat.show()


if __name__ == '__main__':
    main()

