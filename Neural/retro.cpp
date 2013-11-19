#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include "Device.h"
#include "Image.h"
#include "car.h"

#define real float

#define WinX 320
#define WinY 200
#define StartSpeed 25
#define VisionX 50
#define VisionY 1
#define FactorVision 4
#define NbBoxes 15
#define BoxH 20
#define MinSpeed 4

Device* dev;
Drawer win(WinX, WinY, 0);
Drawer win2(WinX, WinY, 0);

class Creature
{ // a neural net that conduce the porsche
  public :
  Creature();
  void Manual(real acc, real vth);
  int success;

  static Scenery scene;
  private :

  static Porsche porsche;
};

Creature::Creature()
{
  success = 0;
  porsche.Init(StartSpeed);
};

void Creature::Manual(real acc, real vth)
{
  porsche.SetAccel(acc);
  porsche.SetRotation(vth);
  porsche.Iterate(win);
  win2.Copy(win);
  porsche.Vision(win2);
  dev->DisplayWin(&win2);
}

Scenery Creature::scene(WinX, WinY);
Porsche Creature::porsche(Creature::scene, VisionX, VisionY, FactorVision, StartSpeed, MinSpeed);

Creature creature;

void Init()
{
  Palette * palette = new Palette();
  palette->Shade16();
  dev = NewDevice(WinX, WinY, palette);
  dev->AddWin(&win2);
  dev->DefaultPalette(&win2);
  Creature::scene.Generate(NbBoxes, BoxH);
  win.Copy(Creature::scene);
}

#define AccStep 1
#define VthStep 0.1

int main(int argc, char ** argv)
{
  Init();
  real acc=0;
  real vth=0;
  byte c;
  while ( (c=dev->Read(&win2))!=kEND)
    {
      creature.Manual(acc, vth);
      acc = 0;
      vth = 0;
      switch (c)
	{
	  /*
	  case kUP : if (acc<0) acc = AccStep; else acc += AccStep; break;
	  case kDN : if (acc>0) acc = -AccStep; else acc -= AccStep; break;
	  case kLE : if (vth<0) vth = VthStep; else vth += VthStep; break;
	  case kRI : if (vth>0) vth = -VthStep; else vth -= VthStep; break;
	  case kSP :  break;
	  */
	case kUP : acc = 10; break;
	case kDN : acc = -10; break;
	case kLE : vth = -1; break;
	case kRI : vth = 1; break;
	}
    }
}


