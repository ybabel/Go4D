#include "Drawer2D.h"
#include <memory.h>

namespace Go4D
{

  /*****************************************************************************\
  *  Drawer2D
  \*****************************************************************************/


  Drawer2D::Drawer2D(const Image &aImage, int pWidth, int pHeight, bool circlecaps, int pBorder)
    : inherited(aImage)
  {
    assert(pWidth>0);
    assert(pHeight>0);
    assert(pBorder*2 < pWidth);
    assert(pBorder*2 < pHeight);
    fBorder = pBorder;
    Plug(aImage);
    fCircleCaps = circlecaps;
    if (fCircleCaps)
    {
      ellipse_xd = new int[fHeight];
      ellipse_xf = new int[fHeight];
    }
  }

  Drawer2D::Drawer2D(int pWidth, int pHeight, bool AutoAlloc, bool circlecaps, int pBorder)
    : inherited(pWidth, pHeight, AutoAlloc)
  {
    assert( (pWidth>0 && pHeight>0 && pBorder*2 < pWidth && pBorder*2 < pHeight) || (!AutoAlloc));
    fBorder = pBorder;
    xmin = fBorder;
    ymin = fBorder;
    xmax = fWidth-fBorder;
    ymax = fHeight-fBorder;
    fCircleCaps = circlecaps;
    if (fCircleCaps)
    {
      ellipse_xd = new int[fHeight];
      ellipse_xf = new int[fHeight];
    }
  }

  Drawer2D::~Drawer2D()
  {
    if (fCircleCaps)
    {
      delete [] ellipse_xd;
      delete [] ellipse_xf;
    }
  }

  void Drawer2D::Plug(const Image & image)
  {
    inherited::Plug(image);
    xmin = fBorder;
    ymin = fBorder;
    xmax = fWidth-fBorder;
    ymax = fHeight-fBorder;
  }

  void Drawer2D::Copy(const Image & ima)
  {
    assert(fLen == ima.Len());
    Color * imaBuf = ima.GetBuffer();
    //for (int i=0; i<fLen; i++)
    //  fBuffer[i] =imaBuf[i];
    memcpy(fBuffer, imaBuf, fLen);
  }

  bool Drawer2D::ClipBox(const PointZ &p, int &x, int &y, int &w, int &h)
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

  void Drawer2D::BoxZ(const PointZ & p, int w, int h)
  {
    Color color = p.color;
    int x, y;
    if (ClipBox(p,x,y,w,h)) return;
    Color * ptr = fBuffer+x+fWidth*y;

    for (int j=0; j<h; j++)
    {
      for (int i=0; i<w; i++, ptr++) *ptr = color;
      ptr += fWidth-w;
    }
  }

  void Drawer2D::FastBox(int xmin, int ymin, int xmax, int ymax, Color color)
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

  void Drawer2D::LineCoefs(int x1, int y1, int x2, int y2,
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
    l=fWidth*s2;
  }

  void Drawer2D::NoClip(int x1,  int y1, realtype ,
    int x2,  int y2, realtype ,
    realtype c1, realtype c2)
  {
    assert( (x1>=0)&&(x1<=fWidth)&&(y1>=0)&&(y1<=fHeight) );
    assert( (x2>=0)&&(x2<=fWidth)&&(y2>=0)&&(y2<=fHeight) );

    int x,y,dx,dy,s1,s2,interchange,e,l;
    LineCoefs(x1,y1,x2,y2,x,y,dx,dy,s1,s2,interchange,e,l);
    if (dx == 0) return;

    Color * ptr = fBuffer+x+y*fWidth;

    realtype c = c1;
    realtype ic = rapport( (c2-c1)*s1, conv(dx) );

    if (interchange == 1)
      for (int i=0; i<dx; i++)
      {
        *ptr = Color(round(c));
        if (e>=0) { ptr += s1; e = e-2*dx; }
        ptr += l; e = e+2*dy;
        inc(c,ic);
      }
    else
      for (int i=0; i<dx; i++)
      {
        *ptr = Color(round(c));
        if (e>=0) { ptr += l; e = e-2*dx; }
        ptr += s1; e = e+2*dy;
        inc(c,ic);
      }
  }

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

  void Drawer2D::Line(int x1, int y1, int x2, int y2, Color color)
  {
    Line(PointZ(x1, y1, color, 0.0), PointZ(x2, y2, color, 0.0));
  }
  void Drawer2D::Line(const PointZ &p1, const PointZ & p2)
  {
    int x1 = p1.x;
    int y1 = p1.y;
    real z1 = p1.z;
    int x2 = p2.x;
    int y2 = p2.y;
    real z2 = p2.z;
    region c,c1,c2;
    int x=0,y=0;
    real z=0;

    c1 = codebinaire(x1, y1, xmin, ymin, xmax, ymax);
    c2 = codebinaire(x2, y2, xmin, ymin, xmax, ymax);
    while ( (c1.i!=0)||(c2.i!=0) )
    {
      if ((c1.i&c2.i) != 0)
        // the segment don't intercept the box
        return;
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
    NoClip(x1,y1, fconv(z1), x2,y2, fconv(z2), conv(p1.color), conv(p2.color));
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

  void Drawer2D::Ellipse(int CX, int CY,
    int x1, int y1, int x2, int y2, Color color)
  {
    assert(fCircleCaps);

    long dx1, dx2, rx1, rx2;
    long dy1, dy2, ry1, ry2;
    int k, mask;

    // if (x2,y2) is to the right {invert it}
    // counterclockwise by now
    if (x1*y2-y1*x2<0) x2=-x2,y2=-y2;

    k = ((domain(max(x1*x1+x2*x2, y1*y1+y2*y2))+1)>>1)+1;
    mask = (1<<k)-1;

    ClipPixel(PointZ(CX+x1,CY+y1,color));
    ClipPixel(PointZ(CX-x1,CY-y1,color));
    ClipPixel(PointZ(CX+x2,CY+y2,color));
    ClipPixel(PointZ(CX-x2,CY-y2,color));

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

#define NullY int(0x7fffffff)

  inline void Drawer2D::EllipsePixel(int x, int y)
  {
    assert(fCircleCaps);

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

  void Drawer2D::FillEllipse(int CX, int CY,
    int x1, int y1, int x2, int y2, Color color)
  {
    assert(fCircleCaps);

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

    EllipsePixel(CX+x1,CY+y1);
    EllipsePixel(CX-x1,CY-y1);
    EllipsePixel(CX+x2,CY+y2);
    EllipsePixel(CX-x2,CY-y2);

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

    for (int y=0; y<fHeight; y++)
      if (ellipse_xd[y]!=NullY || ellipse_xf[y]!=NullY)
        Horiz(ellipse_xd[y], ellipse_xf[y], y, color);

  }

  void Drawer2D::Ellipse(PointZ center, PointZ xaxe, PointZ yaxe)
  {
    assert(fCircleCaps);

    Ellipse(center.x, center.y,
      xaxe.x-center.x, xaxe.y-center.y,
      yaxe.x-center.x, yaxe.y-center.y, center.color);
  }

  void Drawer2D::FillEllipse(PointZ center, PointZ xaxe, PointZ yaxe)
  {
    assert(fCircleCaps);

    FillEllipse(center.x, center.y,
      xaxe.x-center.x, xaxe.y-center.y,
      yaxe.x-center.x, yaxe.y-center.y, center.color);
  }

  void Drawer2D::Circle(PointZ center, int radius)
  {
    assert(fCircleCaps);

    Ellipse(center.x, center.y,
      radius, 0,
      0, radius, center.color);
  }

  void Drawer2D::FillCircle(PointZ center, int radius)
  {
    assert(fCircleCaps);

    FillEllipse(center.x, center.y,
      0, radius,
      radius, 0, center.color);
  }

  void Drawer2D::ShadeVert(int x, int yb, int ye, int cb, int ce)
  {
    if ( (x<xmin)||(x>xmax) || (yb<ymin)&&(ye<ymin) || (yb>ymax)&&(ye>ymax) )
      return;
    if (yb>ye) {Swap(yb,ye); Swap(cb,ce);}
    realtype IncrC = rapport( (ce-cb), ye-yb );
    realtype  ofsC = conv(cb);
    if (yb<ymin)
    {
      incn(ofsC, IncrC, ymin-yb );
      yb = ymin;
    }
    if (ye>ymax) ye = ymax;
    Color * Cur = fBuffer+fWidth*yb+x;
    for (int y=yb; y<=ye; y++, Cur+=fWidth)
    {
      *Cur = Color(round(ofsC));
      inc(ofsC,IncrC);
    }
  }

#ifdef usefixed
#define declare(r,num,div) int r = ( long((num)<<fprecision) / long((div)==0?1:(div)) )
#define var(a,b) int a = ((b)<<fprecision)
#endif
#ifdef usefloat
#define declare(r,num,div) float r = float(num)/float(div==0?1:div)
#define newdecl(r,num,div) r = float(num)/float(div==0?1:div)
#endif

  void Drawer2D::Fill4(int x1, int y1, int x2, int y2, int x4, int y4, int x3, int y3, int color)
  {  // fill a triangle
    int MinY = y1;
    if (y2<MinY) MinY=y2;
    if (y3<MinY) MinY=y3;
    if (y4<MinY) MinY=y4;
    int MaxY = y1;
    if (y2>MaxY) MaxY=y2;
    if (y3>MaxY) MaxY=y3;
    if (y4>MaxY) MaxY=y4;
    while (y1!=MinY)
    {
      int tmp;
      tmp=x1; x1=x2; x2=x4; x4=x3; x3=tmp;
      tmp=y1; y1=y2; y2=y4; y4=y3; y3=tmp;
    }
    // now the 4 plots are ordered.
    var(fxb,y1==y2?x2:x1);
    var(fxe,y1==y3?x3:x1);
    declare(fr12,x2-x1,y2-y1);
    declare(fr24,x4-x2,y4-y2);
    declare(fr43,x3-x4,y3-y4);
    declare(fr13,x3-x1,y3-y1);
    declare(fr34,x4-x3,y4-y3);
    declare(fr42,x2-x4,y2-y4);

    int y=y1;
    if (y==y2) fxb=conv(x2);
    if (y==y3) fxe=conv(x3);
    while ( y < MaxY)
    {
      Horiz( round(fxb), round(fxe),y,color);
      if (y>=y2)
        if (y>y4) inc(fxb,fr43); else inc(fxb,fr24);
      else inc(fxb,fr12);
      if (y>=y3)
        if (y>y4) inc(fxe,fr42); else inc(fxe,fr34);
      else inc(fxe,fr13);
      y++;
    }
  }

  void Drawer2D::Horiz(int xb, int xe, int y, int color)
  {
    if ( ((y<ymin)||(y>ymax)) || ((xb<xmin)&&(xe<xmin)) || ((xb>xmax)&&(xe>xmax)) ) return;
    if (xb>xe) Swap(xb,xe);
    if (xb<xmin) xb = xmin;
    if (xe>xmax) xe = xmax;
    Color * Cur = fBuffer+fWidth*y+xb;
    for (int x=xb; x<xe; x++, Cur++)
      *Cur = Color(color);
  }

  void Drawer2D::Map4(Image& pIma, int sx1, int sy1, int sx2, int sy2, int sx4, int sy4, int sx3, int sy3, int x1, int y1, int x2, int y2, int x4, int y4, int x3, int y3)
  {  // fill a quadrilatere
    int MinY = y1;
    if (y2<MinY) MinY=y2;
    if (y3<MinY) MinY=y3;
    if (y4<MinY) MinY=y4;
    int MaxY = y1;
    if (y2>MaxY) MaxY=y2;
    if (y3>MaxY) MaxY=y3;
    if (y4>MaxY) MaxY=y4;
    while (y1!=MinY)
    {
      int tmp;
      tmp=x1; x1=x2; x2=x4; x4=x3; x3=tmp;
      tmp=y1; y1=y2; y2=y4; y4=y3; y3=tmp;
      tmp=sx1; sx1=sx2; sx2=sx4; sx4=sx3; sx3=tmp;
      tmp=sy1; sy1=sy2; sy2=sy4; sy4=sy3; sy3=tmp;
    }
    // now the 4 plots are ordered.

    var(fxb,y1==y2?x2:x1);
    var(fxe,y1==y3?x3:x1);
    declare(fr12,x2-x1,y2-y1);
    declare(fr24,x4-x2,y4-y2);
    declare(fr13,x3-x1,y3-y1);
    declare(fr34,x4-x3,y4-y3);
    declare(fr43,x3-x4,y3-y4);
    declare(fr42,x2-x4,y2-y4);

    var(fsxb,y1==y2?sx2:sx1);
    var(fsyb,y1==y2?sy2:sy1);
    var(fsxe,sx1);
    var(fsye,sy1);
    declare(frx12,sx2-sx1,abs(y2-y1));
    declare(fry12,sy2-sy1,abs(y2-y1));
    declare(frx24,sx4-sx2,abs(y4-y2));
    declare(fry24,sy4-sy2,abs(y4-y2));
    declare(frx13,sx3-sx1,abs(y3-y1));
    declare(fry13,sy3-sy1,abs(y3-y1));
    declare(frx34,sx4-sx3,abs(y4-y3));
    declare(fry34,sy4-sy3,abs(y4-y3));
    declare(frx43,sx3-sx4,abs(y3-y4));
    declare(fry43,sy3-sy4,abs(y3-y4));
    declare(frx42,sx2-sx4,abs(y2-y4));
    declare(fry42,sy2-sy4,abs(y2-y4));

    int y=y1;
    if (y==y2) {fxb=conv(x2); fsxb=conv(sx2); fsyb=conv(sy2);}
    if (y==y3) {fxe=conv(x3); fsxe=conv(sx3); fsye=conv(sy3);}

    while ( y < MaxY)
    {
      MapHoriz( round(fxb), round(fxe),y, round(fsxb), round(fsyb), round(fsxe), round(fsye), pIma);
      if (y>=y2)
        if (y>=y4) { inc(fxb,fr43); inc(fsxb,frx43); inc(fsyb,fry43); }
        else       { inc(fxb,fr24); inc(fsxb,frx24); inc(fsyb,fry24); }
      else         { inc(fxb,fr12); inc(fsxb,frx12); inc(fsyb,fry12); }
      if (y>=y3)
        if (y>=y4) { inc(fxe,fr42); inc(fsxe,frx42); inc(fsye,fry42); }
        else       { inc(fxe,fr34); inc(fsxe,frx34); inc(fsye,fry34); }
      else         { inc(fxe,fr13); inc(fsxe,frx13); inc(fsye,fry13); }
      y++;
    }
  }

  void Drawer2D::MapHoriz(int xb, int xe, int y, int sxb, int syb, int sxe, int sye, Image& pIma)
  {
#ifndef wrap
    assert(sxb>=0&&sxb<pIma.Width());
    assert(sxe>=0&&sxe<pIma.Width());
    assert(syb>=0&&syb<pIma.Height());
    assert(sye>=0&&sye<pIma.Height());
    assert(sxb+pIma.Width()*syb<pIma.Len());
    assert(sxe+pIma.Width()*sye<pIma.Len());
#endif
    if ( (y<ymin)||(y>ymax) || (xb<xmin)&&(xe<xmin) || (xb>xmax)&&(xe>xmax) ) return;
    if (xb>xe) {Swap(xb,xe); Swap(sxb,sxe); Swap(syb,sye); }
    int bl=pIma.Width();
#ifdef wrap
    int bh=pIma.Height();
#endif
    declare(fIncrX, (sxe-sxb), xe-xb );
    declare(fIncrY, (sye-syb), xe-xb );
    var(fofsX,0);
    var(fofsY,0);
    if (xb<xmin)
    {
      incn(fofsX,fIncrX,xmin-xb);
      {incn(fofsY,fIncrY,xmin-xb);}
      xb = xmin;
    }
    if (xe>xmax) xe = xmax;
    Color * Cur = fBuffer+fWidth*y+xb;
    Color * Ima = pIma.GetBuffer()+bl*syb+sxb;
    for (int x=xb; x<=xe; x++, Cur++)
    {
#ifndef wrap
      *Cur = *(Ima+round(fofsX)+bl*round(fofsY));
#else
      byte * tmp = Ima+ (round(fofsX)%bl) + bl*(round(fofsY)%bh);
      *Cur = *(tmp);
#endif
      inc(fofsX,fIncrX);
      inc(fofsY,fIncrY);
    }
  }

  void Drawer2D::MapBox(Image& image, int sx1, int sy1, int sx2, int sy2,
    int x1, int y1, int x2, int y2)
  {
    Map4(image,
      sx1, sy1, sx1, sy2, sx2, sy2, sx2, sy1,
      x1, y1, x1, y2, x2-1, y2, x2-1, y1);
  }


#define PIX(i) image.GetBuffer()[i]
#define AVRG4(a,b,c,d) Color((int)(PIX(a)+PIX(b)+PIX(c)+PIX(d))/4)
#define DPIX(x,y) image.GetBuffer()[x+(y)*fWidth]
  void Drawer2D::Blur(const Image & image)
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
      fBuffer[i] = Color(int((
        DPIX(i,0)+DPIX(i-1,0)+DPIX(i+1,0)+
        DPIX(i,1)+DPIX(i-1,1)+DPIX(i+1,1))/6));
      fBuffer[fWidth*(fHeight-1)+i] = Color(int((
        DPIX(i,fHeight-1)+DPIX(i-1,fHeight-1)+DPIX(i+1,fHeight-1)+
        DPIX(i,fHeight-2)+DPIX(i-1,fHeight-2)+DPIX(i+1,fHeight-2))/6));
    }
    for (i=1; i<fHeight-1; i++)
    {
      fBuffer[fWidth*i] = Color(int((
        DPIX(0,i)+DPIX(0,i-1)+DPIX(0,i+1)+
        DPIX(1,i)+DPIX(1,i-1)+DPIX(1,i+1))/6));
      fBuffer[fWidth-1+fWidth*i] = Color(int((
        DPIX(fWidth-1,i)+DPIX(fWidth-1,i-1)+DPIX(fWidth-1,i+1)+
        DPIX(fWidth-2,i)+DPIX(fWidth-2,i-1)+DPIX(fWidth-2,i+1))/6));
    }

    // all the others
    Color * src=fBuffer+fWidth+1, * dst=image.GetBuffer()+fWidth+1;

    for (int x=1; x<fWidth-1; x++)
    {
      for (int y=1; y<fHeight-1; y++)
      {
        *src = Color(int((
          *(dst-fWidth-1)+*(dst-fWidth)+*(dst-fWidth+1)+
          *(dst-1)+*(dst)+*(dst+1)+
          *(dst+fWidth-1)+*(dst+fWidth)+*(dst+fWidth+1))/9));
        src++;
        dst++;
      }
      src+=2;
      dst+=2;
    }
  }

  void Drawer2D::BlurAttract(const Image & image, Color decr, int coef)
  {
    int i;
    assert(fLen == image.Len());

    // edges
    fBuffer[0] = Color(int((AVRG4(0,1,fWidth,fWidth+1)-decr)*coef/256+decr));
    fBuffer[fWidth-1] = Color(int((AVRG4(fWidth-1, fWidth-2, 2*fWidth-1, 2*fWidth-2)-decr)*coef/256+decr));
    fBuffer[fWidth*(fHeight-1)] = Color(int((
      AVRG4(fWidth*(fHeight-1),fWidth*(fHeight-1)+1,
      fWidth*(fHeight-2),fWidth*(fHeight-2)+1) - decr)*coef/256+decr));
    fBuffer[fWidth*(fHeight)-1] = Color(int((
      AVRG4(fWidth*(fHeight)-1,fWidth*(fHeight)-2,
      fWidth*(fHeight-1)-1,fWidth*(fHeight-1)-2) - decr)*coef/256+decr));

    // sides
    for (i=1; i<fWidth-1; i++)
    {
      fBuffer[i] = Color(int(((
        DPIX(i,0)+DPIX(i-1,0)+DPIX(i+1,0)+
        DPIX(i,1)+DPIX(i-1,1)+DPIX(i+1,1))/6-decr)*coef/256+decr));
      fBuffer[fWidth*(fHeight-1)+i] = Color(int(((
        DPIX(i,fHeight-1)+DPIX(i-1,fHeight-1)+DPIX(i+1,fHeight-1)+
        DPIX(i,fHeight-2)+DPIX(i-1,fHeight-2)+DPIX(i+1,fHeight-2))/6-decr)*coef/256+decr));
    }
    for (i=1; i<fHeight-1; i++)
    {
      fBuffer[fWidth*i] = Color(int(((
        DPIX(0,i)+DPIX(0,i-1)+DPIX(0,i+1)+
        DPIX(1,i)+DPIX(1,i-1)+DPIX(1,i+1))/6-decr)*coef/256+decr));
      fBuffer[fWidth-1+fWidth*i] = Color(int(((
        DPIX(fWidth-1,i)+DPIX(fWidth-1,i-1)+DPIX(fWidth-1,i+1)+
        DPIX(fWidth-2,i)+DPIX(fWidth-2,i-1)+DPIX(fWidth-2,i+1))/6-decr)*coef/256+decr));
    }

    // all the others
    Color * src=fBuffer+fWidth+1, * dst=image.GetBuffer()+fWidth+1;

    for (int x=1; x<fWidth-1; x++)
    {
      for (int y=1; y<fHeight-1; y++)
      {
        //*src = byte(int((
        //  (*(dst-fWidth-1)+*(dst-fWidth)+*(dst-fWidth+1)+
        //   *(dst-1)+*(dst)+*(dst+1)+
        //   *(dst+fWidth-1)+*(dst+fWidth)+*(dst+fWidth+1))/9-decr)*coef/256+decr));
        *src = Color(int((
          ((*(dst-fWidth-1)+*(dst-fWidth)+*(dst-fWidth+1)+
          *(dst-1)+*(dst+1)+
          *(dst+fWidth-1)+*(dst+fWidth)+*(dst+fWidth+1))>>3)-decr)*coef/256+decr));
        src++;
        dst++;
      }
      src+=2;
      dst+=2;
    }
  }


}
