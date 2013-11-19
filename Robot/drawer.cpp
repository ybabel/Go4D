/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : drawer.cpp, drawer.h implementation
 *CREATED: 09/10/95
 *DATE   : 09/03/96
 *BUGS   : Ellipse and FillEllipse (lines disapear)
\*****************************************************************************/


#include <string.h>
#include "drawer.h"


/*****************************************************************************\
 *  Drawer
\*****************************************************************************/


Drawer::Drawer(int pWidth, int pHeight, int pBorder, int autoalloc)
:Image(pWidth,pHeight, autoalloc)
{
  assert(pWidth>0);
  assert(pHeight>0);
  assert(pBorder*2 < pWidth);
  assert(pBorder*2 < pHeight);
  xmin = pBorder;
  ymin = pBorder;
  xmax = fWidth-pBorder;
  ymax = fHeight-pBorder;
  ellipse_xd = new int[fHeight];
  ellipse_xf = new int[fHeight];
}

Drawer::~Drawer()
{
  delete [] ellipse_xd;
  delete [] ellipse_xf;
}

void Drawer::Clear(byte pColor, realtype )
{
  memset(fBuffer, pColor, fLen);
  //for (int i=0; i<fLen; i++)
  //  *(fBuffer+i) = pColor;
}

void Drawer::Resize(int newsizex, int newsizey)
{
  Image::Resize(newsizex, newsizey);
  xmin = 0;
  ymin = 0;
  xmax = fWidth;
  ymax = fHeight;
}

Drawer & Drawer::operator = (const Image & ima)
{
  assert(fLen == ima.fLen);
  // becarefull don't copy the zbuffer (it doesn't exist in Image)
  if( &ima == this ) return *this;
  for (int i=0; i<fLen; i++)
    fBuffer[i] =ima.fBuffer[i];
  return *this;
}

void Drawer::Copy(const Drawer & drawer)
{
  assert(fLen == drawer.fLen);
  if( &drawer == this ) return;
  for (int i=0; i<fLen; i++)
    fBuffer[i] =drawer.fBuffer[i];
}

#define PIX(i) drawer.fBuffer[i]
#define AVRG4(a,b,c,d) byte((int)(PIX(a)+PIX(b)+PIX(c)+PIX(d))/4)
#define DPIX(x,y) drawer.fBuffer[x+(y)*fWidth]
void Drawer::Blur(const Drawer & drawer)
{
  int i;
  assert(fLen == drawer.fLen);

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
  byte * src=fBuffer+fWidth+1, * dst=drawer.fBuffer+fWidth+1;

  for (int x=1; x<fWidth-1; x++)
  {
    for (int y=1; y<fHeight-1; y++)
    {
      *src = byte(int((
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

void Drawer::BlurAttract(const Drawer & drawer, byte decr, int coef)
{
  int i;
  assert(fLen == drawer.fLen);

  // edges
  fBuffer[0] = byte(int((AVRG4(0,1,fWidth,fWidth+1)-decr)*coef/256+decr));
  fBuffer[fWidth-1] = byte(int((AVRG4(fWidth-1, fWidth-2, 2*fWidth-1, 2*fWidth-2)-decr)*coef/256+decr));
  fBuffer[fWidth*(fHeight-1)] = byte(int((
    AVRG4(fWidth*(fHeight-1),fWidth*(fHeight-1)+1,
      fWidth*(fHeight-2),fWidth*(fHeight-2)+1) - decr)*coef/256+decr));
  fBuffer[fWidth*(fHeight)-1] = byte(int((
    AVRG4(fWidth*(fHeight)-1,fWidth*(fHeight)-2,
      fWidth*(fHeight-1)-1,fWidth*(fHeight-1)-2) - decr)*coef/256+decr));

  // sides
  for (i=1; i<fWidth-1; i++)
  {
    fBuffer[i] = byte(int(((
      DPIX(i,0)+DPIX(i-1,0)+DPIX(i+1,0)+
      DPIX(i,1)+DPIX(i-1,1)+DPIX(i+1,1))/6-decr)*coef/256+decr));
    fBuffer[fWidth*(fHeight-1)+i] = byte(int(((
      DPIX(i,fHeight-1)+DPIX(i-1,fHeight-1)+DPIX(i+1,fHeight-1)+
      DPIX(i,fHeight-2)+DPIX(i-1,fHeight-2)+DPIX(i+1,fHeight-2))/6-decr)*coef/256+decr));
  }
  for (i=1; i<fHeight-1; i++)
  {
    fBuffer[fWidth*i] = byte(int(((
      DPIX(0,i)+DPIX(0,i-1)+DPIX(0,i+1)+
      DPIX(1,i)+DPIX(1,i-1)+DPIX(1,i+1))/6-decr)*coef/256+decr));
    fBuffer[fWidth-1+fWidth*i] = byte(int(((
      DPIX(fWidth-1,i)+DPIX(fWidth-1,i-1)+DPIX(fWidth-1,i+1)+
      DPIX(fWidth-2,i)+DPIX(fWidth-2,i-1)+DPIX(fWidth-2,i+1))/6-decr)*coef/256+decr));
  }

  // all the others
  byte * src=fBuffer+fWidth+1, * dst=drawer.fBuffer+fWidth+1;

  for (int x=1; x<fWidth-1; x++)
  {
    for (int y=1; y<fHeight-1; y++)
    {
      *src = byte(int((
        (*(dst-fWidth-1)+*(dst-fWidth)+*(dst-fWidth+1)+
         *(dst-1)+*(dst)+*(dst+1)+
         *(dst+fWidth-1)+*(dst+fWidth)+*(dst+fWidth+1))/9-decr)*coef/256+decr));
      src++;
      dst++;
    }
    src+=2;
    dst+=2;
  }
}

int Drawer::ClipChar(const PointZ &p, const Fonts &f,
	int &x, int &y, int &w, int &h,
	byte *& chr)
{
  w = f.GetDimX();
  h = f.GetDimY();
  x = p.x;
  y = p.y;

  if (x >= xmax) return TRUE;
  if (y >= ymax) return TRUE;
  if (x+w < xmin) return TRUE;
  if (y+h < ymin) return TRUE;

  if (x+w > xmax) w = xmax-x;
  if (y+h > ymax) h = ymax-y;
  if (x<xmin) {w-=(xmin-x); chr+=(xmin-x); x=xmin;}
  if (y<ymin) {h-=(ymin-y); chr+=(ymin-y)*f.GetDimX(); y=ymin;}
  return FALSE;
}

void Drawer::PutChar(const PointZ &p, const Fonts & f, char c)
{
  byte color = p.color;
  int x,y,w,h;
  byte * chr = f[c];
  if (ClipChar(p,f,x,y,w,h,chr)) return;
  byte * ptr = fBuffer+x+fWidth*y;
  
  for (int j=0; j<h; j++)
    {
      for (int i=0; i<w; i++, ptr++, chr++)
	if (*chr != 0) *ptr = *chr&color;
      ptr += fWidth-w;
      chr += f.GetDimX()-w;
    }
}

int Drawer::ClipBox(const PointZ &p, int &x, int &y, int &w, int &h)
{
  x = p.x;
  y = p.y;

  if (x >= xmax) return TRUE;
  if (y >= ymax) return TRUE;
  if (x+w < xmin) return TRUE;
  if (y+h < ymin) return TRUE;

  if (x+w >= xmax) w = xmax-1-x;
  if (y+h >= ymax) h = ymax-1-y;
  if (x < xmin) {w-=(xmin-x); x=xmin;}
  if (y < ymin) {h-=(ymin-y); y=ymin;}
  return FALSE;
}

void Drawer::Box(const PointZ & p, int w, int h)
{  
  byte color = p.color;
  int x, y;
  if (ClipBox(p,x,y,w,h)) return;
  byte * ptr = fBuffer+x+fWidth*y;
  
  for (int j=0; j<h; j++)
    {
      for (int i=0; i<w; i++, ptr++) *ptr = color;
      ptr += fWidth-w;
    }
}

void Drawer::PutString(const PointZ& p, const Fonts & f, char * s)
{
  PointZ tp = p;
  int dx = f.GetDimX();
  tp.x -= (StrLen(s)*dx)/2;
  tp.y -= f.GetDimY()/2;
  for (int i=0; s[i]!=0; i++, tp.x+=dx)
    PutChar(tp, f, s[i]);
}

void Drawer::WriteString(const PointZ& p, const Fonts & f, char * s)
{
  PointZ tp = p;
  int dx = f.GetDimX();
  for (int i=0; s[i]!=0; i++, tp.x+=dx)
    PutChar(tp, f, s[i]);
}

void Drawer::LineCoefs(int x1, int y1, int x2, int y2,
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

void Drawer::NoClip(int x1,  int y1, realtype ,
  int x2,  int y2, realtype ,
  realtype c1, realtype c2)
{
  assert( (x1>=0)&&(x1<=fWidth)&&(y1>=0)&&(y1<=fHeight) );
  assert( (x2>=0)&&(x2<=fWidth)&&(y2>=0)&&(y2<=fHeight) );

  int x,y,dx,dy,s1,s2,interchange,e,l;
  LineCoefs(x1,y1,x2,y2,x,y,dx,dy,s1,s2,interchange,e,l);
  if (dx == 0) return;

  byte * ptr = fBuffer+x+y*fWidth;

  realtype c = c1;
  realtype ic = rapport( (c2-c1)*s1, conv(dx) );

  if (interchange == 1)
    for (int i=0; i<dx; i++)
      {
      	*ptr = byte(round(c));
      	if (e>=0) { ptr += s1; e = e-2*dx; }
      	ptr += l; e = e+2*dy;
      	inc(c,ic);
      }
  else
    for (int i=0; i<dx; i++)
      {
      	*ptr = byte(round(c));
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

void Drawer::Line(const PointZ &p1, const PointZ & p2)
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

void QuadMapCoefs(realtype xb,realtype xe,realtype l,
		  real & fa, real & fb, real & fc)
{
  real fxb  = roundf(xb);
  real fxe  = roundf(xe);
  real fl   = roundf(l);
  real fd;
  if ( fl > 1 )
    {
      fd = fl*(fxe-fxb)*(fxe-fxb);
      fa = fl-1;
      fb = fxb*(1-2*fl) + fxe;
      fc = fxb*(fxb*fl-fxe);
    }
  else
    {
      fd = (fxe-fxb)*(fxe-fxb);
      fa = fl-1;
      fb = -fxb*fl+fxe*(2-fl);
      fc = fxb*(fxb+fxe*(fl-2));
    }
  if (fabs(fd) < epsilonprecision) fd = 1;
  fa = fa/fd;
  fb = fb/fd;
  fc = fc/fd;
}

void QuadMapInit(real fa, real fb, real fc, realtype xd,
	realtype mxb, realtype mxe,
	realtype myb, realtype mye,
	realtype &mx, realtype &my,
	realtype &tx, realtype &ty,
	realtype &ax, realtype &ay)
{
  real fxd  = roundf(xd);
  real fmxb = roundf(mxb);
  real fmyb = roundf(myb);
  real fmxe = roundf(mxe);
  real fmye = roundf(mye);
  real fk = (fxd*(fa*fxd+fb)+fc);
  mx= fconv( fk*(fmxe-fmxb)+ fmxb );
  my= fconv( fk*(fmye-fmyb)+ fmyb );
  tx= fconv( ((fa+fa)*fxd+fa+fb)*(fmxe-fmxb) );
  ax= fconv( (fa+fa)*(fmxe-fmxb) );
  ty= fconv( ((fa+fa)*fxd+fa+fb)*(fmye-fmyb) );
  ay= fconv( (fa+fa)*(fmye-fmyb) );
}

void QuadMapCoefsInit(realtype xb, realtype xe, realtype l,
  realtype xd,
  realtype mxb, realtype mxe,
  realtype myb, realtype mye,
  realtype &mx, realtype &my,
  realtype &tx, realtype &ty,
  realtype &ax, realtype &ay)
{
  real fa, fb, fc;
  QuadMapCoefs(xb,xe,l, fa, fb, fc);
  QuadMapInit(fa, fb, fc, xd, mxb, mxe, myb, mye, mx, my, tx, ty, ax, ay);
}

void Drawer::Fill(int npoints, PointZ * points, byte color)
{
  #define flat
  #include "fill.cpp"
  #undef flat
}

void Drawer::Shade(int npoints, PointZ * points)
{
  #define gouraud
  #include "fill.cpp"
  #undef gouraud
}

void Drawer::Map(int npoints, PointZ * points,
  MapPoint * mappoints, const Image * ima)
{
  #define mapping
  #include "fill.cpp"
  #undef mapping
}

void Drawer::QuadMap(int npoints, PointZ * points,
  MapPoint * mappoints, const Image * ima)
{
  #define quadmapping
  #include "fill.cpp"
  #undef quadmapping
}

void Drawer::HypMap(int npoints, PointZ * points,
  MapPoint * mappoints, const Image * ima)
{
  #define hypmapping
  #include "fill.cpp"
  #undef hypmapping
}

void Drawer::Horiz(realtype xb, realtype xe,
  realtype zb, realtype ze, int y,byte color)
{
  #define flat
  #include "cliph.cpp"
  #include "horiz.cpp"
  //memset( fBuffer+fWidth*y+xd, xf-xd, color); // not working
  #undef flat
}

void Drawer::ShadeHoriz(realtype xb, realtype xe, realtype zb, realtype ze,
  int y, realtype cb, realtype ce)
{
  #define gouraud
  #include "cliph.cpp"
  #include "horiz.cpp"
  #undef gouraud
}

void Drawer::MapHoriz(realtype xb, realtype xe, realtype zb, realtype ze,
  int y, realtype mxb, realtype myb,
  realtype mxe, realtype mye, const Image * ima)
{
  #define mapping
  #include "cliph.cpp"
  #include "horiz.cpp"
  #undef mapping
}

void Drawer::QuadMapHoriz(realtype xb,realtype xe,realtype zb, realtype ze,
  int y, realtype mxb, realtype myb,
  realtype mxe, realtype mye,
  const Image * ima)
{
  #define quadmapping
  #include "cliph.cpp"
  #include "horiz.cpp"
  #undef quadmapping
}

void Drawer::HypMapHoriz(realtype xb,realtype xe,realtype zb, realtype ze,
  int y, realtype mxb, realtype myb,
  realtype mxe, realtype mye,
  const Image * ima)
{
  #define hypmapping
  #include "cliph.cpp"
  #include "horiz.cpp"
  #undef hypmapping
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

void Drawer::Ellipse(int CX, int CY,
  int x1, int y1, int x2, int y2, byte color)
{
  long dx1, dx2, rx1, rx2;
  long dy1, dy2, ry1, ry2;
  int k, mask;

  // if (x2,y2) is to the right {invert it}
  // counterclockwise by now
  if (x1*y2-y1*x2<0) x2=-x2,y2=-y2;

  k = ((domain(maximum(x1*x1+x2*x2, y1*y1+y2*y2))+1)>>1)+1;
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

void Drawer::FillEllipse(int CX, int CY,
  int x1, int y1, int x2, int y2, byte color)
{
  long dx1, dx2, rx1, rx2;
  long dy1, dy2, ry1, ry2;
  int k, mask;

  for (int l=0; l<fHeight; l++)
    { ellipse_xd[l] = 0; ellipse_xf[l]=0; }

  // if (x2,y2) is to the right {invert it}
  // counterclockwise by now
  if (x1*y2-y1*x2<0) x2=-x2,y2=-y2;

  k = ((domain(maximum(x1*x1+x2*x2, y1*y1+y2*y2))+1)>>1)+1;
  mask = (1<<k)-1;

  #define PIXEL(x,y) if(ellipse_xd[(y)]==0) ellipse_xd[(y)]=(x); else ellipse_xf[(y)]=(x)

  PIXEL(CX+x1,CY+y1);
  PIXEL(CX-x1,CY-y1);
  PIXEL(CX+x2,CY+y2);
  PIXEL(CX-x2,CY-y2);

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
      	  PIXEL(CX+x1,CY+y1);
      	  PIXEL(CX-x1,CY-y1);
      	}
      if (dx2 || dy2)
      	{
      	  PIXEL(CX+x2,CY+y2);
    	    PIXEL(CX-x2,CY-y2);
      	}
    }

  for (int y=0; y<fHeight; y++)
    if (ellipse_xd[y]!=0 || ellipse_xf[y]!=0)
      Horiz(conv(ellipse_xd[y]), conv(ellipse_xf[y]), 0,0, y, color);

}

void Drawer::Ellipse(PointZ center, PointZ xaxe, PointZ yaxe, byte color)
{
  Ellipse(center.x, center.y,
      	  xaxe.x-center.x, xaxe.y-center.y,
       	  yaxe.x-center.x, yaxe.y-center.y, color);
}

void Drawer::FillEllipse(PointZ center, PointZ xaxe, PointZ yaxe, byte color)
{
  FillEllipse(center.x, center.y,
	           xaxe.x-center.x, xaxe.y-center.y,
    	       yaxe.x-center.x, yaxe.y-center.y, color);
}

void Drawer::ShadeVert(int x, int yb, int ye, int cb, int ce)
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
  byte * Cur = fBuffer+fWidth*yb+x;
  for (int y=yb; y<=ye; y++, Cur+=fWidth)
    {
      *Cur = byte(round(ofsC));
      inc(ofsC,IncrC);
    }
}

