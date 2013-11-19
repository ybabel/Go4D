/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : zdrawer.cpp, zdrawer.h implementation
 *CREATED: 09/03/96
 *DATE   : 09/03/96
 *OBJECT : zbufferized drawing
\*****************************************************************************/

#include <string.h>
#include "zdrawer.h"


/*****************************************************************************\
 *  ZDrawer                                                                  *
\*****************************************************************************/


ZDrawer::ZDrawer(int pWidth, int pHeight, int pBorder, int autoalloc)
:Drawer(pWidth,pHeight,pBorder, autoalloc)
{
  alloc(fZBuffer = new realtype[fLen]);
}

ZDrawer::~ZDrawer()
{
  delete [] fZBuffer;
}

void ZDrawer::Resize(int newsizex, int newsizey)
{
  Drawer::Resize(newsizex, newsizey);
  delete [] fZBuffer;
  alloc(fZBuffer = new realtype[fLen]);
}

void ZDrawer::Clear(byte pColor, realtype z)
{
  for (int j=0; j<fLen; j++)
    fZBuffer[j] = z;
  memset(fBuffer, pColor, fLen);
}

void ZDrawer::ClearZ(realtype z)
{
  for (int j=0; j<fLen; j++)
    fZBuffer[j] = z;
}

void ZDrawer::Copy(const ZDrawer & zd)
{
  memcpy(fBuffer, zd.fBuffer, fLen);
  memcpy(fZBuffer, zd.fZBuffer, fLen*sizeof(realtype));
}

void ZDrawer::PutChar(const PointZ &p, const Fonts & f, char c)
{
  byte color = p.color;
  int x,y,w,h;
  byte * chr = f[c];
  if (ClipChar(p,f,x,y,w,h,chr)) return;
  byte * ptr = fBuffer+x+fWidth*y;

  realtype * zptr = fZBuffer+x+fWidth*y;
  realtype z = fconv(p.z);
  for (int j=0; j<h; j++)
    {
      for (int i=0; i<w; i++, ptr++, zptr++, chr++)
      	if ( (*chr!=0) && (z<*zptr) )
       	  {
      	    *ptr = *chr&color;
      	    *zptr = z;
      	  }
      ptr += fWidth-w;
      zptr += fWidth-w;
      chr += f.GetDimX()-w;
    }
}

void ZDrawer::Box(const PointZ & p, int w, int h)
{
  byte color = p.color;
  int x, y;
  if (ClipBox(p,x,y,w,h)) return;
  byte * ptr = fBuffer+x+fWidth*y;

  realtype * zptr = fZBuffer+x+fWidth*y;
  realtype z = fconv(p.z);
  for (int j=0; j<h; j++)
    {
      for (int i=0; i<w; i++, ptr++, zptr++)
      	if (z<*zptr)
      	  {
      	    *ptr = color;
      	    *zptr = z;
      	  }
      ptr += fWidth-w;
      zptr += fWidth-w;
    }
}

void ZDrawer::NoClip(int x1,  int y1, realtype z1,
		    int x2,  int y2, realtype z2,
		    realtype c1, realtype c2)
{
  // bresenham algorithm
  assert( (x1>=0)&&(x1<=fWidth)&&(y1>=0)&&(y1<=fHeight) );
  assert( (x2>=0)&&(x2<=fWidth)&&(y2>=0)&&(y2<=fHeight) );

  int x,y,dx,dy,s1,s2,interchange,e,l;
  LineCoefs(x1,y1,x2,y2,x,y,dx,dy,s1,s2,interchange,e,l);
  if (dx == 0) return;
  
  byte * ptr = fBuffer+x+y*fWidth;
  realtype * zptr = fZBuffer+x+y*fWidth;

  realtype z = z1;
  realtype iz = frapport( (z2-z1), conv(dx) );
  realtype c = c1;
  realtype ic = frapport( (c2-c1), conv(dx) );

  if (interchange == 1)
    for (int i=0; i<dx; i++)
      {
      	if (z<*zptr) {*ptr = byte(round(c)); *zptr=z;}
       	if (e>=0) { ptr += s1; zptr += s1; e = e-2*dx; }
      	ptr += l; zptr += l; e = e+2*dy;
      	c += ic;
      	z += iz;
      }
  else
    for (int i=0; i<dx; i++)
      {
      	if (z<*zptr) {*ptr = byte(round(c)); *zptr=z;}
      	if (e>=0) { ptr += l; zptr += l; e = e-2*dx; }
      	ptr += s1; zptr += s1; e = e+2*dy;
      	c += ic;
      	z += iz;
      }
}


#define zbuffer

void ZDrawer::Horiz(realtype xb, realtype xe, 
	realtype zb, realtype ze, int y,byte color)
{
  #define flat
  #include "cliph.cpp"
  #include "horiz.cpp"
  #undef flat
}

void ZDrawer::ShadeHoriz(realtype xb, realtype xe, realtype zb, realtype ze,
	int y, realtype cb, realtype ce)
{
  #define gouraud
  #include "cliph.cpp"
  #include "horiz.cpp"
  #undef gouraud
}

void ZDrawer::MapHoriz(realtype xb, realtype xe, realtype zb, realtype ze,
  int y, realtype mxb, realtype myb,
	realtype mxe, realtype mye, const Image * ima)
{
  #define mapping
  #include "cliph.cpp"
  #include "horiz.cpp"
  #undef mapping
}

void ZDrawer::QuadMapHoriz(realtype xb,realtype xe,realtype zb, realtype ze,
	int y, realtype mxb, realtype myb,
	realtype mxe, realtype mye,
	const Image * ima)
{
  #define quadmapping
  #include "cliph.cpp"
  #include "horiz.cpp"
  #undef quadmapping
}

void ZDrawer::HypMapHoriz(realtype xb,realtype xe,realtype zb, realtype ze,
	int y, realtype mxb, realtype myb,
	realtype mxe, realtype mye,
	const Image * ima)
{
  #define hypmapping
  #include "cliph.cpp"
  #include "horiz.cpp"
  #undef hypmapping
}

#undef zbuffer

/*****************************************************************************\
 *   Stereogram                                                              *
\*****************************************************************************/


Stereogram::Stereogram(int width, int height, int peyedist, real pfocal)
: Image(width, height)
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
      *(source+i+j*fWidth) = byte(random(16));
}

void Stereogram::ImageSource(const Image & ima)
{
  for (int j=0; j<fHeight; j++)
    for (int i=0; i<fWidth; i++)
      *(source+i+j*fWidth) = ima.GetPixel(i%ima.Width(), j%ima.Height());      
}

Stereogram & Stereogram::operator = (const ZDrawer &input)
{
  assert( input.Width() == fWidth && input.Height() == fHeight);

  byte * buf;
  alloc(buf = new byte [fWidth+eyedist]);
  int * decal;
  alloc(decal = new int [fWidth]);

  for (int j=0; j<fHeight; j++)
    {
      for (int i0=0; i0<fWidth; i0++)
      	{
       	  buf[i0] = source[fWidth*j+i0];
       	}

      for (int k=0; k<fWidth; k++)
        {
        	decal[k]=int((double)eyedist*((1.0-focal/roundf(input.GetZ(k,j)))));
        }

      for (int i=0; i<fWidth; i++)
        {
        	buf[i+decal[i]]= buf[i];
        }

      for (int x=0; x<fWidth; x++)
      	Pixel(x,j,buf[x]);
    }
  delete [] buf;
  delete [] decal;
  return *this;
}
