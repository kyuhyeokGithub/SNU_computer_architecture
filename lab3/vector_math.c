#include "vector_math.h"


vrgb vunpack(argb v)
{
  return ((v & 0xff0000) << 4) | ((v & 0xff00) << 2) | (v & 0xff);
}

argb vpack(vrgb v, uint8 alpha)
{
  uint32 r = (v >> RSHIFT) & MASK;
  if (r >= 0x100) r = 0xff;

  uint32 g = (v >> GSHIFT) & MASK;
  if (g >= 0x100) g = 0xff;

  uint32 b = (v >> BSHIFT) & MASK;
  if (b >= 0x100) b = 0xff;

  return (alpha << 24) | (r << 16) | (g << 8) | (b);
}

vrgb vbrdcst(uint32 w)
{
  return ((w << RSHIFT) & VRMASK) |
         ((w << GSHIFT) & VGMASK) |
         ((w << BSHIFT) & VBMASK);
}

vrgb vaddi(vrgb v, int12 i)
{
  return ((v + (i << RSHIFT)) & VRMASK) |
         ((v + (i << GSHIFT)) & VGMASK) |
         ((v + (i << BSHIFT)) & VBMASK);
}

vrgb vadd(vrgb va, vrgb vb)
{
  return (((va & VRMASK) + (vb & VRMASK)) & VRMASK) |
         (((va & VGMASK) + (vb & VGMASK)) & VGMASK) |
         (((va & VBMASK) + (vb & VBMASK)) & VBMASK);
}

vrgb vsub(vrgb va, vrgb vb)
{
  return (((va & VRMASK) - (vb & VRMASK)) & VRMASK) |
         (((va & VGMASK) - (vb & VGMASK)) & VGMASK) |
         (((va & VBMASK) - (vb & VBMASK)) & VBMASK);
}

vrgb vmul(vrgb va, vrgb vb)
{
  //  not rounded
  //  return ((((((va >> RSHIFT) & MASK) * ((vb >> RSHIFT) & MASK)) >> FPMSHIFT) & MASK) << RSHIFT) |
  //         ((((((va >> GSHIFT) & MASK) * ((vb >> GSHIFT) & MASK)) >> FPMSHIFT) & MASK) << GSHIFT) |
  //         ((((((va >> BSHIFT) & MASK) * ((vb >> BSHIFT) & MASK)) >> FPMSHIFT) & MASK) << BSHIFT);

  // rounded
  uint32 r  = ((va >> RSHIFT) & MASK) * ((vb >> RSHIFT) & MASK);
  uint32 rr = (r >> FPMSHIFT) + ((r & 0xff) >= 128 ? 1 : 0);

  uint32 g  = ((va >> GSHIFT) & MASK) * ((vb >> GSHIFT) & MASK);
  uint32 gr = (g >> FPMSHIFT) + ((g & 0xff) >= 128 ? 1 : 0);

  uint32 b  = ((va >> BSHIFT) & MASK) * ((vb >> BSHIFT) & MASK);
  uint32 br = (b >> FPMSHIFT) + ((b & 0xff) >= 128 ? 1 : 0);

  return (rr << RSHIFT) | (gr << GSHIFT) | (br << BSHIFT);
}
