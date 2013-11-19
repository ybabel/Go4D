#ifndef _DRAWER2D_H_
#define _DRAWER2D_H_

/**
*AUTHOR : Babel yoann
*TITLE  : drawer2D.h, basic images operations
*CREATED: 19/05/99
*DATE   : 19/05/99
*OBJECT : This unit does the same thing dans Drawer.h, but the principle is
*         less generic since it can only draw on Images, it canot support
*         ZImages. This unit is kept for the moment to provide backward
*         compatibility with old programs. In the future it should not be
*         used any more. Use Drawer.h instead. If an algorithm is not
*         implemented in Drawer2D, it would be best to implement it in a
*         template Drawer (in order that it can work with ZImages...)
*/

#include "PlugDrawer.h"

namespace Go4D
{

  /**
  * The drawer 2D is a set of methods that allow to draw various 2D graphics on an image
  * you can draw polygons, and ellipse and other stuff like that
  */
  class Drawer2D : public PluggedDrawer
  {
    typedef PluggedDrawer inherited;

  public :
    ///  The drawer is automatically plugged on the image
    Drawer2D(const Image &, int width, int height, bool circlecaps=false, int border=0);
    ///  the drawer is not plugged, a new image can be allocated, depending on autoalloc value
    Drawer2D(int width, int height, bool AutoAlloc=true, bool circlecaps=false, int border=0);
    /// 
    virtual ~Drawer2D();
    ///  Plug the drawer on an image, the previous buffer is eliminated (destroyed if it
    ///  was owned
    void Plug(const Image & image);
    ///  copy an image
    void Copy(const Image &);
    ///  draw a pixel
    void Pixel(const PointZ &);
    ///  draw a pixel, but check if in the clip zone
    void ClipPixel(const PointZ &);
    ///  draw a box (set Z values ?)
    void BoxZ(const PointZ &, int w, int h);
    ///  fast drawing of a box
    void FastBox(int xmin, int ymin, int xmax, int ymax, Color color);
    ///  draw a line
    void Line(const PointZ &, const PointZ &);
    ///  draw a line, simple parameters
    void Line(int x1, int y1, int x2, int y2, Color color);
    ///  draw a circle
    void Circle(PointZ center, int radius);
    ///  draw a filled circle
    void FillCircle(PointZ center, int radius);
    ///  draw an ellipse
    void Ellipse(PointZ center, PointZ xaxe, PointZ yaxe);
    ///  draw a filled ellipse
    void FillEllipse(PointZ center, PointZ xaxe, PointZ yaxe);
    ///  draw a vertical line and the color is not constant but it's linearly incremented
    void ShadeVert(int x, int yb, int ye, int cb, int ce);
    ///  fill a polygon with 4 edges
    void Fill4(int x1, int y1, int x2, int y2,
      int x3, int y3, int x4, int y4, int color);
    ///  linear mapping of an image with 4 edges
    void Map4(Image&, int sx1, int sy1, int sx2, int sy2,
      int sx3, int sy3, int sx4, int sy4,
      int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
    ///  idem than Map4, but it's a simplified version
    void MapBox(Image&, int sx1, int sy1, int sx2, int sy2,
      int x1, int y1, int x2, int y2);
    ///  only map a line
    void MapHoriz(int xb, int xf, int y,
      int sxb, int syb, int sxe, int sye, Image&);
    ///  apply a filter on the image
    void Blur(const Image &);
    ///  apply an other filter on the image
    void BlurAttract(const Image &, Color attractor, int coef);

  protected :
    bool ClipBox(const PointZ&p, int &x, int &y, int &w, int &h);
    void LineCoefs(int x1, int y1, int x2, int y2,
      int &x, int &y, int &dx, int &dy, int &s1, int &s2,
      int &interchange, int &e, int &l);
    void NoClip(int pXb, int pYb, realtype pZb,
      int pXe, int pYe, realtype pZe,
      realtype pColor1, realtype pColor2);

    void Ellipse(int CX, int CY, int x1, int y1, int x2, int y2, Color color);
    void FillEllipse(int CX, int CY, int x1, int y1, int x2, int y2, Color color);
    void Horiz(int xb, int xf, int y, int color);
    inline void EllipsePixel(int x, int y);

    bool fCircleCaps;
    int fBorder;
    int * ellipse_xd, * ellipse_xf;
  };

  /*****************************************************************************\
  *Inline methods, and properties
  \*****************************************************************************/

  inline void Drawer2D::Pixel(const PointZ & p)
  {
    assert((p.x>=xmin)&&(p.x<xmax)&&(p.y>=ymin)&&(p.y<ymax));
    *(fBuffer+p.x+fWidth*p.y) = p.color;
  }

  inline void Drawer2D::ClipPixel(const PointZ & p)
  {
    if((p.x>=xmin)&&(p.x<xmax)&&(p.y>=ymin)&&(p.y<ymax))
      *(fBuffer+p.x+fWidth*p.y) = p.color;
  }

}

#endif
