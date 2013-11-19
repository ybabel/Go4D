#include "Stereo.h"

/*****************************************************************************\
 *   Stereogram                                                              *
\*****************************************************************************/


Stereogram::Stereogram(const Image & image, int peyedist, real pfocal)
: inherited(image)
{
  eyedist = peyedist;
  focal=pfocal;
  source = new byte [fLen];
  AleaSource();
}

Stereogram::Stereogram(int width, int height, int peyedist, real pfocal)
: inherited(width, height, true)
{
  eyedist = peyedist;
  focal=pfocal;
  source = new byte [fLen];
  AleaSource();
}

Stereogram::~Stereogram()
{
  delete [] source;
}

void Stereogram::AleaSource()
{
  for (int j=0; j<fHeight; j++)
    for (int i=0; i<fWidth; i++)
      *(source+i+j*fWidth) = byte(random(256));
}

void Stereogram::ImageSource(const Image & ima)
{
  for (int j=0; j<fHeight; j++)
    for (int i=0; i<fWidth; i++)
      *(source+i+j*fWidth) = ima.GetPixel(i%ima.Width(), j%ima.Height());
}

Stereogram & Stereogram::operator = (const ZImage &input)
{
  assert( input.Width() == fWidth && input.Height() == fHeight);

  byte * buf;
  alloc(buf = new byte [fWidth+eyedist]);
  int * decal;
  alloc(decal = new int [fWidth]);

  for (int j=0; j<fHeight; j++)
  {
      for (int i0=0; i0<fWidth; i0++)
       	buf[i0] = source[fWidth*j+i0];

      for (int k=0; k<fWidth; k++)
        decal[k]=int((double)eyedist*((1.0-focal/fround(input.GetZ(k,j)))));

      for (int i=0; i<fWidth; i++)
      {
        int ofs=i+decal[i];
        if ( (ofs>=0)&&(ofs<fWidth) )
          buf[i+decal[i]]= buf[i];
      }

      for (int x=0; x<fWidth; x++)
      	Pixel(x,j,buf[x]);
  }
  delete [] buf;
  delete [] decal;
  return *this;
}

void Stereogram::CopyImage(const ZImage & image)
{
  byte * cur=fBuffer;
  byte * curImage = image.GetBuffer();
  for (int i=0; i<fLen; i++, cur++, curImage++)
    *cur = *curImage;
}

