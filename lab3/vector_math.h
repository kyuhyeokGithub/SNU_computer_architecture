#ifndef __VECTOR_MATH__
#define __VECTOR_MATH__

#define BSHIFT  0
#define GSHIFT 10
#define RSHIFT 20
#define MASK   0x3ff

#define FPMSHIFT 8

#define VRMASK 0x3ff00000
#define VGMASK 0x000ffc00
#define VBMASK 0x000003ff

typedef unsigned char uint8;
typedef signed int int12;     // signed 12-bit
typedef unsigned int uint32;

typedef unsigned int argb;
typedef unsigned int vrgb;

vrgb vunpack(argb v);
argb vpack(vrgb v, uint8 alpha);
vrgb vbrdcst(uint32 w);
vrgb vaddi(vrgb v, int12 i);
vrgb vadd(vrgb va, vrgb vb);
vrgb vsub(vrgb va, vrgb vb);
vrgb vmul(vrgb va, vrgb vb);

#endif // __VECTOR_MATH__
