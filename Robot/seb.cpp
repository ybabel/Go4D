/*
*/

#include "device.h"
#include "drawer.h"
#include "sim.h"


#define DIMX 320
#define DIMY 200

int main(void)
{	
	Device device;
	Drawer drawer(DIMX,DIMY);
	device.AddImage(&drawer);

	byte c;
 
	while ((c=device.Read(&drawer))!=kEND)
	{
		switch (c)
		{
		}
		drawer.Clear();
		drawer.ClipPixel(PointZ(10,10,10));
		drawer.Line(PointZ(150,100,10),PointZ(10,10,10));
		device.DisplayImage(&drawer);
	}
}


