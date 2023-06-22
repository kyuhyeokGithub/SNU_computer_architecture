#/-------------------------------------------------------------------------------------------------
#/ 4190.308 Computer Architecture                                                       Spring 2023
#/
#// @file
#// @brief Image blending (vector operations)
#//        This module implements a function that blends two images together filter (assembly 
#//        integer vector version)
#//
#//
#// @section changelog Change Log
#// 2023/MM/DD Your Name Comment
#//
#/-------------------------------------------------------------------------------------------------

    .option nopic
    .attribute unaligned_access, 0
    .attribute stack_align, 16

    .text
    .align 4
    .globl blend_asm
    .type  blend_asm, @function

# struct Image {                 Ofs
#     uint8 *data;                 0
#     int height;                  4
#     int width;                   8
#     int channels;               12
# };

# int blend_asm(                 Reg
#       struct Image *blended,    a0
#       struct Image *img1,       a1
#       struct Image *img2,       a2
#       int overlay,              a3
#       int alpha                 a4
#     )
blend_asm:
    # Check parameters
    li    t0, 1
    bne   a3, t0, .OverlayError   # if overlay != 1 goto .OverlayError

    lw    t1, 12(a1)              # t1 = img1->channels
    lw    t2, 12(a2)              # t2 = img2->channels
    li    t0, 4
    bne   t1, t0, .ChannelError   # if img1->channels != 4 goto .ChannelError
    bne   t2, t0, .ChannelError   # if img2->channels != 4 goto .ChannelError


    # Initialize blended image
    lw    s1, 0(a1)               # s1 = img1->*data
    lw    s2, 4(a1)               # s2 = img1->height
    lw    s3, 8(a1)               # s3 = img1->width
    lw    s4, 0(a2)               # s4 = img2->*data
    lw    s5, 4(a2)               # s5 = img2->height
    lw    s6, 8(a2)               # s6 = img2->width
    
    # ERROR CHECK
    bne   s2, s5, .HeightError    # check Heights are same
    bne   s3, s6, .WidthError     # check Widths are same
    li    s5, 0                   # s5 = 0
    bge   s5, s3, .NonzeroError   # check loop condition : width
    bge   s5, s2, .NonzeroError   # check loop condition : height
    
    # RESTART : Initialize blended image
    mv    s7, a0                  # s7 = *blended
    mul   t5, s2, s3              # 
    sll   t5, t5, 0x2             # t5 = height * width * channels
    lw    s11, 0(s7)              # s11 = blended->*data
    sll   s9, s3, 0x2             # s9 = width * 4
    addi  t3, s9, 0               # t3 = width * 4
    mv    s6, s11                 # s6 = 0
    li    t0, 256                 # t0 = 256

    # Blend

.HeightLoop:
    li    t1, 0                   # t1 = 0
    add   s11, s6, s5             # s11 = blended->*data[0,0,0] to blended->*data[h,0,0]
    add   a6, s4, s5              # a6 : From img2.*data[0,0,0] to img2.*data[h,0,0] 
    add   a7, s1, s5              # a7 : From img1.*data[0,0,0] to img1.*data[h,0,0] 

.WidthLoop:
    addi       a7, a7, 4          # a7 = img1.*data[h,w,0]
    addi       a6, a6, 4          # a6 = img2.*data[h,w,0]
    addi       s11, s11, 4        # s11 = blended.*data[h,w,0]
    lw        s9, 0(a7)           # s9 = img1.data[h,w,0:4]
    lw        s10, 0(a6)          # s10 = img2.data[h,w,0:4]

    srli      t4, s10, 24         # t4 = img2.data[h,w,3]
    srli      t2, s9, 24          # t2 = img1.data[h,w,3]

    mul       t4, t4, a4          # t4 = img2.data[h,w,3] * alpha
    srli      t4, t4, 0x8         # t4 = t4 >> 8 ( =alpha_prime )

    sub       t6, t0, t4          # t6 = 256 - alpha_prime ( =beta_prime )
    svbrdcst  t4, t4              # t4 = sv(xx|alpha_prime|alpha_prime|alpha_prime)
    svbrdcst  t6, t6              # t6 = sv(xx|beta_prime|beta_prime|beta_prime)
    
    svunpack  s9, s9              # s9 = svunpack(s9)
    svunpack  s10, s10            # s10 = svunpack(s10)       
    svmul     s9, s9, t6          # s9 = s9 * beta_prime for RGB
    svmul     s10, s10, t4        # s10 = s10 * alpha_prime for RGB
    svadd     s9, s9, s10         # s9 = s9 svadd s10
    svpack    s9, s9, t2          # s9 = new_data[h,w,0:4]
    sw        s9, 0(s11)          # store s9 to blended.data[h,w,0:4]

    addi      t1, t1, 4           # t1 = t1 + 4
    bne       t1, t3, .WidthLoop  # goto WidthLoop

    add       s5, s5, t3          # s5 += width * 4
    bne       s5, t5, .HeightLoop # goto HeightLoop
    
    li        t1, 4               # t1 = 4

# STORE VALUES FOR BLENDED
    sw    s2, 4(s7)               # blended->height = img1->height
    sw    s3, 8(s7)               # blended->width = img1->width
    sw    t1, 12(s7)              # blended->channels = img1->channels
    
    mv    a0, x0
    ret

.OverlayError:
    li    a0, -1
    ret

.ChannelError:
    li    a0, -2
    ret

.HeightError:
    li    a0, -3
    ret

.WidthError:
    li    a0, -4
    ret

.NonzeroError:  
    li    a0, -5
    ret
