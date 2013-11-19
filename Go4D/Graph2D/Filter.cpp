/*****************************************************************************\
*AUTHOR : Babel yoann
*TITLE  : Filter.cpp, Filter.h implementation
*CREATED: 27/27/96
*DATE   : 27/27/96
*BUGS   :
\*****************************************************************************/

#include "Filter.h"

namespace Go4D
{

  FilteredImage::FilteredImage(const Image & aImage, const Image & aSource)
    : inherited(aImage)
  {
    assert(aSource.Len() == fLen); // more permissive than testing Width and Height
    fSource = &aSource;
    fBias = 0;
    fFactor = 1;
  }

  FilteredImage::FilteredImage(int aWidth, int aHeight, const Image & aSource)
    : inherited(aWidth, aHeight, true)
  {
    assert(aSource.Len() == fLen); // more permissive than testing Width and Height
    fSource = &aSource;
    fBias = 0;
    fFactor = 1;
  }

#define DPIX(x,y) fSource->GetPixel(x,y)

  FilteredImage & FilteredImage::Process()
  {
    int i;
    assert(fLen == fSource->Len());

    // edges
    fBuffer[0] = Color(real((
      DPIX(0,0)*fFilter(1,1) + DPIX(1,0)*fFilter(2,1) +
      DPIX(0,1)*fFilter(1,2) + DPIX(1,1)*fFilter(2,2)
      )*fFactor*4/9+fBias));
    fBuffer[fWidth-1] = Color(real((
      DPIX(fWidth-2,0)*fFilter(0,1) + DPIX(fWidth-1,0)*fFilter(1,1) +
      DPIX(fWidth-2,1)*fFilter(0,2) + DPIX(fWidth-1,1)*fFilter(1,2)
      )*fFactor*4/9+fBias));
    fBuffer[fWidth*(fHeight-1)] = Color(real((
      DPIX(0,fHeight-2)*fFilter(1,0) + DPIX(1,fHeight-2)*fFilter(2,0) +
      DPIX(0,fHeight-1)*fFilter(1,1) + DPIX(1,fHeight-1)*fFilter(2,1)
      )*fFactor*4/9+fBias));
    fBuffer[fWidth*(fHeight)-1] = Color(real((
      DPIX(fWidth-2,fHeight-2)*fFilter(0,0) + DPIX(fWidth-1,fHeight-2)*fFilter(1,0) +
      DPIX(fWidth-2,fHeight-1)*fFilter(0,1) + DPIX(fWidth-1,fHeight-1)*fFilter(1,1)
      )*fFactor*4/9+fBias));

    // sides
    for (i=1; i<fWidth-1; i++)
    {
      fBuffer[fWidth*(fHeight-1)+i] = Color(real((
        DPIX(i,fHeight-2)*fFilter(0,0) + DPIX(i-1,fHeight-2)*fFilter(1,0) + DPIX(i+1,fHeight-2)*fFilter(2,0)+
        DPIX(i,fHeight-1)*fFilter(0,1) + DPIX(i-1,fHeight-1)*fFilter(1,1) + DPIX(i+1,fHeight-1)*fFilter(2,1)
        )*fFactor*6/9+fBias));
      fBuffer[i] = Color(real((
        DPIX(i,0)*fFilter(0,1) + DPIX(i-1,0)*fFilter(1,1) + DPIX(i+1,0)*fFilter(2,1)+
        DPIX(i,1)*fFilter(0,2) + DPIX(i-1,1)*fFilter(1,2) + DPIX(i+1,1)*fFilter(2,2)
        )*fFactor*6/9+fBias));
    }
    for (i=1; i<fHeight-1; i++)
    {
      fBuffer[fWidth*i] = Color(real((
        DPIX(0,i)*fFilter(1,0)   + DPIX(1,i)*fFilter(2,0)+
        DPIX(0,i-1)*fFilter(1,1) + DPIX(1,i-1)*fFilter(2,1)+
        DPIX(0,i+1)*fFilter(1,2) + DPIX(1,i+1)*fFilter(2,2)
        )*fFactor*6/9 + fBias));
      fBuffer[fWidth-1+fWidth*i] = Color(real((
        DPIX(fWidth-2,i)*fFilter(0,0)   + DPIX(fWidth-1,i)*fFilter(1,0)+
        DPIX(fWidth-2,i-1)*fFilter(0,1) + DPIX(fWidth-1,i-1)*fFilter(1,1)+
        DPIX(fWidth-2,i+1)*fFilter(0,2) + DPIX(fWidth-1,i+1)*fFilter(1,2)
        )*fFactor*6/9+fBias));
    }

    // all the others
    Color * src=fBuffer+fWidth+1, * dst=fSource->GetBuffer()+fWidth+1;

    for (int y=1; y<fHeight-1; y++)
    {
      for (int x=1; x<fWidth-1; x++)
      {
        *src = Color(real((
          *(dst-fWidth-1)*fFilter(0,0)+*(dst-fWidth)*fFilter(1,0)+*(dst-fWidth+1)*fFilter(2,0)+
          *(dst-1)*fFilter(0,1)+*(dst)*fFilter(1,1)+*(dst+1)*fFilter(2,1)+
          *(dst+fWidth-1)*fFilter(0,2)+*(dst+fWidth)*fFilter(1,2)+*(dst+fWidth+1)*fFilter(2,2)
          )*fFactor+fBias));
        src++;
        dst++;
      }
      src+=2;
      dst+=2;
    }

    return *this;
  }

  FilteredImage & FilteredImage::BlurMotion()
  {
    // to be added later
    return *this;
  }

  FilteredImage & FilteredImage::Emboss()
  {
    int i;
    assert(fLen == fSource->Len());

    // edges
    fBuffer[0] = Color(real((
      DPIX(0,0)*fFilter(1,1) + DPIX(1,0)*fFilter(2,1) +
      DPIX(0,1)*fFilter(1,2) + DPIX(1,1)*fFilter(2,2)
      )*fFactor*4/9+fBias));
    fBuffer[fWidth-1] = Color(real((
      DPIX(fWidth-2,0)*fFilter(0,1) + DPIX(fWidth-1,0)*fFilter(1,1) +
      DPIX(fWidth-2,1)*fFilter(0,2) + DPIX(fWidth-1,1)*fFilter(1,2)
      )*fFactor*4/9+fBias));
    fBuffer[fWidth*(fHeight-1)] = Color(real((
      DPIX(0,fHeight-2)*fFilter(1,0) + DPIX(1,fHeight-2)*fFilter(2,0) +
      DPIX(0,fHeight-1)*fFilter(1,1) + DPIX(1,fHeight-1)*fFilter(2,1)
      )*fFactor*4/9+fBias));
    fBuffer[fWidth*(fHeight)-1] = Color(real((
      DPIX(fWidth-2,fHeight-2)*fFilter(0,0) + DPIX(fWidth-1,fHeight-2)*fFilter(1,0) +
      DPIX(fWidth-2,fHeight-1)*fFilter(0,1) + DPIX(fWidth-1,fHeight-1)*fFilter(1,1)
      )*fFactor*4/9+fBias));

    // sides
    for (i=1; i<fWidth-1; i++)
    {
      fBuffer[fWidth*(fHeight-1)+i] = Color(real((
        DPIX(i,fHeight-2)*fFilter(0,0) + DPIX(i-1,fHeight-2)*fFilter(1,0) + DPIX(i+1,fHeight-2)*fFilter(2,0)+
        DPIX(i,fHeight-1)*fFilter(0,1) + DPIX(i-1,fHeight-1)*fFilter(1,1) + DPIX(i+1,fHeight-1)*fFilter(2,1)
        )*fFactor*6/9+fBias));
      fBuffer[i] = Color(real((
        DPIX(i,0)*fFilter(0,1) + DPIX(i-1,0)*fFilter(1,1) + DPIX(i+1,0)*fFilter(2,1)+
        DPIX(i,1)*fFilter(0,2) + DPIX(i-1,1)*fFilter(1,2) + DPIX(i+1,1)*fFilter(2,2)
        )*fFactor*6/9+fBias));
    }
    for (i=1; i<fHeight-1; i++)
    {
      fBuffer[fWidth*i] = Color(real((
        DPIX(0,i)*fFilter(1,0)   + DPIX(1,i)*fFilter(2,0)+
        DPIX(0,i-1)*fFilter(1,1) + DPIX(1,i-1)*fFilter(2,1)+
        DPIX(0,i+1)*fFilter(1,2) + DPIX(1,i+1)*fFilter(2,2)
        )*fFactor*6/9 + fBias));
      fBuffer[fWidth-1+fWidth*i] = Color(real((
        DPIX(fWidth-2,i)*fFilter(0,0)   + DPIX(fWidth-1,i)*fFilter(1,0)+
        DPIX(fWidth-2,i-1)*fFilter(0,1) + DPIX(fWidth-1,i-1)*fFilter(1,1)+
        DPIX(fWidth-2,i+1)*fFilter(0,2) + DPIX(fWidth-1,i+1)*fFilter(1,2)
        )*fFactor*6/9+fBias));
    }

    // all the others
    Color * src=fBuffer+fWidth+1, * dst=fSource->GetBuffer()+fWidth+1;

#define PREC 65536
#define FPREC 256.0
    int f10 = int(fFilter(1,0)*FPREC);
    int f01 = int(fFilter(0,1)*FPREC);
    int f21 = int(fFilter(2,1)*FPREC);
    int f12 = int(fFilter(1,2)*FPREC);
    Color bias = Color(fBias);
    int factor = int(fFactor);
    for (int y=1; y<fHeight-1; y++)
    {
      for (int x=1; x<fWidth-1; x++)
      {
        *src = Color(int((
          *(dst-fWidth)*f10+
          *(dst-1)*f01+*(dst+1)*f21+
          *(dst+fWidth)*f12
          )*factor)/PREC+bias);
        src++;
        dst++;
      }
      src+=2;
      dst+=2;
    }

    return *this;
  }

}
