/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : drawer.h                     
 *CREATED: 09/03/96
 *DATE   : 09/03/96
 *OBJECT : drawing algo, in linear 256 color par pixel buffer. 
 *         line shape(flat, gouraud, mapping, QuadMapping), ellipse
 *         box, plot, string write.
 *         differents methods of drawing : normal, zbufferized, sbufferized
 *         support differents real format (look in common.h)
 *DEFS   : usefixed, usefloat
 *	        usefixed or usefloat is necessary to compile
\*****************************************************************************/

#ifndef _DRAWER_H_
#define _DRAWER_H_

#include "image.h"

/*****************************************************************************\
 *POINTZ : all the drawing method take one or more PointZ in parameter.
 *  This class is used to describe a point. It had a z coordinate in case
 *  the zbuffer is activated. All the pixel drawn in the image is tested
 *  if zbuffer is on. The color field is used in most of the method (but 
 *  not all -> Map don't use it) to describe the color that will be used
 *  to display the object
 *
 *MAPPOINT : is used only in method map. When you do a mapping you must 
 *  specifie the coordinate where you map the image, and the image to 
 *  be mapped itself. In fact it is not an entire image that is mapped
 *  but only a shape wich edges are defined by a list of MapPoints.
 *
 *DRAWER : bufferized window, that mean until you don't call Display(), 
 *  all you do is not send to the screen, but in the buffer.
 *
 *Clear : fill all the image with the same color
 *Pixel : put a pixel at the position and the color specified int the PointZ
 *  there is an assertion that control the position of the pixel
 *PutChar : write the char at the required position (PointZ), and clip the
 *  char if it's necessary, no error raised. the description of the char
 *  is stored in the font
 *Box : fill a rectangle, the left-up edge is the PointZ, the dimesions are
 *  w and h. the rectangle is automatically clipped.
 *PutString : write the entire string in the image. the string is clipped.
 *  the string is centered on the position. the position specified is NOT
 *  the upper-left corner of the string.
 *Line : draw a line (bresenham algo) between the 2 points (the line is
 *  gouraud shaded, in the case the color are differents)
 *Fill : fill the shape with a uniform color. the shape MUST be convex. 
 *  (to be exact y-convex, it means that on same line (y constant) all the 
 *  points must be aligned, no rupture.
 *Shade : same thing than Fill, but the color are gouraud shaded between 
 *  the points.
 *Map : same thing than Fill, an image is linearly mapped on the shape
 *QuadMap : same thing than Map but image is quadraticly mapped (it is more
 *  realistic, and not very much slow)
 *Operator= : copie the bitmaps of the entry image, an assertion controls
 *  that the dimensions of the images are equals.
 *Ellipse : Draw an ellipse, the first point is the center, the other are 
 *  the (relative) positions of the axes.
 *ShadeVert : vertical optimized shaded-line drawing (use by voxels)
 *Horiz : use by Fill, draw an horizontal line in the specified color
 *
 *
 *NOTE : this class work with both float and fixed, depending on the define
 *  (usefloat, usefixed)
\*****************************************************************************/

class PointZ
{
  friend Drawer;
  friend ZDrawer;
  friend SDrawer;

  private :
  int x;
  int y;
  real z;
  byte color;
  public :
  PointZ & SetZ(real newz) { z=newz; return *this; }
  PointZ(){x=0; y=0; z=0; color=0;}
  PointZ(int px, int py, byte pc, real pz=0) // Mini = always visible
    {x=px; y=py; z=pz; color=pc;}
  void Decal(int dy) {y+=dy;}
};

struct MapPoint
{
  int x;
  int y;
};

class Drawer : public Image
{
  public :
  Drawer(int width, int height, int border=0, int autoalloc=TRUE);
  virtual ~Drawer();
  virtual void Resize(int newsizex, int newsizey);
  virtual void Clear(byte pColor = 0, realtype pZ=Maxi);
  virtual void Pixel(const PointZ &);
  virtual void ClipPixel(const PointZ &);
  virtual void Box(const PointZ &, int w, int h);
  virtual void PutChar(const PointZ &, const Fonts &, char c);
  void Blur(const Drawer &);
  void BlurAttract(const Drawer &, byte attractor, int coef);
  void Copy(const Drawer &);
  void PutString(const PointZ &, const Fonts &, char * s);
  void WriteString(const PointZ &, const Fonts &, char * s);
  void Line(const PointZ &, const PointZ &);
  void Fill(int NbPlots, PointZ * points, byte color);
  void Shade(int NbPlots, PointZ * points);
  void Map(int NbPlots, PointZ * points, MapPoint * mpoints, const Image * ima);
  void QuadMap(int NbPlots, PointZ * points, MapPoint * mpoints, const Image * ima);
  void HypMap(int NbPlots, PointZ * points, MapPoint * mpoints, const Image * ima);
  Drawer & operator = (const Image &);

  void Ellipse(PointZ center, PointZ xaxe, PointZ yaxe, byte color);
  void FillEllipse(PointZ center, PointZ xaxe, PointZ yaxe, byte color);

  void ShadeVert(int x, int yb, int ye, int cb, int ce);

  protected :
  void LineCoefs(int x1, int y1, int x2, int y2,
		 int &x, int &y, int &dx, int &dy, int &s1, int &s2,
		 int & interchange, int &e, int &l);
  int ClipChar(const PointZ&, const Fonts&,
		int &x, int &y, int &w, int &h, byte *& chr);
  int ClipBox(const PointZ&p, int &x, int &y, int &w, int &h);
  virtual void NoClip(int pXb, int pYb, realtype pZb,
    int pXe, int pYe, realtype pZe,
    realtype pColor1, realtype pColor2);
  virtual void Horiz(realtype xb, realtype xe,
	  realtype zb, realtype ze, int y, byte color);
  virtual void ShadeHoriz(realtype xb, realtype xe, realtype zb, realtype ze,
		int y, realtype cb, realtype ce);
  virtual void MapHoriz(realtype xb, realtype xe, realtype zb, realtype ze,
		int y, realtype mxb, realtype myb,
		realtype mxe, realtype mye,
		const Image *);
  virtual void QuadMapHoriz(realtype xb, realtype xe,
    realtype zb, realtype ze,
    int y, realtype mxb, realtype myb,
    realtype mxe, realtype mye, const Image *);
  virtual void HypMapHoriz(realtype xb, realtype xe,
    realtype zb, realtype ze,
    int y, realtype mxb, realtype myb,
    realtype mxe, realtype mye, const Image *);

  void Ellipse(int CX, int CY, int x1, int y1, int x2, int y2, byte color);
  void FillEllipse(int CX, int CY, int x1, int y1, int x2, int y2, byte color);

  int  xmin, xmax, ymin, ymax;
  int * ellipse_xd, * ellipse_xf;
};

void QuadMapCoefsInit(realtype xb, realtype xe, realtype l, 
		      realtype xd,
		      realtype mxb, realtype mxe,
		      realtype myb, realtype mye,
		      realtype &mx, realtype &my, 
		      realtype &tx, realtype &ty,
		      realtype &ax, realtype &ay);

/*****************************************************************************\
 *Inline methods, and properties
\*****************************************************************************/

inline void Drawer::Pixel(const PointZ & p)
{
  assert((p.x>=xmin)&&(p.x<xmax)&&(p.y>=ymin)&&(p.y<ymax));
  *(fBuffer+p.x+fWidth*p.y) = (byte)p.color;
}

inline void Drawer::ClipPixel(const PointZ & p)
{
  if((p.x>=xmin)&&(p.x<xmax)&&(p.y>=ymin)&&(p.y<ymax))
    *(fBuffer+p.x+fWidth*p.y) = (byte)p.color;
}


#endif
