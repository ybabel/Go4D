#include "FireDemo.h"

#define trunc(x) (((x)<0.0)?int((x)):int(x)-1)
#define SetFilter(i,a,b) filter[x+WinX*y][i]= \
    trunc(a)+WinX/2+WinX*(trunc(b)+WinY/2)

MainApp::MainApp(Device & aDevice)
  : inherited(aDevice),
  drawer(fImage.Width(), fImage.Height())
{
  filter = new FilterMatrix[fImage.Len()];
  sun = false;
  for (int y=0; y<WinY; y++)
    for (int x=0; x<WinX; x++)
    {
      real rx = (real)x-real(WinX)/2.0;
      real ry = (real)y-real(WinY)/2.0;
      real norme = sqrt(sqr(rx)+sqr(ry));
      if (norme<0.001)
      {
        SetFilter(0, 0, 0);
        SetFilter(1, 0, 0);
        SetFilter(2, 0, 0);
        SetFilter(3, 0, 0);
        SetFilter(4, 0, 0);
        SetFilter(5, 0, 0);
        SetFilter(6, 0, 0);
        SetFilter(7, 0, 0);
        continue;
      }
      real nrx = rx/norme;
      real nry = ry/norme;

      SetFilter(0, rx+nry, ry-nrx);
      SetFilter(1, rx-nry, ry-nrx);

      SetFilter(2, rx+nry-nrx, ry-nrx-nry);
      SetFilter(3, rx-nry-nrx, ry-nrx-nry);
      SetFilter(4, rx    -nrx, ry    -nry);

      SetFilter(5, rx-nry-nrx*2.0, ry-nrx-nry*2.0);
      SetFilter(6, rx+nry-nrx*2.0, ry-nrx-nry*2.0);
      SetFilter(7, rx    -nrx*2.0, ry    -nry*2.0);
    }
  Init();
}

void MainApp::Init()
{
  if (sun)
  {
    particleSize = 2;
    burn = 3;
    margin = 64;
  }
  else
  {
    particleSize = 2;
    burn = 2;
    margin = 112;
  }
  explosionangle = 0.0;
  explosionsize = 0;
}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
    case kUP :
      fImage.ClearAll();
      margin-=8;
      margin=margin<0?0:margin;
      break;
    case kDN :
      fImage.ClearAll();
      margin+=8;
      margin=margin>WinX?WinX:margin;
      break;
    case kRI :
      particleSize++;
      particleSize=particleSize<1?1:particleSize;
      break;
    case kLE :
      particleSize--;
      break;
    case kLO :
      burn++;
      break;
    case kHI :
      burn--;
      burn=burn<0?0:burn;
      break;
    case kTB :
      sun = !sun;
      fImage.ClearAll();
      Init();
      break;
    case kSP :
      Init();
      break;
    case kKA :
      explosionangle = real(random(360))*2.0*3.1415/360.0;
      explosionsize = 24;
      explosiondir = random(2)<1?-1.0:1.0;
      break;
    default : inherited::OnKeyPressed(aKey);
  }
}




#define WinX2 2*WinX

Color MainApp::Blur1(Color * old, int ofs)
{
  Color color = Color(int(
    (int)old[ofs+1]+
    //(int)old[ofs]+
    (int)old[ofs-1]+

    (int)old[ofs+WinX+1]+
    (int)old[ofs+WinX]+
    (int)old[ofs+WinX-1]+

    (int)old[ofs+WinX2+1]+
    (int)old[ofs+WinX2]+
    (int)old[ofs+WinX2-1]+
    0
    ) >> 3);
  return color;
}

Color MainApp::Blur2(Color * old, int ofs)
{
  Color color = Color(int(
    (int)old[ofs+1]+
    (int)old[ofs]*2+
    (int)old[ofs-1]+

    (int)old[ofs+WinX+1]+
    (int)old[ofs+WinX]+
    (int)old[ofs+WinX-1]+

    (int)old[ofs+WinX2+1]+
    (int)old[ofs+WinX2]+
    (int)old[ofs+WinX2-1]+

    (int)old[ofs-WinX2+1]+
    (int)old[ofs-WinX2]+
    (int)old[ofs-WinX2-1]+

    (int)old[ofs-WinX+1]+
    (int)old[ofs-WinX]+
    (int)old[ofs-WinX-1]+
    0
    ) >> 4);
  return color;
}

Color MainApp::Blur3(Color * old, int ofs)
{
  Color color = Color(int(
    (int)old[ofs+1]+
    //(int)old[ofs]+
    (int)old[ofs-1]+

    (int)old[ofs+WinX+1]+
    (int)old[ofs+WinX]+
    (int)old[ofs+WinX-1]+

    (int)old[ofs-WinX+1]+
    (int)old[ofs-WinX]+
    (int)old[ofs-WinX-1]+
    0
    ) >> 3);
  return color;
}

Color MainApp::Blur4(Color * old, int ofs)
{
  Color color = Color(int(
    (int)old[filter[ofs][0]]+
    (int)old[filter[ofs][1]]+
    (int)old[filter[ofs][2]]+
    (int)old[filter[ofs][3]]+
    (int)old[filter[ofs][4]]+
    (int)old[filter[ofs][5]]+
    (int)old[filter[ofs][6]]+
    (int)old[filter[ofs][7]]+
    0
    ) >> 3);
  return color;
}

void MainApp::Alea1()
{
  for (int i=0; i<32; i++)
  {
    int x = random(WinX);
    drawer.BoxZ(PointZ(x, WinY-1-particleSize, 255, 0), particleSize, particleSize);
  }
}

void MainApp::Alea2()
{
  for (int i=0; i<64; i++)
  {
    int x = random(WinX-particleSize);
    int y = random(WinY-particleSize)+particleSize;
    drawer.BoxZ(PointZ(x, y-particleSize, 255, 0), particleSize, particleSize);
  }
}

void MainApp::Alea3()
{
  int i;
  for (i=0; i<64; i++)
  {
    int x = i*2+64;
    int y = 64;
    drawer.BoxZ(PointZ(x, y-particleSize, 255, 0), particleSize, particleSize);
    x = i*2+64;
    y = 256-64;
    drawer.BoxZ(PointZ(x, y-particleSize, 255, 0), particleSize, particleSize);
  }
  for (i=0; i<64; i++)
  {
    int y = i*2+64;
    int x = 64;
    drawer.BoxZ(PointZ(x, y-particleSize, 255, 0), particleSize, particleSize);
    y = i*2+64;
    x = 256-64;
    drawer.BoxZ(PointZ(x, y-particleSize, 255, 0), particleSize, particleSize);
  }
}

void MainApp::Alea4()
{
  for (int i=0; i<64; i++)
  {
    real angle=real(random(360))*2.0*3.1415/360.0;
    int x = int(real(real(margin)*cos(angle)))+WinX/2;
    int y = int(real(real(margin)*sin(angle)))+WinY/2;
    drawer.BoxZ(PointZ(x, y, 255, 0), particleSize, particleSize);
  }
  if (explosionsize>0)
  {
    explosiondir = real(random(16))<2.0?explosiondir*-1.0:explosiondir;
    explosionsize--;
    explosionangle+=2.0*2.0*3.1415/360.0*real(explosiondir);
    int newSize = (particleSize+8);
    int x = int(real(real(margin+newSize/2)*cos(explosionangle)))+WinX/2;
    int y = int(real(real(margin+newSize/2)*sin(explosionangle)))+WinY/2;
    drawer.BoxZ(PointZ(x-newSize/2, y-newSize/2, 255, 0), newSize, newSize);
  }
}

void MainApp::Cycle()
{
  Draw();
};

void MainApp::Draw()
{ 
  if (sun)
    Alea4();
  else
    Alea1();
  int usemargin=sun?0:margin;

  for (int y=2; y<WinY-2; y++)
    for (int x=usemargin; x<WinX-1-usemargin; x++)
    {
      int ofs = x+y*WinX;
      Color * old = drawer.GetBuffer();
      Color color;
      if (sun)
        color = Blur4(old, ofs);
      else
        color = Blur3(old, ofs);

      color = color<burn?0:color-burn;
      fImage.Pixel(x, y-1, color);
    }

  drawer.Copy(fImage);
  inherited::Draw();
}



