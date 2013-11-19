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
#include "Voxel.h"

int prof=32;

VoxelMap::VoxelMap(int pMaxColor)
{
  fMapZ= new realtype[MapX*MapY];
  fMapC=new Color[MapX*MapY];
  fMaxColor = pMaxColor;
  srand(1);
  for (int i=0; i<MapX*MapY; i++) fMapZ[i]=0;
  MapZ(0,0) = MaxZ/2;
  SetZ(0, 0, MapX, MapY);
  SetColor();
  oldy = new int[WinX];
  horiz = new int[WinX];
  oldc = new Color[WinX];
  tmy = new real [prof];
  tstepy = new real [prof];
  tbegu = new real [prof];
  tstepu = new real [prof];
  tbegv = new real [prof];
  tstepv = new real [prof];
}

VoxelMap::~VoxelMap()
{
  delete [] fMapZ;
  delete [] fMapC;
  delete [] oldy;
  delete [] horiz;
  delete [] oldc;
  delete [] tmy;
  delete [] tstepy;
  delete [] tbegu;
  delete [] tstepu;
  delete [] tbegv;
  delete [] tstepv;
}

int VoxelMap::Noise(int pValue1, int pValue2, int pint)
{
  if (pint == 0) pint = 1;
  int r = (rand()%pint*2-pint);
  r += (pValue1+pValue2)/2;
  r = r<0?0:r;
  r = r>=MaxZ?MaxZ-1:r;
  return r;
}

void VoxelMap::SetZ(int xb, int yb, int xe, int ye)
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

void VoxelMap::SetZ1(int xb, int yb, int xe, int ye)
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

void VoxelMap::BlurZ()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)
      //MapZ(i,j) = (k1*MapZ(i,j) + k2*MapZ(i+3,j-2) + k3*MapZ(i-2,j+4))/(k1+k2+k3);
      MapZ(i,j) = 
      (k1*MapZ(i,j) + k2*MapZ(i+1,j-1) + k3*MapZ(i-1,j+1))/(k1+k2+k3);
}

void VoxelMap::BlurZ0()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)
      MapZ(i,j) = 
      (k1*MapZ(i,j) + k2*MapZ(i-1,j-1) + k3*MapZ(i+1,j+1))/(k1+k2+k3);
}

void VoxelMap::BlurZ1()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)
      MapZ(i,j) = ( MapZ(i,j) + MapZ(i+1,j) )/2;
}

void VoxelMap::BlurZ2()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)
      MapZ(i,j) = ( MapZ(i,j) + MapZ(i,j+1) )/2;
}

void VoxelMap::BlurC()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX-1; i++)
      MapC(i,j) = ( MapC(i+1,j) + MapC(i,j) )/2;
}

void VoxelMap::SetColor()
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

void VoxelMap::Satur()
{
  for (int j=0; j<MapY; j++)
    for (int i=0; i<MapX; i++)	  
    {
      MapC(i,j) = MapC(i,j)>=fMaxColor-2?fMaxColor-3:MapC(i,j);   
      if (MapZ(i,j)>water) { MapZ(i,j) = water; MapC(i,j) = 254; }
      MapC(i,j) = MapC(i,j)<=0?0:MapC(i,j);
    }
}

void VoxelMap::PutColor(Drawer2D & w)
{
  for (int j=0; j<w.Height(); j++)
    for (int i=0; i<w.Width(); i++)
      w.Pixel(PointZ(i,j, MapC(i,j)));
}

void VoxelMap::PutMapZ(Drawer2D & w)
{
  for (int j=0; j<w.Height(); j++)
    for (int i=0; i<w.Width(); i++)
      w.Pixel(PointZ(i,j, MapZ(i,j)));
}

void VoxelMap::precalc(real pscrz, real pdecal, real t, Drawer2D &w)
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

void VoxelMap::VOXEL(real ou, real ov, Drawer2D & w)
{
  for (int k0 = 0; k0<WinX; k0++) horiz[k0]=WinY;
  for (int k = 0; k<WinX; k++) {oldy[k] = WinY; oldc[k] = NbColors/2;}
  for (int i=0; i<prof; i++)
  {
    real my = tmy[i]; real stepy = tstepy[i]; real stepu = tstepu[i];
    real mu = tbegu[i]+ou; real stepv = tstepv[i]; real mv = tbegv[i]+ov;

    int   imu = int(mu);    int imv = int(mv);
    int   oldny = int( real (stepy*MapZ( imu, imv) + my) );
    Color  oldnc = MapC( mu, mv );
    int   oldmx=0; 
    for(int mx=0; mx<WinX; mx++, mu+=stepu, mv+=stepv)
    {
      int inewmu = int(mu);   int inewmv = int(mv);
      if ( ( imu!=inewmu && c>marge ) 
        || (imv!=inewmv && s>marge ) 
        || (mx==WinX-1) )
      { 
        int ny = int( real (stepy*MapZ( inewmu, inewmv ) + my) );
        Color nc = MapC( inewmu, inewmv );
        real pentey = real(ny-oldny)/real(mx-oldmx);
        real pentec = real(nc-oldnc)/real(mx-oldmx);
        real curny = oldny;  real curnc = oldnc;
        int midx = (oldmx+mx)/2;


        for (int tmpmx0=oldmx; tmpmx0<midx; tmpmx0++, 
          curny+=pentey+penteadd, curnc+=pentec)
        {
          int icurny = int(curny);
          Color icurnc = Color(curnc);
          if ( icurny<oldy[tmpmx0] && icurny<horiz[tmpmx0] )
          {
            //for (int slow=0; slow<100; slow ++)
            w.ShadeVert(tmpmx0, horiz[tmpmx0]-1, icurny , 
              oldc[tmpmx0], icurnc);
            //if (stop) scr.DisplayImage(&w);
            horiz[tmpmx0] = icurny;
          }
          oldy[tmpmx0] = icurny;
          oldc[tmpmx0] = icurnc;
        }

        for (int tmpmx1=midx; tmpmx1<mx; tmpmx1++, 
          curny+=pentey-penteadd, curnc+=pentec)
        {
          int icurny = int(curny);
          Color icurnc = Color(curnc);
          if ( icurny<oldy[tmpmx1] && icurny<horiz[tmpmx1] )
          {
            //for (int slow=0; slow<100; slow ++)
            w.ShadeVert(tmpmx1, horiz[tmpmx1]-1, icurny , 
              oldc[tmpmx1], icurnc);
            //if (stop) scr.DisplayImage(&w);
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
  for(int mx=0; mx<WinX; mx++)
  {
    w.ShadeVert(mx, 0, horiz[mx], 
      255, 255);
  }
}

