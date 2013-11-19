  /*\
 <*#*>  seb.cc 19960504 sim client by seb powered by duschmoul
  \*/

#include "device.h"
#include "drawer.h"
#include "sim.h"

#define DIMX 320
#define DIMY 200

int main(void)
{	
	PositionList p;

	Device device;
	Drawer drawer(DIMX,DIMY);
	device.AddImage(&drawer);

	p.AddPosition(0.0, 0.0, 10.0,M_PI*0.16);
	
	byte c;
		
	while ((c=device.Read(&drawer))!=kEND)
	{
		drawer.Clear();
		switch(c)
		{
		case kKA : p[p.courant].ti+=PAST; break ;
		case kKQ : p[p.courant].ti-=PAST; break ;
		case kLE : p[p.courant].type-- ; break ;
		case kRI : p[p.courant].type++ ; break ;

		case kSP : p.AddPosition(0.0,0.0,10.0,0.0); p.Next(); 
				break ;
		case kKI : if (p.GetNbPositions()>=2)
				{ p.DelLast(); p.Previous() ; }
				break ;
		case kDN : p.Previous(); break ;
		case kUP : p.Next(); break ;
		}
 		p.Trace(drawer);
		device.DisplayImage(&drawer);
	}
	cout << p ;
}

