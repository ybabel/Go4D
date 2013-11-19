#include "Convert.h"

namespace Go4D
{

  /******************************************************************************\
  * Converter
  \******************************************************************************/

  Converter::Converter(const Image & image)
    : inherited(image)
  {
  }

  Converter::Converter(int width, int height)
    : inherited(width, height, true)
  {
  }

  void Converter::FromHeightField(const HeightField & field, real coef, real base)
  {
    for (int x=0; x<fWidth; x++)
      for (int y=0; y<fHeight; y++)
      {
        int fieldX = (x*field.Width())/fWidth;
        int fieldY = (y*field.Height())/fHeight;
        byte color = byte(field.GetHeight(fieldX, fieldY)*coef+base);
        Pixel(x, y, color);
      }
  }

  void Converter::ToHeightField(HeightField & field, real amin, real amax)
  {
    for (int x=0; x<field.Width(); x++)
      for (int y=0; y<field.Height(); y++)
      {
        int fieldX = (x*fWidth)/field.Width();
        int fieldY = (y*fHeight)/field.Height();
        real h = real(GetPixel(fieldX, fieldY))*(amax-amin)/256.0+amin;
        field.SetHeight(x, y, h);
      }
  }

  void Converter::Copy(const Image & image)
  {
    assert(fLen == image.Len());
    byte * src = image.GetBuffer();
    byte * dst = fBuffer;
    for (int i=0; i<fLen; i++)
    {
      *dst = *src;
      dst++; src++;
    }
  }

  void Converter::Scale(const Image & image)
  {
    for (int x=0; x<fWidth; x++)
      for (int y=0; y<fHeight; y++)
      {
        int iX = (x*image.Width())/fWidth;
        int iY = (y*image.Height())/fHeight;
        Pixel(x, y, image.GetPixel(iX, iY));
      }
  }

  /******************************************************************************\
  * Blurer
  \******************************************************************************/

  Blurer::Blurer(const Image &image)
    : inherited(image)
  {
  }

  Blurer::Blurer(int width, int height)
    : inherited(width, height, true)
  {
  }

#define PIX(i) image.GetBuffer()[i]
#define AVRG4(a,b,c,d) byte((int)(PIX(a)+PIX(b)+PIX(c)+PIX(d))/4)
#define DPIX(x,y) image.GetBuffer()[x+(y)*fWidth]
  void Blurer::Blur(const Image & image)
  {
    int i;
    assert(fLen == image.Len());

    // edges
    fBuffer[0] = AVRG4(0,1,fWidth,fWidth+1);
    fBuffer[fWidth-1] = AVRG4(fWidth-1, fWidth-2, 2*fWidth-1, 2*fWidth-2);
    fBuffer[fWidth*(fHeight-1)] =
      AVRG4(fWidth*(fHeight-1),fWidth*(fHeight-1)+1,
      fWidth*(fHeight-2),fWidth*(fHeight-2)+1);
    fBuffer[fWidth*(fHeight)-1] =
      AVRG4(fWidth*(fHeight)-1,fWidth*(fHeight)-2,
      fWidth*(fHeight-1)-1,fWidth*(fHeight-1)-2);

    // sides
    for (i=1; i<fWidth-1; i++)
    {
      fBuffer[i] = byte(int((
        DPIX(i,0)+DPIX(i-1,0)+DPIX(i+1,0)+
        DPIX(i,1)+DPIX(i-1,1)+DPIX(i+1,1))/6));
      fBuffer[fWidth*(fHeight-1)+i] = byte(int((
        DPIX(i,fHeight-1)+DPIX(i-1,fHeight-1)+DPIX(i+1,fHeight-1)+
        DPIX(i,fHeight-2)+DPIX(i-1,fHeight-2)+DPIX(i+1,fHeight-2))/6));
    }
    for (i=1; i<fHeight-1; i++)
    {
      fBuffer[fWidth*i] = byte(int((
        DPIX(0,i)+DPIX(0,i-1)+DPIX(0,i+1)+
        DPIX(1,i)+DPIX(1,i-1)+DPIX(1,i+1))/6));
      fBuffer[fWidth-1+fWidth*i] = byte(int((
        DPIX(fWidth-1,i)+DPIX(fWidth-1,i-1)+DPIX(fWidth-1,i+1)+
        DPIX(fWidth-2,i)+DPIX(fWidth-2,i-1)+DPIX(fWidth-2,i+1))/6));
    }

    // all the others
    byte * src=fBuffer+fWidth+1, * dst=image.GetBuffer()+fWidth+1;

    for (int x=1; x<fWidth-1; x++)
    {
      for (int y=1; y<fHeight-1; y++)
      {
        /*
        *src = byte(int((
        *(dst-fWidth-1)+*(dst-fWidth)+*(dst-fWidth+1)+
        *(dst-1)+*(dst)+*(dst+1)+
        *(dst+fWidth-1)+*(dst+fWidth)+*(dst+fWidth+1))/9));
        */
        *src = byte(int((
          *(dst-fWidth-1)+*(dst-fWidth)+*(dst-fWidth+1)+
          *(dst-1)+*(dst+1)+
          *(dst+fWidth-1)+*(dst+fWidth)+*(dst+fWidth+1)) >> 3));
        src++;
        dst++;
      }
      src+=2;
      dst+=2;
    }
  }


}
