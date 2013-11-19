/**
*AUTHOR : Babel yoann
*TITLE  : Palette.cpp, Palette.h implementation
*CREATED: 27/06/96
*DATE   : 27/06/96
*BUGS   :
*/

#include <math.h>
#include "Palette.h"

namespace Go4D
{

  /*****************************************************************************\
  * RGBColor
  \*****************************************************************************/

  RGBColor::RGBColor()
  {
    r = 0; g = 0; b = 0;
  }

  RGBColor::RGBColor(byte pr, byte pg, byte pb)
  {
    r = pr;
    g = pg;
    b = pb;
  }

  RGBColor::RGBColor(const RGBColor & aRGBColor)
  {
    r = aRGBColor.r;
    g = aRGBColor.g;
    b = aRGBColor.b;
  }

#ifdef DEBUGOUT
  ostream & RGBColor::DebugOut(ostream & os) const
  {
    os << "RBG" << incendl
      << "{" << iendl
      << "R:" << int(r) << " - "
      << "G:" << int(g) << " - "
      << "B:" << int(b) << iendl
      << "}" << decendl;
    return os;
  }
#endif

  RGBColor & RGBColor::operator = (const RGBColor &aRGBColor)
  {
    r = aRGBColor.r;
    g = aRGBColor.g;
    b = aRGBColor.b;
    return *this;
  }

  bool RGBColor::operator == (const RGBColor &aRGBColor) const
  {
    return  (r==aRGBColor.r) && (g==aRGBColor.g) && (b==aRGBColor.b);
  }

  bool RGBColor::operator != (const RGBColor &aRGBColor) const
  {
    return  ! ((r==aRGBColor.r) && (g==aRGBColor.g) && (b==aRGBColor.b));
  }

  RGBColor RGBColor::operator | (const RGBColor & aRGBColor) const
  {
    return   RGBColor(r | aRGBColor.r, g | aRGBColor.g, b | aRGBColor.b);
  }

  RGBColor RGBColor::operator + (const RGBColor & aRGBColor) const
  {
    RGBColor tmp(r | aRGBColor.r, g | aRGBColor.g, b | aRGBColor.b);
    real norm = min(this->Abs(), aRGBColor.Abs());
    real tmpnorm = tmp.Abs();
    if (!tmpnorm)
      tmp *= (norm/tmpnorm);
    return tmp;
  }

  RGBColor RGBColor::operator - (const RGBColor & aRGBColor) const
  {
    int lr = r-aRGBColor.r;
    int lg = g-aRGBColor.g;
    int lb = b-aRGBColor.b;
    return RGBColor(byte(abs(lr)), byte(abs(lg)), byte(abs(lb)));
  }

#define SUP(x) ((x)>255?255:(x))
  RGBColor RGBColor::operator * (real coef) const
  {
    assert(coef>=0);
    int lr = Trunc(abs(r*coef));
    int lg = Trunc(abs(g*coef));
    int lb = Trunc(abs(b*coef));
    return RGBColor(byte(SUP(lr)), byte(SUP(lg)), byte(SUP(lb)));
  }

  RGBColor RGBColor::operator ^ (const RGBColor & aRGBColor) const
  {
    int lr = int(r*aRGBColor.r)>>8;
    int lg = int(g*aRGBColor.g)>>8;
    int lb = int(b*aRGBColor.b)>>8;
    return RGBColor(byte(SUP(lr)), byte(SUP(lg)), byte(SUP(lb)));
  }

  RGBColor & RGBColor::operator *= (real coef)
  {
    assert(coef>=0);
    r = byte(SUP(abs(r*coef)));
    g = byte(SUP(abs(g*coef)));
    b = byte(SUP(abs(b*coef)));
    return *this;
  }

  RGBColor & RGBColor::operator *= (const RGBColor & aRGBColor)
  {
    r = byte(SUP(int(int(r*aRGBColor.r)>>8)));
    g = byte(SUP(int(int(g*aRGBColor.r)>>8)));
    b = byte(SUP(int(int(b*aRGBColor.r)>>8)));
    return *this;
  }

  real RGBColor::Abs() const
  {
    real result = r*r+g*g+b*b;
    return sqrt(result);
  }

  int RGBColor::Norm() const
  {
    return abs(r+g+b);
  }

#ifdef DEBUGOUT
  ostream & operator << (ostream & os, const RGBColor &aRGB)
  {
    return aRGB.DebugOut(os);
  }
#endif

  /*****************************************************************************\
  * Palette
  \*****************************************************************************/

  Palette::Palette(int aCount)
  {
    assert(aCount>0);
    fCount = aCount;
    alloc(fEntries = new RGBColor[fCount]);
  }

  Palette::~Palette()
  {
    delete [] fEntries;
  }

  Palette::Palette(const Palette &aPalette)
  {
    fCount = aPalette.fCount;
    alloc(fEntries = new RGBColor[fCount]);
    for (int i=0; i<fCount; i++)
      fEntries[i] = aPalette.fEntries[i];
  }

#ifdef DEBUGOUT
  ostream & Palette::DebugOut(ostream & os) const
  {
    os << "Palette" << incendl
      << "{" << iendl;
    for (int i=0; i<fCount; i++)
      fEntries[i].DebugOut(os);
    os << "}" << decendl;
    return os;
  }
#endif

  int Palette::operator == (const Palette & aPalette) const
  {
    int result = TRUE;
    for (int i = 0; i < fCount; i++)
      if (fEntries[i] != aPalette.fEntries[i])
      {
        result = FALSE;
        break;
      }
      return result;
  }

  int Palette::operator != (const Palette & aPalette) const
  {
    int result = TRUE;
    for (int i = 0; i < fCount; i++)
      if (fEntries[i] == aPalette.fEntries[i])
      {
        result = FALSE;
        break;
      }
      return result;
  }

  Palette Palette::operator | (const Palette & aPalette) const
  {
    Palette result(aPalette.fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] | aPalette.fEntries[i];
    return result;
  }

  Palette Palette::operator + (const Palette & aPalette) const
  {
    Palette result(aPalette.fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] + aPalette.fEntries[i];
    return result;
  }

  Palette Palette::operator - (const Palette & aPalette) const
  {
    Palette result(aPalette.fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] - aPalette.fEntries[i];
    return result;
  }

  Palette Palette::operator * (const Palette & aPalette) const
  {
    Palette result(aPalette.fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] ^ aPalette.fEntries[i];
    return result;
  }

  Palette Palette::operator | (const RGBColor & RGBColor) const
  {
    Palette result(fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] | RGBColor;
    return result;
  }

  Palette Palette::operator + (const RGBColor & RGBColor) const
  {
    Palette result(fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] + RGBColor;
    return result;
  }

  Palette Palette::operator - (const RGBColor & RGBColor) const
  {
    Palette result(fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] - RGBColor;
    return result;
  }

  Palette Palette::operator ^ (const RGBColor & RGBColor) const
  {
    Palette result(fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] ^ RGBColor;
    return result;
  }

  Palette Palette::operator * (real aCoef) const
  {
    Palette result(fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i]*aCoef;
    return result;
  }

  Palette& Palette::operator *= (real aCoef)
  {
    for (int i = 0; i<fCount; i++)
      fEntries[i] *= aCoef;
    return *this;
  }

  Palette& Palette::operator *= (const RGBColor & RGBColor)
  {
    for (int i = 0; i<fCount; i++)
      fEntries[i] *= RGBColor;
    return *this;
  }

  RGBColor & Palette::BestRGB(const RGBColor & RGBColor) const
  {
    int best=0;
    int dist=(fEntries[0]-RGBColor).Norm();
    int tmp;
    for (int i=1; i<fCount; i++)
      if ((tmp=(fEntries[i]-RGBColor).Norm())<dist)
      {
        dist = tmp;
        best = i;
      }
      return fEntries[best];
  }

  int Palette::Best(const RGBColor & RGBColor) const
  {
    int best=0;
    int dist=(fEntries[0]-RGBColor).Norm();
    int tmp;
    for (int i=1; i<fCount; i++)
      if ((tmp=(fEntries[i]-RGBColor).Norm())<dist)
      {
        dist = tmp;
        best = i;
      }
      return best;
  }

#ifdef DEBUGOUT
  ostream & operator << (ostream & os, const Palette & aPalette)
  {
    return aPalette.DebugOut(os);
  }
#endif

  struct BaseCol
  {
    unsigned long Red;
    unsigned long Green;
    unsigned long Blue;
  };

  struct BaseCol kBase [16] =
  {
    {  128L,   0L,   0L}, //0    RED
    {  128L,  32L,   0L}, //16
    {  128L,  64L,   0L}, //32
    {  128L,  96L,   0L}, //48
    {  128L, 128L,   0L}, //64
    {  0L,   128L,   0L}, //80   GREEN
    {  0L,   128L,  32L}, //96
    {  0L,   128L,  64L}, //112
    {  0L,   128L,  96L}, //128
    {  0L,   128L, 128L}, //144
    {  0L,     0L, 128L}, //160  BLUE
    {  32L,    0L, 128L}, //176
    {  64L,    0L, 128L}, //192
    {  96L,    0L, 128L}, //208
    {  128L,   0L, 128L}, //224
    {  128L, 128L, 128L}  //240 WHITE
  };

  byte func(unsigned long col, unsigned long fdiv)
  {
    if (col==0) return 0;
    return byte( (col*2-1)*(32-fdiv)/32 );
  }

  byte func2(unsigned long col, unsigned long fdiv)
  {
    if (col==0) return 0;
    return byte( (col*2-1)*(16-fdiv)/16 );
  }

  Palette & Palette::Shade16()
  {
    assert(fCount==256);
    for (int i=0; i<16; i++)
    {
      for (int j=0; j<16; j++)
      {
        fEntries[i*16+j].R() = byte(func(kBase[i].Red,j));
        fEntries[i*16+j].G() = byte(func(kBase[i].Green,j));
        fEntries[i*16+j].B() = byte(func(kBase[i].Blue,j));
      }
    }
    fEntries[255].R() = 0;
    fEntries[255].G() = 0;
    fEntries[255].B() = 0;
    return *this;
  }

  Palette & Palette::RedShade()
  {
    for (int i=0; i<fCount; i++)
    {
      fEntries[i].R() = byte(i*256/fCount);
      fEntries[i].G() = 0;
      fEntries[i].B() = 0;
    }
    return *this;
  }

  Palette & Palette::WhiteShade()
  {
    for (int i=0; i<fCount; i++)
    {
      fEntries[i].R() = byte(i*256/fCount);
      fEntries[i].G() = byte(i*256/fCount);
      fEntries[i].B() = byte(i*256/fCount);
    }
    return *this;
  }

  Palette & Palette::Voxel()
  {
    for (int i=0; i<fCount-2; i++)
    {
      fEntries[i].R() = byte(i*256/fCount);
      fEntries[i].G() = 0;
      fEntries[i].B() = 0;
    }


    fEntries[fCount-6].R() = 200;
    fEntries[fCount-6].G() = 0;
    fEntries[fCount-6].B() = 0;


    fEntries[fCount-5].R() = 150;
    fEntries[fCount-5].G() = 0;
    fEntries[fCount-5].B() = 50;

    fEntries[fCount-4].R() = 100;
    fEntries[fCount-4].G() = 50;
    fEntries[fCount-4].B() = 150;


    fEntries[fCount-3].R() = 50;
    fEntries[fCount-3].G() = 100;
    fEntries[fCount-3].B() = 200;


    fEntries[fCount-2].R() = 0;
    fEntries[fCount-2].G() = 100;
    fEntries[fCount-2].B() = 200;

    fEntries[fCount-1].R() = 0;
    fEntries[fCount-1].G() = 0;
    fEntries[fCount-1].B() = 255;
    return *this;
  }

  Palette & Palette::Fire()
  {
    assert(fCount==256);
    int i;
    for (i = 0; i < fCount; i++) {
      fEntries[i].R() = 0;
      fEntries[i].G() = 0;
      fEntries[i].B() = 0;
    }
    for (i = 64; i < 64+64; i++)
      fEntries[i].R() = (byte)((i-64)*4);
    for (; i < 64+64+64; i++) {
      fEntries[i].R() = 255;
      fEntries[i].G() = (byte)((i-64-64-64)*4);
    }
    for (; i < 64+64+64+64; i++) {
      fEntries[i].R() = 255;
      fEntries[i].G() = 255;
      fEntries[i].B() = (byte)((i-64-64-64-64)*4);
    }
    return *this;
  }

  /*****************************************************************************\
  * CountPalette
  \*****************************************************************************/

  CountPalette::CountPalette(int aCount)
    : inherited(aCount)
  {
    alloc(fEntriesCount = new int[fCount]);
    InitCounts();
  }

  CountPalette::CountPalette(const CountPalette & aPalette)
    : inherited((Palette &)aPalette)
  {
    alloc(fEntriesCount = new int[fCount]);
    for (int i=0; i<fCount; i++)
      fEntriesCount[i] = aPalette.fEntriesCount[i];
  }

  CountPalette::CountPalette(const Palette & aPalette)
    : inherited((Palette &)aPalette)
  {
    alloc(fEntriesCount = new int[fCount]);
    InitCounts();
  }

  CountPalette::~CountPalette()
  {
    delete [] fEntriesCount;
  }

  void CountPalette::InitCounts()
  {
    for (int i=0; i<fCount; i++)
      fEntriesCount[i] = 0;
  }

#ifdef DEBUGOUT
  ostream & CountPalette::DebugOut(ostream & os) const
  {
    os << "CountPalette" << incendl
      << "{" << iendl;
    inherited::DebugOut(os);
    os << "fEntriesCount : " << iendl;
    for (int i=0; i<fCount; i++)
      os << fEntriesCount[i] << " ";
    os << iendl << "}" << decendl;
    return os;
  }
#endif

  CountPalette CountPalette::operator | (const CountPalette & aPalette) const
  {
    CountPalette result(aPalette.fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] | aPalette.fEntries[i];
    return result;
  }

  CountPalette CountPalette::operator + (const CountPalette & aPalette) const
  {
    CountPalette result(aPalette.fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] + aPalette.fEntries[i];
    return result;
  }

  CountPalette CountPalette::operator - (const CountPalette & aPalette) const
  {
    CountPalette result(aPalette.fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] - aPalette.fEntries[i];
    return result;
  }

  CountPalette CountPalette::operator * (const CountPalette & aPalette) const
  {
    CountPalette result(aPalette.fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] ^ aPalette.fEntries[i];
    return result;
  }

  CountPalette CountPalette::operator | (const RGBColor & RGBColor) const
  {
    CountPalette result(fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] | RGBColor;
    return result;
  }

  CountPalette CountPalette::operator + (const RGBColor & RGBColor) const
  {
    CountPalette result(fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] + RGBColor;
    return result;
  }

  CountPalette CountPalette::operator - (const RGBColor & RGBColor) const
  {
    CountPalette result(fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] - RGBColor;
    return result;
  }

  CountPalette CountPalette::operator ^ (const RGBColor & RGBColor) const
  {
    CountPalette result(fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] ^ RGBColor;
    return result;
  }

  CountPalette CountPalette::Transluent(const RGBColor & RGBColor) const
  {
    CountPalette result(fCount);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] = fEntries[i] ^ RGBColor;
    return result;
  }

  CountPalette CountPalette::operator * (real aCoef) const
  {
    CountPalette result(*this);
    for (int i = 0; i<fCount; i++)
      result.fEntries[i] *= aCoef;
    return result;
  }

  CountPalette& CountPalette::operator *= (real aCoef)
  {
    for (int i = 0; i<fCount; i++)
      fEntries[i] *= aCoef;
    return *this;
  }

  CountPalette& CountPalette::operator *= (const RGBColor & RGBColor)
  {
    for (int i = 0; i<fCount; i++)
      fEntries[i] *= RGBColor;
    return *this;
  }

  CountPalette & CountPalette::AddImage(const Image & aImage)
  {
    for (int j=0; j<aImage.Height(); j++)
      for (int i=0; i<aImage.Width(); i++)
        if (aImage.GetPixel(i,j)<fCount)
          fEntriesCount[aImage.GetPixel(i,j)]++;
        else
          error("unknown color");
    return *this;
  }

#ifdef DEBUGOUT
  ostream & operator << (ostream & os, const CountPalette & aPalette)
  {
    return aPalette.DebugOut(os);
  }
#endif

  /*****************************************************************************\
  * PaletteBuilder
  \*****************************************************************************/

  PaletteBuilder::PaletteBuilder(int aCount, int aThreshold)
    : inherited(aCount)
  {
    fThreshold = aThreshold;
  }

  PaletteBuilder::PaletteBuilder(const PaletteBuilder & aPalette, int aThreshold)
    : inherited((CountPalette &)aPalette)
  {
    fThreshold = aThreshold;
  }

  PaletteBuilder::PaletteBuilder(const Palette & aPalette, int aThreshold)
    : inherited((Palette &)aPalette)
  {
    fThreshold = aThreshold;
  }

  // you can change Norm by Abs if you want here
  int PaletteBuilder::Present(const RGBColor & aRGB)
  {
    for (int i=0; i<fCount; i++)
      if ((fEntries[i]-aRGB).Norm()<=fThreshold)
        return i;
    return NOTPRESENT;
  }

  void PaletteBuilder::AddColor(const RGBColor & aRGB, int colorcount)
  {
    int tmp;
    if ((tmp=Present(aRGB)) == NOTPRESENT)
    {
      // search the first unused color and replace it
      int empty;
      for (empty=0; empty<fCount; empty++)
        if (fEntriesCount[empty]==0) break;
      assert(empty!=fCount);
      fEntries[empty] = aRGB;
      fEntriesCount[empty] = colorcount;
    }
    else
      fEntriesCount[tmp] += colorcount;
  }

#ifdef DEBUGOUT
  ostream & PaletteBuilder::DebugOut(ostream & os) const
  {
    os << "PaletteBuilder" << incendl
      << "{" << iendl;
    inherited::DebugOut(os);
    os << "threshold : " << fThreshold;
    os << "}" << decendl;
    return os;
  }
#endif

  // assumes that there is enough empty color
  PaletteBuilder & PaletteBuilder::AddPalette(const CountPalette & aPalette)
  {
    for (int i=0; i<aPalette.fCount; i++)
      AddColor(aPalette.fEntries[i], aPalette.fEntriesCount[i]);
    return *this;
  }

  // !!!! becareful there is no error handling in this function
#define NOTFOUND -1
  Palette PaletteBuilder::Best(int ResultCount) const
  {
    assert(ResultCount > 0);
    Palette result(ResultCount);
    bool * alreadyused;
    alloc(alreadyused = new bool[fCount]);
    for (int k=0; k<fCount; k++)
      alreadyused[k] = false;

    // 1 - find the first ResultCount most used fEntries
    for (int i=0; i<ResultCount; i++)
    {
      // 1 - search the color the most used not marked
      int best = NOTFOUND;
      int max = 0;
      for (int j=0; j<fCount; j++)
        if (!alreadyused[j] && fEntriesCount[j]>max)
        {
          max = fEntriesCount[j];
          best = j;
        }

        // 2 - if has found, mak the color, and add it to the palette.
        if (best!=NOTFOUND)
        {
          result[i] = fEntries[best];
          alreadyused[best]=true;
        }
    }
    delete [] alreadyused;
    return result;
  }

#ifdef DEBUGOUT
  ostream & operator << (ostream & os, const PaletteBuilder & aPalette)
  {
    return aPalette.DebugOut(os);
  }
#endif

}
