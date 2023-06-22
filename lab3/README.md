# Assembly is Fun

In this third phase, we write the blend algorithm in assembly and with custom vector instructions.

You will learn
* how to extend the GNU assembler/disassembler with new vector instructions
* how a simple RISC-V pipeline simulator works
* how to support the new vector instructions in the simulator

[[_TOC_]]


## Important Dates

| Date | Description |
|:---  |:--- |
| Monday, May 15 | Handout |
| Sunday, June 4 | Submission deadline |

### Hand-out

Fork this repository into your namespace and make sure its visibility is set to private. Read the instructions here carefully, then clone your copy of the lab to your computer and get to work.

**Important:** Make sure to push your work to your private repository in your namespace and not the official handout. *Pushing code or merge requests to the handout will result in a 50-100% point reduction*. If you do not know how to use Git, look it up. Most Git commands support the `--dry-run` option that shows you what actions a command would perform without actually executing them.

### Submission

Commit and push your work frequently to avoid data loss. When you are ready to submit your code for grading, create a tag called "Submission". The timestamp of the "Submission" tag is considered your submission time.

To create a tag, visit the repository on GitLab and navigate to Repository -> Tags. Enter "Submission" as the Tag name then hit "Create tag". You can leave the other fields empty.

If you later discover an error and want to update your submission, you can delete the "Submission" tag, fix your solution, and create a new "Submission" tag. If that happens _after_ the submission deadline, you also need to email the TAs so that they are aware of the update to your submission.

### Coding Rules

In this lab, you will change a number of files, from the RISC-V GNU toolchain, to the PyRISC simulator, to your assembly code. In the following sections, more information about which files to modify is provided. As usual, write legible code, follow the style of the file(s) you are modifying, and do not forget to comment your work.


## Image Manipulation in Assembly

The GNU C compiler is *very* good at producing optimized code - in general, it is not possible to achieve much faster code by coding an algorithm directly in assembly.

In our fixed-point .8-bit implementation of the image manipulation algorithms, we have processed each color component separately. Since each channel only requires 8 bit, it might be possible to pack more than one channel into a 32-bit register and execute operations in parallel. This technique is called SIMD - single instruction, multiple data.

### SIMD Computations

Consider a 32-bit ARGB representation of a pixel: each channel occupies 8 bit
```
-------------------------------------
|aaaaaaaa|rrrrrrrr|gggggggg|bbbbbbbb|
-------------------------------------
```

Let's see what happens if we add two pixels:
```
                                    -------------------------------------
    p1 = 0x41526379 = 41 52 63 79 = |01000001|01010010|01100011|01111001|
                                    -------------------------------------
   +p2 = 0x87634128 = 87 63 41 28 = |10000111|01100011|01000001|00101000|
                                    -------------------------------------
 p1+p2 = 0xc8b5a4a1 = c8 b5 a4 a1 = |11001000|10110101|10100100|10100001|
                                    =====================================
```

This technique works for addition and subtraction as long as there is no overflow from one channel to the next. Consider:
```
                                    -------------------------------------
    p1 = 0x41526379 = 41 52 63 99 = |01000001|01010010|01100011|10011001|
                                    -------------------------------------
   +p2 = 0x87634128 = 87 63 41 88 = |10000111|01100011|01000001|10001000|
                                    -------------------------------------
 p1+p2 = 0xc8b5a4a1 = c8 b5 a5 21 = |11001000|10110101|10100101|00100001|
                                    =====================================
```
The correct value for the blue channel at posision 7-0 is 0x99+0x88=0x121. Since the value doesn't fit into 8 bits anymore, it overflowed into the green channel, changing the (correct) result of 0xa4 to 0xa5.

With multiplication, things get even more complicated. Multiplying two .8 fixed-point values results in a .16 fixed-point value - we have to shift that value right by 8 position. Obviously, we cannot achieve that with standard integer operations.


### SIMD Instructions

Many processors offer special instructions to support SIMD computations. Intel's MMX and SSE extensions are examples thereof. We want to design our own SIMD instructions to accelerate color computions.

There are various ways to design the layout of a vector register and the instructions that operate on such values. For this lab, we settle on the follwing:

* **Vector register format**  
  Vector registers are 32-bit - this allows us to operate on general-purpose registers that contain vectorized values. We divide a register into three channels of 10 bits each as follows:  
  ```
  ----------------------------------------
  |00|aa.aaaaaaaa|gg.gggggggg|bb.bbbbbbbb|
  ----------------------------------------
  ```
  We drop the alpha channel since the computation of the alpha value is often different than the computation of the color channels. Furthermore, we add two bits before the radix point, yielding a 2.8 fixed-point format. This allows us to deal with
  somewhat larger values internally and obtain a higher precision.

* **Vector operations**  
  Vector operations are aware of the vector encoding and ensure that values do not overflow from one element into the next. To implement our algorithms with vector operations, we defined the following operations  
  | Operation | Operands    | Computation | Description |
  |:----------|:-----------:|:------------|:------------|
  | svunpack   | d <- s      | d = (s&0xff0000)<<4 &vert; (s&0xff00)<<2 &vert; s&0xff | Unpack a ARGB value into the vector format. Drops the alpha component. |
  | svpack     | d <- s1, s2 | d = s2<<24 &vert; SAT((v>>20)&0x3ff)<<16 &vert; SAT((v>>10)&0x3ff)<<8 &vert; SAT(v&0x3ff) where SAT(c)=c < 0x100 ? c : 0xff | Pack a vector register into ARGB format. Color channels are saturated at 0xff. The alpha value is provided separately. |
  | svbrdcst   | d <- s      | d = (s<<20)&0x3ff00000 &vert; (s<<10)&0xffc00 &vert; s&0x3ff | Broadcast a single value to all channels in a vector. |
  | svaddi     | d <- s, i   | d = (s+(i<<20))&0x3ff00000 &vert; (s+(i<<10))&0xffc00 &vert; (s+i)&0x3ff | Add a single value to all channels in a vector. |
  | svadd      | d <- s1, s2 | d = ((s1&0x3ff00000)+(s2&0x3ff00000))&0x3ff00000 &vert; ((s1&0xffc00)+(s2&0xffc00))&0xffc00 &vert; ((s1&0x3ff)+(s2&0x3ff))&0x3ff | Add two vector registers. |
  | svsub      | d <- s1, s2 | d = ((s1&0x3ff00000)-(s2&0x3ff00000))&0x3ff00000 &vert; ((s1&0xffc00)-(s2&0xffc00))&0xffc00 &vert; ((s1&0x3ff)-(s2&0x3ff))&0x3ff | Subtract two vector registers. |
  | svmul      | d <- s1, s2 | d = ROUND(((s1>>20)&0x3ff)\*((s2>>20)&0x3ff))<<20 &vert; ROUND(((s1>>10)&0x3ff)\*((s2>>10)&0x3ff))<<10 &vert; ROUND((s1&0x3ff)\*(s2&0x3ff)) where ROUND(c)=(c>>8) + (c&0xff)<0x80 ? 0 : 1 | Multiply two vector registers and round up. |

  To prevent name clashes with existing extensions, we prepend our operations with `s` and `v` for *S*NU *V*ector operation. A reference implementation of the operations in C is provided in `vector_math.c/h`. 


## Extending RISC-V with Custom Instructions

Extending RISC-V with custom instructions requires the following steps:
1. Define the instruction encoding for each custom operation
2. Add the instructions to the RISC-V GNU toolchain such that we can assemble/disassemble our new instructions
3. Extend a RISC-V simulator to support the new instructions

In the following, we will explain each step in more detail.


### 1. Defining the Instruction Encoding for new Instructions

RISC-V has been built to be extensible. The instruction encoding defines several blocks to be used by custom extensions. Refer to Chapter 24 of the [The RISC-V Instruction Set Manual, Volume I: Unprivileged ISA (Document Version 20191213)](https://riscv.org/wp-content/uploads/2019/12/riscv-spec-20191213.pdf). Table 24.1 shows that several blocks are reserved for custom extensions. In this lab, we will encode our instructions in the *custom_0* block, i.e., the opcode field will be 0001011b = 0xb for all instructions.

We encode all instructions as R-type except `vaddi` which is of type I. Both R and I-type instructions have a `funct3` field which we are going to use to distinguish between the different operations. Let's agree on the following encoding

| Operation | Type | Opcode  | Funct3 | Notes |
|:----------|:----:|:-------:|:------:|:------|
| svunpack  |  R   | 0001011 | 000    | rs2 not used. |
| svpack    |  R   | 0001011 | 001    | |
| svbrdcst  |  R   | 0001011 | 010    | rs2 not used. |
| svaddi    |  I   | 0001011 | 011    | |
| svadd     |  R   | 0001011 | 100    | |
| svsub     |  R   | 0001011 | 101    | |
| svmul     |  R   | 0001011 | 110    | |

Examples:
```
                              |funct7 | rs2 | rs1 |fu3| rd  |opcode |
  svbrdcst  x28, x29      -->  0000000 00000 11101 010 11100 0001011
  svmul     x6, x7,x10    -->  0000000 01010 00111 110 00110 0001011

                               |    imm     | rs1 |fu3| rd  |opcode |
  svaddi    x9, x11, 5    -->   000000000101 01011 011 01001 0001011
```

### 2. Adding the Instructions to the RISC-V GNU Toolchain

Our goal is to write assembly code using the new instructions, then compile the assembly code to machine code with the RISC-V gcc. We also want the our new instructions are correctly disassembled by objdump.

To achieve that, we have to modify the RISC-V GNU toolchain. If you use the CSAP VM, all necessary tools are already installed, but you have to extract and install the toolchain under your `share` folder since the partitions of the CSAP VM are only as large as absolutely necessary. If you use your own Linux installation, use a recent version of Ubuntu (>=18.04 TLS) or MacOS. For Windows users, the combination of WSL and Ubuntu should work - however, we do not provide technical support for Windows.

#### Installing the Necessary Developer Tools
If you use the CSAP VM, you can ignore this section.

**Ubuntu users** install the following build packages with `apt-get`: autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev.  
**MacOS users** use `brew` to install: gawk gnu-sed gmp mpfr libmpc isl zlib expat.

#### Downloading the RISC-V GNU Toolchain
You can now download the RISC-V GNU Toolchain from Github. If you work in the CSAP VM, make sure to change into a folder under your `share` directory first.
```bash
$ git clone --recursive https://github.com/riscv/riscv-gnu-toolchain
```
This will take a while and download around 3GiB of data. The entire repository occupies around 7GiB. If you do not have that much free space, [you can experiment](https://stackoverflow.com/questions/1209999/how-to-use-git-to-get-just-the-latest-revision-of-a-project) with the `--depthi=1` and `--shallow-submodules` parameter.

#### Building the RISC-V GNU Toolchain
First, configure the toolchain with
```bash
$ cd riscv-gnu-toolchain
$ mkdir build
$ cd build
$ ../configure --prefix=/opt/riscv --with-arch=rv32g --with-abi=ilp32d
```
Feel free to change the installation path to something else, such as `--prefix=../../rvgtc/` if you do not have sufficient space under `/opt/`. If you do change the installation path, make sure to adjust the path in the following examples.

You can now build the toolchain a first time. This may take a long time.
```
sudo make -j$(nproc) linux
```

#### Adding new Instructions to Binutils
The assembler and disassembler use the opcode definitions located in `binutils/include/opcode/riscv-opc.h` and `binutils/opcodes/riscv-opc.c`.

First, we define the opcode encodings in the header file `binutils/include/opcode/riscv-opc.h`. You will observe that for each instruction a `MATCH` and a `MASK` is defined. The `MATCH` constant defines the bits of the instruction encoding (with all registers and immediates set to 0). The `MASK`, on the other hand, must have all bits set to 1 that belong to the instruction encoding. This includes fields that are not used, such as the `rs2` field for the svunpack instruction.

You can add your instructions anywhere in the header file inside the `#ifndef/#endif` blocks.

Here is an example to get you started:
```C
...
#define MATCH_SVUNPACK 0x0000000b        // custom-0, funct3 = 0
#define MASK_SVUNPACK  0xfff0707f        // funct7, funct3, opcode, and rs2
#define MATCH_SVPACK   0x0000100b        // custom-0, funct3 = 1
#define MASK_SVPACK    0xfe00707f        // funct7, funct3, and opcode
...
```

Second, we need to add the instructions to the instruction table `risc_v_opcodes[]` in `binutils/opcodes/riscv-opc.c`. The table has eight columns with the following contents:
* mnemonic: how the operation will be called in assembly code. Usually set to the lowercase of the MATCH/MASK.
* xlen: set to 0 for 32 bit operations.
* isa: this defines the instruction set architecture that supports this instruction. Set to `INSN_CLASS_I` for all except `svmull` which should be `INSN_CLASS_M` since it requires multiplication hardware.
* operands: defines the number and type of operands.
    * `d`: destination register
    * `s`: source register 1
    * `t`: source register 2
    * `j`: immediate value
* match: set to the `MATCH_xxx` constant of the instruction.
* mask: set to the `MASK_xxx` constant of the instruction.
* match_func: set to `match_opcode`.
* pinfo: set to 0.

The instructions `svunpack` and `svpack` would be defined as follows:
```C
...
{"svunpack",    0, INSN_CLASS_I,     "d,s",       MATCH_VUNPACK, MASK_VUNPACK, match_opcode, 0 },
{"svpack",      0, INSN_CLASS_I,     "d,s,t",     MATCH_VPACK, MASK_VPACK, match_opcode, 0 },
...
```

Add the remaining operations as well, then re-compile the toolchain. You only have to recompile the binutils:
```bash
$ cd build/build-binutils-linux
$ sudo make -j$(nproc)
$ sudo make install
```

You should now be able to use the new instructions in your own hand-crafted assembly code.


#### Submission

Enter the directory `binutils` directory under your RISC-V GNU toolchain sources and run
```bash
$ git diff > binutils.patch
```

This will generate a patch file containing your modifications. Add that file to your repository and include it in the submission.



### 3. Extending a Simulator to Support the New Instructions

In this (and the next) lab, we use a customized version of the PyRISC simulator that was developed at SNU. The customized simulator supports
* customizing the address and size of the instruction memory
* customizing the address and size of the data memory
* loading files into the data memory before execution begins
* saving data memory block to files after execution stops
* the `mul` instruction

These modifications allow us to run the image manipulation algorithms with ease.

The simulator is located in the `pyrisc-csap` directory of this handout and requires Python 3. If you run an older installation of Ubuntu or MacOS, [the Internet](https://duckduckgo.com/?q=%22how+to+install+python+3+on%22") will know how to install Python 3 on your system.

#### PyRISC CSAP

PyRISC CSAP is a customized version of the [5-stage RISC-V simulator](https://github.com/snu-csl/pyrisc). Refer to the original version for documentation and instructions. Comparing the modifications related to the added `mul` instructions in PyRISC CSAP with the original version will give you a good idea on how to approach this task.

Similar to the RISC-V GNU toolchain, we have to modify the simulator to make it aware of the new instructions, i.e., how to decode, execute, and print these instructions.

1. **Define instruction encodings**  
   The instruction encodings of PyRISC are defined in the file `isa.py`. As the GNU toolchain, PyRISC requires a `MATCH` and a `MASK` constant to define a new instruction.
   Locate the match pattern for `MUL` and the corresponing `MUL_MASK` a bit further down. Add the new instructions in the same format.
2. **Define the control signals**  
   Next, we define the control signals for the new instructions in the file `control.py` and `consts.py`. First, we need to add the new instructions to the `csignals` table. The table contains a key and a list with 12 values as follows:  
   ```Python
   MUL : [ Y, BR_N  , OP1_RS1, OP2_RS2, OEN_1, OEN_1, ALU_MUL  , WB_ALU, REN_1, MEN_0, M_X  , MT_X, ],
   ```
   * key: mnemonic (how the operation is called internally)
   * values: a list of 12 values. In order:  
      * val_inst: set to `Y`.
      * br_type: branch type. Set to `BR_N` is the instruction does not branch.
      * op1_sel: operand 1 selection. Set to `OP1_RS1` to retrieve the value from the register file. Set to `OP1_X` if the instruction does not have source 1 operand.
      * op2_sel: operand 2 selection. Set to `OP2_RS2` to retrieve the value from the register file. Set to `OP2_IMI` if the second operand is a I-type immediate, or `OP2_X` if the instruction does not have a second operand.
      * rs1/2_oen: source operand 1/2 enable. Set to `OEN_1` if this register operand needs to be considered when checking for data hazards, otherwise set to `OEN_0`. Do this for both register operands.
      * alu_fun: ALU function. For new ALU operations, you need to define additional `ALU_xxx` constants in `const.py` and use them here.
      * wb_sel: selection of the write-back value. Set to `WB_ALU` to write back the value from the ALU, `WB_MEM` to write back the data from the memory, or `WB_X` if no data is written back.
      * rw_wen: register file write enable. Set to `REN_1` to write a value back, `REN_0` otherwise.
      * mem_en: memory enable. Set to `MEN_1` to enable the memory, `MEN_0` otherwise.
      * mem_fun: memory function. `M_XRD` to read, `M_XWR` to write, and `M_X` if the memory is not active.
      * msk_sel: memory mask (bit width) selection. Since our new instructions to not access the memory, set to `MT_X`.

    Check whether you need to modify the control signal generation for the new instructions by inspecting the code below the `csignals` table.
3. **Define the log output**--
   The file `datapath.py` contains helper strings to print the operation of the pipeline duing operation. We need to add our new instructions to the `ALU_OPS` table in the method `EX(Pipe)::log(self):`
   ```Python
   ALU_OPS = {
     ...
     ALU_MUL     : f'# {self.alu_out:#010x} <- {self.op1_data:#010x} * {self.alu2_data:#010x}',
     ...
   }
   ```
   Add the log formats for the new instructions accordingly. Refer to the other strings in the table to learn what values are available.
4. **Define the functionality**  
   As a final step, we need to implement the ALU operations as defined above for each instruction. Locate the method `op(self, alufun, alu1, alu2)` in class `ALU`. Implement the functionality of the new operations by adding a new `elif ...` block to the table. Again, we have already implement `ALU_MUL` to give you an idea how to approach this:
   ```Python
   class ALU(object):

    ...

    def op(self, alufun, alu1, alu2):

        np.seterr(all='ignore')
        if alufun == ALU_ADD:
            output = WORD(alu1 + alu2)
        elif alufun == ALU_SUB:
            ...
        elif alufun == ALU_MUL:
            output = WORD(alu1 * alu2)
        ...
        else:
            output = WORD(0)

        return output
   ```

You should now have a simulator that can execute your custom instructions.

#### Submission

Add the modified files to your Git repository and include them in your submission.


## Implementing the Blending Algorithm in Assembly

Finally, we are ready to implement the blending algorithm in assembly.

The handout contains a number of files that help you execute C + assembly code in the PyRISC CSAP simulator.

|File | Description |
|:----|:------------|
|Makefile | GNU Make driver. Use to compile and run your code.|
|link.ld  | GNU linker script. Do not modify.|
|startup.s| Initial startup code; replaces the C runtime. Do not modify.|
|blend_pyrisc.c| Main driver, invokes your blend algorithm. Modify only to change the alpha value (`ALPHA`).|
|imlib_pyrisc.c| Image library to read/write image files in our CSAP RAW format.|
|blend/imlib.h| Header files, do not modify.|
|vector_math.c/h| Vector math implementation in C. Do not modify. No guarantees for correctness. |
|blend_vasm.s| RISC-V assembly file. This is where your work goes.|

The `blend_vasm.s` file contains skeletton code that allows you to compile the project.


### Building the RISC-V Binary

Use the provide Makefile and run
```bash
$ make compile
```

If you have installed your RISC-V GNU toolchain in a different location than `/opt/riscv/` you will need to adjust the `PREFIX` variable in the Makefile.

### Running the RISC-V Binary

The Makefile also assists you in running the code
```bash
$ make run
```

This invokes the simulator with the blend_pyrisc binary and, if everything goes well, saves the generated output to `images/blended.raw`. You can use the `raw2img.py` script from Part 1 to convert the file to a PNG and inspect it visually. The Makefile contains a target `convert` that automates this conversion; also here, adjust paths if necessary.

Run `pyrisc-csap/snurisc5.py --help` to see all available options. You may want to increase the logging level when debugging. The `--cycle` (`-c`) command line parameter will be very useful when you do not want to debug your code from the beginning.


### Getting started

A good way to get started is to program simple functions in C, translate them to RISC-V assemly, and then inspect the generated code. If you are unsure about how to implement the SIMD version of the blend algorithm, you can first implement in C using the provided `vector_math` library. To do so, create a copy of your `blend_int.c` file in Part 2
```bash
$ cd ../part-2
$ cp blend_int.c blend_vint.c
$ cp ../part-3/vector_math.* .
```
Then include `vector_math.h` in your new file, and modify the `blend_driver` target in the Makefile to compile `blend_vint.c` instead of `blend_int.c`.


<div align="center" style="font-size: 1.75em;">

**Happy coding!**
</div>
