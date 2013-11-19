#include "WaterDemo.h"

MainApp::MainApp(Device & aDevice, Palette & palette)
  : inherited(aDevice),
  drawer(fImage.Width(), fImage.Height(), true),
  sourceImage("test1.pcx")
{
  newWater = new int[WinX*WinY];
  oldWater = new int[WinX*WinY];
  sourceImage.ImportPalette(palette);
  Init();
}

void MainApp::Init()
{
  drawer.Map4(sourceImage,
    0,0,
    0,sourceImage.Height()-1,
    sourceImage.Width()-1,sourceImage.Height()-1,
    sourceImage.Width()-1,0,
    0,0,
    0,WinY-1,
    WinX-1,WinY-1,
    WinX-1,0);
  for (int y=0; y<WinY; y++)
    for (int x=0; x<WinX; x++)
    {
      newWater[x+y*WinX] = 0;
      oldWater[x+y*WinX] = 0;
    }
  touchWidth = 2;
  touchHeight=5000;
}

void MainApp::OnMouseClick(int, int i, int j)
{
  for (int y=0-touchWidth; y<1+touchWidth; y++)
    for (int x=0-touchWidth; x<1+touchWidth; x++)
    {
      real norme= sqrt(sqr(x)+sqr(x));
      if (int(norme)<touchWidth)
        oldWater[x+i+(y+j)*WinX] = touchHeight;
    }
}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
    case kUP :
      touchWidth+=2;
      break;
    case kDN :
      touchWidth-=2;
      break;
    case kRI :
      touchHeight+=1000;
      break;
    case kLE :
      touchHeight-=1000;
      break;
    case kTB :
      break;
    default : inherited::OnKeyPressed(aKey);
  }
}


void MainApp::Draw()
{

  for (int y=1; y<WinY-1; y++)
    for (int x=1; x<WinX-1; x++)
    {
      int ofs = x+y*WinX;
      int NewWater = ((
        oldWater[ofs+WinX]+
        oldWater[ofs-WinX]+
        oldWater[ofs+1]+
        oldWater[ofs-1]+
        oldWater[ofs+WinX+1]+
        oldWater[ofs+WinX-1]+
        oldWater[ofs-WinX+1]+
        oldWater[ofs-WinX-1]));
      /*int temp = ((NewWater>>1)+(NewWater>>2)-newWater[ofs]);
      newWater[ofs] = (temp+(temp>>1)+(temp>>3))>>3;*/ // make a very nice effect
      int temp = ((NewWater>>1)+(NewWater>>2)-newWater[ofs]); // determine water viscosity = 3*OldWater8Voisins/4-OldOldWater = 6*old1 - old2 ~5
      newWater[ofs] = (temp+(temp>>1)+(temp>>4))>>3; // determine perturbance radius (x+x/2+x/16)/8 = (25/16) /8 = (7.8/5) /8 = 0.97
    }

  /*
  for (int y=2; y<WinY-2; y++)
    for (int x=2; x<WinX-2; x++)
    {
      int ofs = x+y*WinX;
      int newh = (
         (
          (
           (oldWater[ofs + WinX]
          + oldWater[ofs - WinX]
          + oldWater[ofs + 1]
          + oldWater[ofs - 1]
           )<<1)
          + ((oldWater[ofs - WinX - 1]
          + oldWater[ofs - WinX + 1]
          + oldWater[ofs + WinX - 1]
          + oldWater[ofs + WinX + 1]))
          + ( (
              oldWater[ofs - (WinX*2)]
            + oldWater[ofs + (WinX*2)]
            + oldWater[ofs - 2]
            + oldWater[ofs + 2]
            ) >> 1 )
          + ( (
              oldWater[ofs - (WinX*2) - 1]
            + oldWater[ofs - (WinX*2) + 1]
            + oldWater[ofs + (WinX*2) - 1]
            + oldWater[ofs + (WinX*2) + 1]
            + oldWater[ofs - 2 - WinX]
            + oldWater[ofs - 2 + WinX]
            + oldWater[ofs + 2 - WinX]
            + oldWater[ofs + 2 + WinX]
            ) >> 2 )
         )
        >> 3)
        - (newWater[ofs]);
      newWater[ofs] =  (newh - (newh >> 4))>>1;
    }*/


  for (int y=1; y<WinY-1; y++)
    for (int x=1; x<WinX-1; x++)
    {
      int dx = newWater[x+y*WinX]-newWater[x+(y+1)*WinX];
      int dy = newWater[x+y*WinX]-newWater[x+1+y*WinX];
      int ofsx = abs((dx >> 2)+x)%WinX;
      int ofsy = abs((dy >> 2)+y)%WinY;
      byte color = drawer.GetPixel(ofsx, ofsy);
      fImage.Pixel(x, y, color);
    }

  int* tmp = newWater;
  newWater = oldWater;
  oldWater = tmp;

  inherited::Draw();
}

void MainApp::Cycle()
{
  Draw();
}
