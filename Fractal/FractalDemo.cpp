//---------------------------------------------------------------------------
#include "FractalDemo.h"

MainApp::MainApp(Device & aDevice)
  : inherited(aDevice),
  drawer(fImage, fImage.Width(), fImage.Height())
{
  Init();
#ifndef UNDER_CE
  Draw();
#endif
}

void MainApp::Init()
{
  startr = 0.0;
  starti = 0.0;
  fractype = ftMandel;
  fracmode = fmMove;
  maxZoom = WinX;
  offsX = 0;
  offsY = 0;
  maxIter = 32;
  ColorPrecision = 4;
  paramStep=0.1;
}

#define zoomStep 1.1
#define offsStep 10

real changebase(int i, real Max, real offs)
{
  real rMax = Max/2.0;
  real roffs = offs*rMax/WinX;
  real ri = real(i+roffs);

  return (ri)/rMax;
}

int Mandelbrot(real startr, real starti, real cr, real ci, int MaxIter)
{
  real zr = startr;
  real zi = starti;
  int iterations=0;
  while ( ((zr*zr+zi*zi) < 4) && (iterations<MaxIter) )
  {
    real tmpzr = zr*zr - zi*zi + cr;
    real tmpzi = 2.0*zr*zi + ci;
    zr = tmpzr;
    zi = tmpzi;
    iterations++;
  }
  return iterations;
}

#define COLORTEST(a,b) (abs(a-b)<ColorPres)
#define SILVERMANDEL(x, y) Mandelbrot(startr, starti, changebase(x-WinX/2, maxZoom, offsX), changebase(y-WinY/2, maxZoom, offsY), MaxIter)

void SilverMandelbrotCorner(Drawer2D & drawer,
  real maxZoom, real offsX, real offsY,
  real startr, real starti, int MaxIter, int ColorPres,
  int xmin, int xmax, int ymin, int ymax,
  int UpLeft, int UpRight, int BottomLeft, int BottomRight)
{
  if ( ((xmax-xmin)<2) && ((ymin-ymax)<2) )
  {
    static_cast<Image>(drawer).Pixel(xmin, ymin, Color(UpLeft));
    return;
  }
  int MiddleX=(xmin+xmax)/2;
  int MiddleY=(ymin+ymax)/2;
  int Middle=SILVERMANDEL(MiddleX, MiddleY);

  if ( COLORTEST(UpLeft,UpRight)
    &&COLORTEST(UpLeft,BottomLeft)
    &&COLORTEST(UpLeft,BottomRight)
    &&COLORTEST(UpLeft,Middle)
    )
  {
    drawer.FastBox(xmin, xmax, ymin, ymax, Color(Middle));
  }
  else
  {
    int MiddleUp=SILVERMANDEL(MiddleX, ymin);
    int MiddleBottom=SILVERMANDEL(MiddleX, ymax);
    int MiddleLeft=SILVERMANDEL(xmin, MiddleY);
    int MiddleRight=SILVERMANDEL(xmax, MiddleY);

    SilverMandelbrotCorner(drawer, maxZoom, offsX, offsY, startr, starti, MaxIter, ColorPres,
      MiddleX, xmax, MiddleY, ymax, Middle, MiddleRight, MiddleBottom, BottomRight);

    SilverMandelbrotCorner(drawer, maxZoom, offsX, offsY, startr, starti, MaxIter, ColorPres,
      xmin, MiddleX, MiddleY, ymax, MiddleLeft, Middle, BottomLeft, MiddleBottom);

    SilverMandelbrotCorner(drawer, maxZoom, offsX, offsY, startr, starti, MaxIter, ColorPres,
      MiddleX, xmax, ymin, MiddleY, MiddleUp, UpRight, Middle, MiddleRight);

    SilverMandelbrotCorner(drawer, maxZoom, offsX, offsY, startr, starti, MaxIter, ColorPres,
      xmin, MiddleX, ymin, MiddleY, UpLeft, MiddleUp, MiddleLeft, Middle);
  }
}

void SilverMandelbrot(Drawer2D & drawer,
  real maxZoom, real offsX, real offsY,
  real startr, real starti, int MaxIter, int ColorPres,
  int xmin, int xmax, int ymin, int ymax)
{
  int UpLeft = SILVERMANDEL(xmin, ymin);
  int UpRight = SILVERMANDEL(xmax, ymin);
  int BottomLeft = SILVERMANDEL(xmin, ymax);
  int BottomRight = SILVERMANDEL(xmax, ymax);
  SilverMandelbrotCorner(drawer, maxZoom, offsX, offsY, startr, starti, MaxIter, ColorPres,
    xmin, xmax, ymin, ymax, UpLeft, UpRight, BottomLeft, BottomRight);
}

#define SILVERJULIA(x, y) Mandelbrot(changebase(x-WinX/2, maxZoom, offsX), changebase(y-WinY/2, maxZoom, offsY), startr, starti, MaxIter)

void SilverJuliaCorner(Drawer2D & drawer,
  real maxZoom, real offsX, real offsY,
  real startr, real starti, int MaxIter, int ColorPres,
  int xmin, int xmax, int ymin, int ymax,
  int UpLeft, int UpRight, int BottomLeft, int BottomRight)
{
  if ( ((xmax-xmin)<2) && ((ymin-ymax)<2) )
  {
    static_cast<Image>(drawer).Pixel(xmin, ymin, Color(UpLeft));
    return;
  }
  int MiddleX=(xmin+xmax)/2;
  int MiddleY=(ymin+ymax)/2;
  int Middle=SILVERJULIA(MiddleX, MiddleY);

  if ( COLORTEST(UpLeft,UpRight)
    &&COLORTEST(UpLeft,BottomLeft)
    &&COLORTEST(UpLeft,BottomRight)
    &&COLORTEST(UpLeft,Middle)
    )
  {
    drawer.FastBox(xmin, xmax, ymin, ymax, Color(Middle));
  }
  else
  {
    int MiddleUp=SILVERJULIA(MiddleX, ymin);
    int MiddleBottom=SILVERJULIA(MiddleX, ymax);
    int MiddleLeft=SILVERJULIA(xmin, MiddleY);
    int MiddleRight=SILVERJULIA(xmax, MiddleY);

    SilverJuliaCorner(drawer, maxZoom, offsX, offsY, startr, starti, MaxIter, ColorPres,
      MiddleX, xmax, MiddleY, ymax, Middle, MiddleRight, MiddleBottom, BottomRight);

    SilverJuliaCorner(drawer, maxZoom, offsX, offsY, startr, starti, MaxIter, ColorPres,
      xmin, MiddleX, MiddleY, ymax, MiddleLeft, Middle, BottomLeft, MiddleBottom);

    SilverJuliaCorner(drawer, maxZoom, offsX, offsY, startr, starti, MaxIter, ColorPres,
      MiddleX, xmax, ymin, MiddleY, MiddleUp, UpRight, Middle, MiddleRight);

    SilverJuliaCorner(drawer, maxZoom, offsX, offsY, startr, starti, MaxIter, ColorPres,
      xmin, MiddleX, ymin, MiddleY, UpLeft, MiddleUp, MiddleLeft, Middle);
  }
}

void SilverJulia(Drawer2D & drawer,
  real maxZoom, real offsX, real offsY,
  real startr, real starti, int MaxIter, int ColorPres,
  int xmin, int xmax, int ymin, int ymax)
{
  int UpLeft = SILVERJULIA(xmin, ymin);
  int UpRight = SILVERJULIA(xmax, ymin);
  int BottomLeft = SILVERJULIA(xmin, ymax);
  int BottomRight = SILVERJULIA(xmax, ymax);
  SilverJuliaCorner(drawer, maxZoom, offsX, offsY, startr, starti, MaxIter, ColorPres,
    xmin, xmax, ymin, ymax, UpLeft, UpRight, BottomLeft, BottomRight);
}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
    case kUP :
      if (fracmode==fmParam)
        starti+=paramStep;
      else if (fracmode==fmZoom)
        maxZoom*=zoomStep;
      else if (fracmode==fmMove)
        offsY-=offsStep*WinX/maxZoom;
      break;
    case kDN :
      if (fracmode==fmParam)
        starti-=paramStep;
      else if (fracmode==fmZoom)
        maxZoom/=zoomStep;
      else if (fracmode==fmMove)
        offsY+=offsStep*WinX/maxZoom;
      break;
    case kRI :
      if (fracmode==fmParam)
        startr+=paramStep;
      else if (fracmode==fmZoom)
        ColorPrecision*=2;
      else if (fracmode==fmMove)
        offsX+=offsStep*WinX/maxZoom;
      break;
    case kLE :
      if (fracmode==fmParam)
        startr-=paramStep;
      else if (fracmode==fmZoom)
        { ColorPrecision/=2; ColorPrecision=ColorPrecision<1?1:ColorPrecision; }
      else if (fracmode==fmMove)
        offsX-=offsStep*WinX/maxZoom;
      break;
    case kTB :
      if (fractype==ftMandel)
        fractype=ftSilverMandel;
      else if (fractype==ftSilverMandel)
        fractype=ftJulia;
      else if (fractype==ftJulia)
        fractype=ftSilverJulia;
      else
        fractype=ftMandel;
      break;
    case kKZ : fracmode = fmZoom; break;
    case kKI : fracmode = fmMove; break;
    case kKU : fracmode = fmParam; break;
    case kSP : Init(); break;
    case kLO : maxIter/=2; break;
    case kHI : maxIter*=2; break;
    case kK1 : paramStep=0.1; break;
    case kK2 : paramStep=0.01; break;
    case kK3 : paramStep=0.001; break;
    default : inherited::OnKeyPressed(aKey);
  }
  Draw();
}

void MainApp::Draw()
{

  switch (fractype)
  {
    case ftMandel :
      for (int i=0; i<WinX; i++)
        for (int j=0; j<WinY; j++)
          fImage.Pixel(i,j, Color(Mandelbrot(
            startr, starti,
            changebase(i-WinX/2, maxZoom, offsX), changebase(j-WinY/2, maxZoom, offsY),
            maxIter
          )));
      break;
    case ftJulia :
      for (int i=0; i<WinX; i++)
        for (int j=0; j<WinY; j++)
          fImage.Pixel(i,j, Color(Mandelbrot(
            changebase(i-WinX/2, maxZoom, offsX), changebase(j-WinY/2, maxZoom, offsY),
            startr, starti,
            maxIter
          )));
      break;
    case ftSilverMandel :
      SilverMandelbrot(drawer, maxZoom, offsX, offsY, startr, starti, maxIter, ColorPrecision, 0, WinX, 0, WinY);
      break;
    case ftSilverJulia :
      SilverJulia(drawer, maxZoom, offsX, offsY, startr, starti, maxIter, ColorPrecision, 0, WinX, 0, WinY);
      break;
  }
  inherited::Draw();
}



