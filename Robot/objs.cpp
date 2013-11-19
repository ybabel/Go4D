/*****************************************************************************\
 * Object viewer 
\*****************************************************************************/

#include <fstream.h>
#include "device.h"
#include "object.h"

#define DIMX 300
#define DIMY 300
#define FOCAL 3
#define STEP M_PI/64

int main(void)
{
  Device device;
  ZDrawer drawer(DIMX,DIMY);
  //real teta=-M_PI/3, phi=0, psi=-M_PI/3;
  real teta=M_PI/2, phi=0, psi=0;
  real z=-FOCAL;
  Object object("face.obj");
  Camera camera(teta,phi,psi,0,0,z,2,1);
  byte r;
  byte mode = 1;

  device.AddImage(&drawer);
  device.RedPalette(&drawer);
  //device.DefaultPalette(&drawer);
  while ( (r=device.Read(&drawer))!=kEND)
    {
      switch (r)
	{
	case kUP : teta+=STEP; break;
	case kDN : teta-=STEP; break;
	case kLE : phi += STEP; break;
	case kRI : phi -= STEP; break;
	case kFW : psi += STEP; break;
	case kBW : psi -= STEP; break;
	case kLO : z += 0.1; break;
	case kHI : z -= 0.1; break;
	case kTB : mode = 1-mode; break;
	}
      drawer.Clear();
      camera.NewAngle(teta,phi,psi);
      camera.NewPosition(0,0,z);
      object.Update(camera);
      if (mode) 
	object.Draw(camera, drawer, Vector3(1,1,-1));
      else 
	object.QuickDraw(camera, drawer, Vector3(1,1,-1));
      device.DisplayImage(&drawer);
    }
}

