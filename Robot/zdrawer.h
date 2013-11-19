/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : zdrawer.h
 *CREATED: 09/03/96
 *DATE   : 09/03/96
 *OBJECT : zbufferized drawing
\*****************************************************************************/

#ifndef _ZDRAWER_H_
#define _ZDRAWER_H_

#include "drawer.h"

/*****************************************************************************\
 *ZDRAWER : same thing than drawer, but z-buffer is added, to all the routines
 *  ellipse and shadevert are not z-bufferrized
 *fZBuffer : the z-coordinate map for each pixel of the screen
\*****************************************************************************/


class ZDrawer : public Drawer
{
  public :
  ZDrawer(int width, int height, int border=0, int autoalloc=TRUE);
  virtual ~ZDrawer();
  void Resize(int newsizex, int newsizey);
  void Clear(byte pColor = 0, realtype pZ = Maxi);
  void ClearZ(realtype pZ = Maxi); 
  realtype GetZ(int x, int y) const;
  void Pixel(const PointZ &);
  void ClipPixel(const PointZ &);
  void Box(const PointZ &, int w, int h);
  void PutChar(const PointZ &, const Fonts &, char c);
  void Copy(const ZDrawer &);

  protected :
  void NoClip(int pXb, int pYb, realtype pZb,
    int pXe, int pYe, realtype pZe,
    realtype pColor1, realtype pColor2);
  void Horiz(realtype xb, realtype xe,
    realtype zb, realtype ze, int y, byte color);
  void ShadeHoriz(realtype xb, realtype xe, realtype zb, realtype ze,
    int y, realtype cb, realtype ce);
  void MapHoriz(realtype xb, realtype xe,
    realtype zb, realtype ze, int y,
    realtype mxb, realtype myb,
    realtype mxe, realtype mye,
    const Image *);
  void QuadMapHoriz(realtype xb, realtype xe,
    realtype zb, realtype ze,
    int y, realtype mxb, realtype myb,
    realtype mxe, realtype mye, const Image *);
  void HypMapHoriz(realtype xb, realtype xe,
    realtype zb, realtype ze,
    int y, realtype mxb, realtype myb,
    realtype mxe, realtype mye, const Image *);

  realtype * fZBuffer;
};

/*****************************************************************************\
 *STEREOGRAM : give it a zbufferized image, and it will build the
 *  correspondant stereogram
\*****************************************************************************/

class Stereogram : public Image
{
  public :
  Stereogram(int width, int height, int peyedist, real pfocal);
  ~Stereogram();
  Stereogram & operator = (const ZDrawer & );
  void AleaSource();
  void ImageSource(const Image &);
  int eyedist;
  real focal;
  
  private :
  void Pixel(int x, int y, byte color);
  byte * source;
};


/*****************************************************************************\
 *Inline methods, and properties
\*****************************************************************************/

inline void ZDrawer::Pixel(const PointZ & p)
{
  assert((p.x>=xmin)&&(p.x<xmax)&&(p.y>=ymin)&&(p.y<ymax));
  if ( (fconv(p.z)<*(fZBuffer+p.x+fWidth*p.y)) )
    {
      *(fBuffer+p.x+fWidth*p.y) = p.color;
      *(fZBuffer+p.x+fWidth*p.y) = fconv(p.z);
    }
}

inline void ZDrawer::ClipPixel(const PointZ & p)
{
  if ((p.x>=xmin)&&(p.x<xmax)&&(p.y>=ymin)&&(p.y<ymax))
    if ( (fconv(p.z)<*(fZBuffer+p.x+fWidth*p.y)) )
      {
      	*(fBuffer+p.x+fWidth*p.y) = p.color;
      	*(fZBuffer+p.x+fWidth*p.y) = fconv(p.z);
      }
}

inline realtype ZDrawer::GetZ(int x, int y) const
{
  assert((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight));
  return *(fZBuffer+x+fWidth*y);
}

inline void Stereogram::Pixel(int x, int y, byte color)
{
  assert((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight));
  *(fBuffer+x+fWidth*y) = color;
}


#endif
