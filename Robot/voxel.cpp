/*
  AUTHOR    : Babel Yoann
  TITLE     : voxel.cc
  DATE      : 12/11/95
  OBJECT    : voxel landscape demo
              using mars demo technique not designed to be reused, 
              just a small try to see if the method is working well
	      new algorithm : interpolate the points by 2 lines
	      parameter that affect the qualiti of resutl : 
	      marge, margedepth, prof, penteadd
*/


#ifdef LIBGPP
#include <ostream.h>
#endif

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include "device.h"
#include "drawer.h"

#define MapX 256
#define MapY 256
#define WinX 400
#define WinY 300
#define MaxZ 64
#define k1 6
#define k2 19
#define k3 16
#define NbColors 255
int prof =50;
#define focal 0.7
#define echx 256
#define echy 128
#define water  40

class map
{
  public :
  map(int pMaxColor);
  ~map();
  void PutColor(Drawer & w);
  void PutMapZ(Drawer & w);
  void precalc(real, real, real, Drawer &);
  void VOXEL(real, real, Drawer & w);
  void BlurZ();
  void BlurZ0();
  void BlurZ1();
  void BlurZ2();
  void BlurC();
  void SetColor();
  void Satur();
  realtype * fMapZ;
  private :
  byte * fMapC;
  int fMaxColor;
  void SetZ(int xb, int yb, int xe, int ye);
  void SetZ1(int xb, int yb, int xe, int ye);
  int Noise(int pValue1, int pValue2, int pHeight);
  int * oldy, * horiz;
  byte * oldc;
  real scrx, oz, scrz, decal;
  real teta;
  real * tmy, * tstepy, * tbegu, *tbegv, * tstepu, * tstepv;
  real c,s; //cos teta, sin teta
};


Device scr;
Drawer w(WinX,WinY);
map m(NbColors);


#define MapZ(x,y) fMapZ[(int(x)&(MapX-1))+MapX*(int(y)&(MapY-1))]
#define MapC(x,y) fMapC[(int(x)&(MapX-1))+MapX*(int(y)&(MapY-1))]
#define CurMapZ(x,y) CurMapZ[(int(x)&(MapX-1))+MapX*(int(y)&(MapY-1))]
#define CurMapC(x,y) CurMapC[(int(x)&(MapX-1))+MapX*(int(y)&(MapY-1))]


map::map(int pMaxColor)
{
  fMapZ= new realtype[MapX*MapY];
  fMapC=new byte[MapX*MapY];
  fMaxColor = pMaxColor;
  srand(1);
  for (int i=0; i<MapX*MapY; i++) fMapZ[i]=0;
  MapZ(0,0) = MaxZ/2;
  SetZ(0, 0, MapX, MapY);
  SetColor();
  oldy = new int[WinX];
  horiz = new int[WinX];
  oldc = new byte[WinX];
  tmy = new real [prof];
  tstepy = new real [prof];
  tbegu = new real [prof];
  tstepu = new real [prof];
  tbegv = new real [prof];
  tstepv = new real [prof];
}

map::~map()
{
  delete fMapZ;
  delete fMapC;
  delete oldy;
  delete horiz;
  delete oldc;
  delete tmy;
  delete tstepy;
  delete tbegu;
  delete tstepu;
  delete tbegv;
  delete tstepv;
}

int map::Noise(int pValue1, int pValue2, int pint)
{
  if (pint == 0) pint = 1;
  int r = (rand()%pint*2-pint);
  r += (pValue1+pValue2)/2;
  r = r<0?0:r;
  r = r>=MaxZ?MaxZ-1:r;
  return r;
}

void map::SetZ(int xb, int yb, int xe, int ye)
{
  int Zbb = MapZ(xb,yb);
  int Zbe = MapZ(xb,ye);
  int Zee = MapZ(xe,ye);
  int Zeb = MapZ(xe,yb);
  int xbe = (xb+xe)/2;
  int ybe = (yb+ye)/2;
  int lx = (xe-xb);
  int ly = (ye-yb);
  if (MapZ(xb,ybe)==0)
    MapZ(xb,ybe) = Noise(Zbb, Zbe, ly/2);
  if (MapZ(xbe,ye)==0) 
    MapZ(xbe,ye) = Noise(Zbe, Zee, lx/2);
  if (MapZ(xe,ybe)==0) 
    MapZ(xe,ybe) = Noise(Zee, Zeb, ly/2);
  if (MapZ(xbe,yb)==0) 
    MapZ(xbe,yb) = Noise(Zeb, Zbb, lx/2);
  if (MapZ(xbe,ybe)==0) 
    MapZ(xbe,ybe) = Noise( (Zbb+Zbe)/2, (Zee+Zeb)/2, (ly+lx)/8 );
  if (lx<=2&&ly<=2) return;
  SetZ(xb,ybe, xbe,ye);
  SetZ(xbe,ybe, xe,ye);
  SetZ(xb,yb, xbe,ybe);
  SetZ(xbe,yb, xe,ybe);
}

void map::SetZ1(int xb, int yb, int xe, int ye)
{
  int Zbb = MapZ(xb,yb);
  int Zbe = MapZ(xb,ye);
  int Zee = MapZ(xe,ye);
  int Zeb = MapZ(xe,yb);
  int xbe = (xb+xe)/2;
  int ybe = (yb+ye)/2;
  int lx = (xe-xb);
  int ly = (ye-yb);
  MapZ(xbe,ybe) = Noise( (Zbb+Zbe)/2, (Zee+Zeb)/2, (ly+lx)/8 );
  if (lx<=1&&ly<=1) return;
  SetZ1(xb,ybe, xbe,ye);
  SetZ1(xbe,ybe, xe,ye);
  SetZ1(xb,yb, xbe,ybe);
  SetZ1(xbe,yb, xe,ybe);
}

void map::BlurZ()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)
//MapZ(i,j) = (k1*MapZ(i,j) + k2*MapZ(i+3,j-2) + k3*MapZ(i-2,j+4))/(k1+k2+k3);
      MapZ(i,j) = 
	(k1*MapZ(i,j) + k2*MapZ(i+1,j-1) + k3*MapZ(i-1,j+1))/(k1+k2+k3);
}

void map::BlurZ0()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)
      MapZ(i,j) = 
	(k1*MapZ(i,j) + k2*MapZ(i-1,j-1) + k3*MapZ(i+1,j+1))/(k1+k2+k3);
}

void map::BlurZ1()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)
      MapZ(i,j) = ( MapZ(i,j) + MapZ(i+1,j) )/2;
}

void map::BlurZ2()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)
      MapZ(i,j) = ( MapZ(i,j) + MapZ(i,j+1) )/2;
}

void map::BlurC()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX-1; i++)
      MapC(i,j) = ( MapC(i+1,j) + MapC(i,j) )/2;
}

void map::SetColor()
{
  int MaxDiff = 0;
  int MinDiff = 0;
  for (int j0=0; j0<MapY; j0++)
    for (int i=0; i<MapX; i++)
      {
	int d = MapZ(i+1,j0)-MapZ(i,j0);
	if (d>MaxDiff) 
	  MaxDiff = d;
	if (d<MinDiff)
	  MinDiff = d;
      }
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)	  
      {
	int DeltaZ = (MapZ(i+1,j)-MapZ(i,j))*2;
	//MapC(i,j) = MapZ(i,j)*16;
	if (DeltaZ<0) MapC(i,j) = abs(DeltaZ)*8;
	else MapC(i,j) = DeltaZ*16+fMaxColor/2;
      }
}

void map::Satur()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)	  
      {
	MapC(i,j) = MapC(i,j)>=fMaxColor-2?fMaxColor-3:MapC(i,j);   
	if (MapZ(i,j)>water) { MapZ(i,j) = water; MapC(i,j) = 254; }
	MapC(i,j) = MapC(i,j)<=0?0:MapC(i,j);
      }
}

void map::PutColor(Drawer & w)
{
  for (int j=0; j<w.Height(); j++)
    for (int i=0; i<w.Width(); i++)
      w.Pixel(PointZ(i,j, MapC(i,j)));
}

void map::PutMapZ(Drawer & w)
{
  for (int j=0; j<w.Height(); j++)
    for (int i=0; i<w.Width(); i++)
      w.Pixel(PointZ(i,j, MapZ(i,j)));
}

int  margedepth = 0;

void map::precalc(real pscrz, real pdecal, real t, Drawer &w)
{
  assert(pdecal != 0);
  scrz = pscrz;
  decal = pdecal;
  scrx = real(w.Width()/2);
  oz = scrz+decal;
  teta = t;
  c = cos(teta);
  s = sin(teta);

  real di = focal+1+margedepth; // distance
  for (int i=0; i<prof; i++, di += 1)
    {
      tmy[i]     = decal-focal*oz/di;
      tstepy[i]  = focal/di*echy;
      real h    = di/(focal*echx);
      real larg = -scrx*h;
      tbegu[i]   =  c*larg + s*di;
      tbegv[i]   = -s*larg + c*di;
      tstepu[i]  = c*h;
      tstepv[i]  = -s*h; 
    }
  c = fabs(c); //inmportant for the test behind
  s = fabs(s); 
} 

int stop = 0;
#define marge 1/sqrt(2)
real penteadd = -0.3;

void map::VOXEL(real ou, real ov, Drawer & w)
{
  for (int k0 = 0; k0<WinX; k0++) horiz[k0]=WinY;
  for (int k = 0; k<WinX; k++) {oldy[k] = WinY; oldc[k] = NbColors/2;}
  for (int i=0; i<prof; i++)
    {
      real my = tmy[i]; real stepy = tstepy[i]; real stepu = tstepu[i];
      real mu = tbegu[i]+ou; real stepv = tstepv[i]; real mv = tbegv[i]+ov;
      
      int   imu = int(mu);    int imv = int(mv);
      int   oldny = int( real (stepy*MapZ( imu, imv) + my) );
      byte  oldnc = MapC( mu, mv ); 
      int   oldmx=0; 
      for(int mx=0; mx<WinX; mx++, mu+=stepu, mv+=stepv)
	{
	  int inewmu = int(mu);   int inewmv = int(mv);
	  if ( ( imu!=inewmu && c>marge ) 
	      || (imv!=inewmv && s>marge ) 
	      || (mx==WinX-1) ) 
	    { 
	      int ny = int( real (stepy*MapZ( inewmu, inewmv ) + my) );
	      byte nc = MapC( inewmu, inewmv );
	      real pentey = real(ny-oldny)/real(mx-oldmx);
	      real pentec = real(nc-oldnc)/real(mx-oldmx);
	      real curny = oldny;  real curnc = oldnc;
              int midx = (oldmx+mx)/2;


	      for (int tmpmx0=oldmx; tmpmx0<midx; tmpmx0++, 
		   curny+=pentey+penteadd, curnc+=pentec)
		{
		  int icurny = int(curny);
		  byte icurnc = byte(curnc);
		  if ( icurny<oldy[tmpmx0] && icurny<horiz[tmpmx0] )
		    {
		      w.ShadeVert(tmpmx0, horiz[tmpmx0]-1, icurny , 
				  oldc[tmpmx0], icurnc);
		      if (stop) scr.DisplayImage(&w);
		      horiz[tmpmx0] = icurny;
		    }
		  oldy[tmpmx0] = icurny;
		  oldc[tmpmx0] = icurnc;
		}
	      
	      for (int tmpmx1=midx; tmpmx1<mx; tmpmx1++, 
		   curny+=pentey-penteadd, curnc+=pentec)
		{
		  int icurny = int(curny);
		  byte icurnc = byte(curnc);
		  if ( icurny<oldy[tmpmx1] && icurny<horiz[tmpmx1] )
		    {
		      w.ShadeVert(tmpmx1, horiz[tmpmx1]-1, icurny , 
				  oldc[tmpmx1], icurnc);
		      if (stop) scr.DisplayImage(&w);
		      horiz[tmpmx1] = icurny;
		    }
		  oldy[tmpmx1] = icurny;
		  oldc[tmpmx1] = icurnc;
		}
              

	      oldmx = mx; oldny = ny; oldnc = nc;
	      imu = inewmu; imv = inewmv;
	    }
	}
    }
}

#define Step 2
  
int main(void)
{
  int following = 1;
  scr.AddImage(&w);
  scr.VoxelPalette(&w);
  m.BlurZ();
  m.BlurZ0();
  m.BlurZ1();
  m.BlurZ2();
  m.SetColor();
  m.BlurC();
  m.Satur();
  m.PutMapZ(w);
  scr.DisplayImage(&w);
  m.PutColor(w);
  scr.DisplayImage(&w);
  real x=0,y=30, d=64;
  real z = -MaxZ*4+200;
  real t=0;
  int r;
  m.precalc(z,d,t,w);
  scr.AutoRepeatOff();
  while ( (r=scr.Move(&w))!=kEND )
    {
      if (r&kUPMask) {y+=Step*cos(t); x+=Step*sin(t);}
      if (r&kDNMask) {y-=Step*cos(t); x-=Step*sin(t);}
      if (r&kLEMask) {t-=M_PI/32; m.precalc(z,d,t,w);}
      if (r&kRIMask) {t+=M_PI/32; m.precalc(z,d,t,w);}
      switch (r & kKeyMask)
	{
        case kFW : 
	  prof += 1; 
	  #ifdef LIBGPP
	  cout << "prof=" << prof << kCR; 
	  #endif
	  m.precalc(z,d,t,w); 
	  break;
        case kBW : 
	  prof -= 1;
	  #ifdef LIBGPP
	  cout << "prof=" << prof << kCR; 
	  #endif
	  m.precalc(z,d,t,w); 
	  break;
        case kLO : 
	  margedepth += 1;
	  #ifdef LIBGPP
	  cout << "margedepth=" << margedepth << kCR ; 
	  #endif
	  m.precalc(z,d,t,w); 
	  break;
        case kHI : 
	  margedepth -= 1; 
	  #ifdef LIBGPP
	  cout << "margedepth=" << margedepth << kCR ; 
	  #endif
	  m.precalc(z,d,t,w); 
	  break;
	case kSP : stop = 1-stop; break;  
	case kKU : scr.DefaultPalette(&w); break;
	case kKI : scr.VoxelPalette(&w); break;
	case kKQ : 
	  z+=echy; 
	  m.precalc(z,d,t,w);
	  #ifdef LIBGPP
	  cout << "z=" << z << kCR; 
	  #endif
	  break;
	case kKW : 
	  z-=echy;
	  m.precalc(z,d,t,w); 
	  #ifdef LIBGPP
	  cout << "z=" << z << kCR;  
	  #endif
	  break;
	case kKA : 
	  d+=Step*8; 
	  m.precalc(z,d,t,w); 
	  #ifdef LIBGPP
	  cout << "d=" << d << kCR;  
	  #endif
	  break;
	case kKZ : 
	  d-=Step*8; 
	  m.precalc(z,d,t,w); 
	  #ifdef LIBGPP
	  cout << "d=" << d << kCR;  
	  #endif
	  break;
	case kTB : following = 1-following; break;
	} 
      w.Clear(255);
      real zmin = (m.MapZ(x+Step*sin(t)*(focal+1),
			  y+Step*cos(t)*(focal+1))*echy - echy*10);
      if ( (z > zmin) || following ) 
	{
	  z=  zmin; 
	  m.precalc(z,d,t,w); 
	}
      m.VOXEL(x,y,w);
      scr.DisplayImage(&w);
    }
}








