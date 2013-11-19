#ifndef _DRAWER_H_
#define _DRAWER_H_

/*
*AUTHOR : Babel yoann
*TITLE  : drawer.h, basic drawing algorithms on images
*CREATED: 15/05/99
*DATE   : 15/05/99
*OBJECT : Drawing of lines, ellipse and boxes on images. With use of
*         templates, the drawing can work on Images or ZImages (images+ZBuffer)
*         Basically the drawer work on Images, but it can be easily extended
*         to draw on ZBuffer or Object Buffer (an integer is draw in a buffer
*         that identifies the last object draw, this is useful to select
*         objects drawed).
*/

#include "PlugDrawer.h"

namespace Go4D
{

  /** The pixeler is used by the dawer to put individual pixel into an image.
  * This class provide a fast way (with inline method), and a direct access to
  * images.
  */
  class Pixeler
  {
  public :
    /// constructor 
    Pixeler() {};
    ///  the image in which the drawing are done
    void SetImage(const Image & image)
    {
      fImage=&image;
      fStartPixel = fImage->GetBuffer();
      fWidth = fImage->Width();
    }
    ///  used when a list of pixel are drawed, select the first pixel
    inline void SetStart(const PointZ & start)
    {
      fCurPixel=fStartPixel+start.x+start.y*fWidth;
      color = start.color;
    }
    ///  used when a list of pixel are drawed, select the last pixel
    inline void SetEnd(const PointZ & end) {}
    ///  set the number of pixel drawed between start and end (use to process some internal increments)
    inline void SetSteps(int) {}
    ///  go to the next pixel
    inline void Next() { fCurPixel++; }
    ///  skip L pixels
    inline void NextL(int L) { fCurPixel+=L; }
    ///  draw the current pixel (according to StartPixel and EndPixel
    inline void Pixel()
    {
      assert(fCurPixel-fStartPixel>=0);
      assert(fCurPixel-fStartPixel<fImage->Width()*fImage->Height());
      *fCurPixel=color;
    }
    ///  draw a pixel at specific position
    inline void PixelAt(int x, int y, realtype z)
    {
      *(fStartPixel+x+y*fWidth)=color;
    }

  protected :
    const Image * fImage;
    int fWidth;
    Color color;
    Color * fStartPixel;
    Color * fCurPixel;
  };

  /** This template class use a pixeled to draw lines/circles/... in an image.
  * The drawing can be adapted to ZBuffer for example.
  */
  template <class Pixeler> class Drawer : public PluggedDrawer
  {
    typedef PluggedDrawer inherited;
  public :
    ///  draw in a particular image, the drawing is limited to a square area wich size is width-border, height-border
    Drawer(const Image & aImage, int pBorder=0)
      : inherited(aImage)
    {
      assert(pBorder*2 < fWidth);
      assert(pBorder*2 < fHeight);
      fBorder = pBorder;
      xmin = fBorder;
      ymin = fBorder;
      xmax = fWidth-fBorder;
      ymax = fHeight-fBorder;
      ellipse_xd = new int[fHeight];
      ellipse_xf = new int[fHeight];
    }
    Drawer(int pWidth, int pHeight, bool AutoAlloc=true, int pBorder=0)
      : inherited(pWidth, pHeight, AutoAlloc)
    {
      if (AutoAlloc)
        fPixeler.SetImage(*this);
      assert( (pWidth>0 && pHeight>0 && pBorder*2 < fWidth && pBorder*2 < fHeight) || (!AutoAlloc));
      fBorder = pBorder;
      xmin = fBorder;
      ymin = fBorder;
      xmax = fWidth-fBorder;
      ymax = fHeight-fBorder;
      ellipse_xd = new int[fHeight];
      ellipse_xf = new int[fHeight];
    }
    /// 
    ~Drawer()
    {
      delete [] ellipse_xd;
      delete [] ellipse_xf;
    }
    ///  change the image where the drawing is done
    void Plug(const Image & image)
    {
      inherited::Plug(image);
      fPixeler.SetImage(image);
      xmin = fBorder;
      ymin = fBorder;
      xmax = fWidth-fBorder;
      ymax = fHeight-fBorder;
    }
    ///  copy an image into an other
    void Copy(const Image & ima)
    {
      assert(fLen == ima.Len());
      byte * imaBuf = ima.GetBuffer();
      for (int i=0; i<fLen; i++)
        fBuffer[i] =imaBuf[i];
    }
    ///  draw a square box in the image
    void Box(const PointZ & p, int w, int h)
    {
      int x, y;
      if (ClipBox(p,x,y,w,h)) return;
      fPixeler.SetStart(PointZ(x,y, p.color, p.z));
      fPixeler.SetEnd(PointZ(x+w,y+h, p.color, p.z));
      fPixeler.SetSteps(h);
      for (int j=0; j<h; j++)
      {
        for (int i=0; i<w; i++, fPixeler.Next())
          fPixeler.Pixel();
        fPixeler.NextL(fWidth-w);
      }
    }
    /// fast box drawing, without Z buffering at all
    void FastBox(int xmin, int ymin, int xmax, int ymax, Color color)
    {
      int x=xmin, y=ymin;
      Color * ptr = fBuffer+x+fWidth*y;
      int h = ymax-ymin;
      int w = xmax-xmin;

      for (int j=0; j<h; j++)
      {
        for (int i=0; i<w; i++, ptr++) *ptr = color;
        ptr += fWidth-w;
      }
    }

    ///  draw a line 
    void Line(const PointZ &p1, const PointZ & p2)
    {
      int x1; int y1; real z1; realtype color1;
      int x2; int y2; real z2; realtype color2;
      if (ClipLine(p1, p2, x1, y1, z1, x2, y2, z2, color1, color2))
        NoClipLine(x1, y1, z1, x2, y2, z2, color1, color2);
    }
    ///  draw an ellipse
    void Ellipse(PointZ center, PointZ xaxe, PointZ yaxe)
    {
      Ellipse(center.x, center.y,
        xaxe.x-center.x, xaxe.y-center.y,
        yaxe.x-center.x, yaxe.y-center.y,
        center.color, center.z, false);
    }
    ///  draw an filled ellipse
    void FillEllipse(PointZ center, PointZ xaxe, PointZ yaxe)
    {
      Ellipse(center.x, center.y,
        xaxe.x-center.x, xaxe.y-center.y,
        yaxe.x-center.x, yaxe.y-center.y,
        center.color, center.z, true);
    }
    ///  draw a cirle
    void Circle(PointZ center, int radius)
    {
      Ellipse(center.x, center.y,
        radius, 0,
        0, radius,
        center.color, center.z, false);
    }
    ///  draw a filled cirle
    void FillCircle(PointZ center, int radius)
    {
      Ellipse(center.x, center.y,
        0, radius,
        radius, 0,
        center.color, center.z, true);
    }

  protected :

    struct regionc
    {
      int gauche : 1;
      int droite : 1;
      int basse  : 1;
      int haute  : 1;
    };

    union region
    {
      int i;
      regionc r;
    };

    region codebinaire( int x,  int y, int xmin, int ymin, int xmax, int ymax)
    {
      // POST renvoie la region dans laquelle se situe le point
      region r;
      r.i = 0;
      if (x<xmin)
        r.r.gauche=1;    // le point est a gauche de la zone
      else if (x>=xmax)
        r.r.droite=1;    // le point est a droite de la zone
      if (y<ymin)
        r.r.basse=1;     // le point est en dessous de la zone
      else if (y>=ymax)
        r.r.haute=1;     // le point est au dessus de la zone
      return(r);
    }

    bool ClipBox(const PointZ&p, int &x, int &y, int &w, int &h)
    {
      x = p.x;
      y = p.y;

      if (x >= xmax) return true;
      if (y >= ymax) return true;
      if (x+w < xmin) return true;
      if (y+h < ymin) return true;

      if (x+w >= xmax) w = xmax-1-x;
      if (y+h >= ymax) h = ymax-1-y;
      if (x < xmin) {w-=(xmin-x); x=xmin;}
      if (y < ymin) {h-=(ymin-y); y=ymin;}
      return false;
    }

    void LineCoefs(int x1, int y1, int x2, int y2,
      int &x, int &y, int &dx, int &dy, int &s1, int &s2,
      int &interchange, int &e, int &l)
    {
      x = x1;
      y = y1;
      dx = abs(x2-x1);
      dy = abs(y2-y1);
      s1 = sign(x2-x1);
      s2 = sign(y2-y1);
      if (dy>dx)
      {
        int tmp = dx;
        dx = dy;
        dy = tmp;
        interchange = 1;
      }
      else
        interchange = 0;
      e = 2*dy-dx;
      l = fWidth*s2;
    }
    // !!! warning color is not clipped
    bool ClipLine(const PointZ &p1, const PointZ & p2,
      int& x1, int& y1, real& z1,
      int& x2, int& y2, real& z2,
      realtype& color1, realtype& color2)
    {
      x1 = p1.x;
      y1 = p1.y;
      z1 = p1.z;
      x2 = p2.x;
      y2 = p2.y;
      z2 = p2.z;
      region c,c1,c2;
      int x=0,y=0;
      real z=0;

      c1 = codebinaire(x1, y1, xmin, ymin, xmax, ymax);
      c2 = codebinaire(x2, y2, xmin, ymin, xmax, ymax);
      while ( (c1.i!=0)||(c2.i!=0) )
      {
        if ((c1.i&c2.i) != 0)
          // the segment don't intercept the box
          return false;
        if (c1.i==0) c=c2; else c=c1;
        if (c.r.gauche!=0)
        {
          x = xmin;
          y = y1+((y2-y1)*(x-x1))/(x2-x1);
          z = z1+((z2-z1)*(x-x1))/(x2-x1);
        }
        else if (c.r.droite!=0)
        {
          x = xmax-1;
          y = y1+((y2-y1)*(x-x1))/(x2-x1);
          z = z1+((z2-z1)*(x-x1))/(x2-x1);
        }
        else if (c.r.basse!=0)
        {
          y = ymin;
          x = x1+((x2-x1)*(y-y1))/(y2-y1);
          z = z1+((z2-z1)*(y-y1))/(y2-y1);
        }
        else if (c.r.haute!=0)
        {
          y = ymax-1;
          x = x1+((x2-x1)*(y-y1))/(y2-y1);
          z = z1+((z2-z1)*(y-y1))/(y2-y1);
        }
        if (c.i==c1.i)
        {
          x1 = x;
          y1 = y;
          z1 = z;
          c1 = codebinaire(x,y, xmin, ymin, xmax, ymax);
        }
        else
        {
          x2 = x;
          y2 = y;
          z2 = z;
          c2 = codebinaire(x,y, xmin, ymin, xmax, ymax);
        }
      }
      color1 = conv(p1.color);
      color2 = conv(p2.color);
      return true;
    }
    void NoClipLine(int x1,  int y1, real z1,
      int x2,  int y2, real z2,
      realtype c1, realtype c2)
    {
      assert( (x1>=0)&&(x1<fWidth)&&(y1>=0)&&(y1<fHeight) );
      assert( (x2>=0)&&(x2<fWidth)&&(y2>=0)&&(y2<fHeight) );

      int x,y,dx,dy,s1,s2,interchange,e,l;
      LineCoefs(x1,y1,x2,y2,x,y,dx,dy,s1,s2,interchange,e,l);
      if (dx == 0) return;

      fPixeler.SetStart(PointZ(x1,y1,Color(round(c1)),z1));
      fPixeler.SetEnd(PointZ(x2,y2,Color(round(c2)),z2));
      fPixeler.SetSteps(dx);
      if (interchange == 1)
        for (int i=0; i<dx; i++)
        {
          fPixeler.Pixel();
          if (e>=0) { fPixeler.NextL(s1+l); e = e-2*dx; }
          else fPixeler.NextL(l);
          e = e+2*dy;
        }
      else
        for (int i=0; i<dx; i++)
        {
          fPixeler.Pixel();
          if (e>=0) { fPixeler.NextL(l+s1); e = e-2*dx; }
          else fPixeler.NextL(s1);
          e = e+2*dy;
        }
    }
    int domain(long x)
    {
      int d=0;
      do
      {
        d++;
        x >>= 1;
      }
      while (x);
      return d;
    }
    void compute(int k, int mask, long *rx, long *dx, long y)
    {
      *rx += (y<<1);
      *dx = *rx>>k;
      *rx &= mask;
    }
#define NullY int(0x7fffffff)
    inline void EllipsePixel(int x, int y)
    {
      if (y>=0 && y<fHeight)
      {
        if (ellipse_xd[(y)]==NullY)
        {
          ellipse_xd[(y)]=(x);
          ellipse_xf[(y)]=(x);
        }
        else
        {
          ellipse_xd[(y)]=min(x, ellipse_xd[(y)]);
          ellipse_xf[(y)]=max(x, ellipse_xf[(y)]);
        }
      }
    }
    inline void ClipPixel(const PointZ & p)
    {
      if((p.x>=xmin)&&(p.x<xmax)&&(p.y>=ymin)&&(p.y<ymax))
        fPixeler.PixelAt(p.x, p.y, fconv(p.z));
    }
    void Ellipse(int CX, int CY,
      int x1, int y1, int x2, int y2, Color color,
      real z, bool fill)
    {
      long dx1, dx2, rx1, rx2;
      long dy1, dy2, ry1, ry2;
      int k, mask;

      for (int l=0; l<fHeight; l++)
      { ellipse_xd[l] = NullY; ellipse_xf[l]=NullY; }

      // if (x2,y2) is to the right {invert it}
      // counterclockwise by now
      if (x1*y2-y1*x2<0) x2=-x2,y2=-y2;

      k = ((domain(max(x1*x1+x2*x2, y1*y1+y2*y2))+1)>>1)+1;
      mask = (1<<k)-1;

      fPixeler.SetStart(PointZ(CX, CY, color, z));
      fPixeler.SetEnd(PointZ(CX, CY, color, z));

      if (fill)
      {
        EllipsePixel(CX+x1,CY+y1);
        EllipsePixel(CX-x1,CY-y1);
        EllipsePixel(CX+x2,CY+y2);
        EllipsePixel(CX-x2,CY-y2);
      }
      else
      {
        ClipPixel(PointZ(CX+x1,CY+y1,color));
        ClipPixel(PointZ(CX-x1,CY-y1,color));
        ClipPixel(PointZ(CX+x2,CY+y2,color));
        ClipPixel(PointZ(CX-x2,CY-y2,color));
      }

      dx1 = dx2 = dy1 = dy2 = 0;
      rx1 = rx2 = ry1 = ry2 = 1<<(k-1);

      int oldx2=x2, oldy2=y2;
      while (! (x1==oldx2 && y1==oldy2) )
      {
        rx1 -= dx2;
        rx2 += dx1;
        ry1 -= dy2;
        ry2 += dy1;
        compute(k, mask, &rx1, &dx1, x2);
        compute(k, mask, &rx2, &dx2, x1);
        compute(k, mask, &ry1, &dy1, y2);
        compute(k, mask, &ry2, &dy2, y1);
        x1 += dx1;
        x2 -= dx2;
        y1 += dy1;
        y2 -= dy2;

        if (fill)
        {
          if (dx1 || dy1)
          {
            EllipsePixel(CX+x1,CY+y1);
            EllipsePixel(CX-x1,CY-y1);
          }
          if (dx2 || dy2)
          {
            EllipsePixel(CX+x2,CY+y2);
            EllipsePixel(CX-x2,CY-y2);
          }
        }
        else
        {
          if (dx1 || dy1)
          {
            ClipPixel(PointZ(CX+x1,CY+y1,color));
            ClipPixel(PointZ(CX-x1,CY-y1,color));
          }
          if (dx2 || dy2)
          {
            ClipPixel(PointZ(CX+x2,CY+y2,color));
            ClipPixel(PointZ(CX-x2,CY-y2,color));
          }
        }
      }

      if (fill)
        for (int y=0; y<fHeight; y++)
          if (ellipse_xd[y]!=NullY || ellipse_xf[y]!=NullY)
            Horiz(ellipse_xd[y], ellipse_xf[y], y, color, z, z);

    }
#define DeNullify(x) ((x)==0.0?1.0:x)
    void Horiz(int xb, int xe, int y, Color color, real zb, real ze)
    {
      if ( ((y<ymin)||(y>ymax)) || ((xb<xmin)&&(xe<xmin)) || ((xb>xmax)&&(xe>xmax)) ) return;
      if (xb>xe) { Swap(xb,xe); Swap(zb,ze); }
      real z = zb;
      if (xb<xmin)
      {
        real iz = (ze-zb)/ DeNullify(real(xe-xb));
        z += iz*(xmin-xb);
        xb = xmin;
      }
      if (xe>xmax)
      {
        real iz = (zb-ze)/ DeNullify(real(xb-xe));
        ze += iz*(xmax-xe);
        xe = xmax;
      }
      fPixeler.SetStart(PointZ(xb,y,color,z));
      fPixeler.SetEnd(PointZ(xe,y,color,ze));
      fPixeler.SetSteps(xe-xb);
      for (int x=xb; x<xe; x++, fPixeler.Next())
        fPixeler.Pixel();
    }

    Pixeler fPixeler;
    int fBorder;
    int * ellipse_xd, * ellipse_xf;
  };

}

#endif
