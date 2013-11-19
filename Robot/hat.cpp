/*****************************************************************************\
 *AUTHOR : Babel yoann Antras
 *TITLE  : hat.cc
 *CREATED: 14/11/95
 *DATE   : 15/11/95
 *OBJECT : shaded hat (cardinal sinus), rotating in 3D
 *BUGS   : 
\*****************************************************************************/

#include <math.h>
#include <ostream.h>
#include "device.h"
#include "zdrawer.h"
#include "sdrawer.h"
#include "face.h"

#define NPlotX 5
#define NPlotR 5
#define NPlot NPlotX*NPlotR

Device scr;

PlotSet hat(NPlot), tmphat(NPlot);

//ZDrawer w(320,200);
//FaceSet fs((NPlotX-1)*NPlotR);

//Drawer w(320,200);
//SortedObject fs ((NPlotX-1)*NPlotR);

SDrawer w(320,200);
FaceSet fs((NPlotX-1)*NPlotR);


Rotation rot;
Vector3 translate;
real Dim = 10;
real phi=M_PI/4, psy=0, teta=M_PI/4;
Vector3 light;
real focal = 100;
real zoom = 10/focal; 

void Drawhat(void)
{
  tmphat = hat;
  tmphat.Rotate(rot);
  tmphat.Translate(translate);
  w.Clear();
  fs.Draw(w,light, focal, zoom);
  w.Flush();  //only for s-buffer
  scr.DisplayImage(&w);    
}

void NewRot(void)
{
  rot.RotateEuler(teta, phi ,psy);
  light[0] = cos(M_PI/4); light[1] = 0; light[2] = sin(M_PI/4);
  light = rot*light;
}

void Init(void)
{
  translate.Set(0,0,10*focal);
  scr.AddImage(&w);
  scr.RedPalette(&w);
  NewRot();
  for (int j1=0; j1<NPlotR; j1++)
    {
      real angle=j1*2*M_PI/NPlotR;
      for (int i=0; i<NPlotX; i++)
	{
	  Plot tmp;
	  tmp[0] = Dim*i*cos(angle);
	  tmp[1] = Dim*i*sin(angle);
	  real h= i*2*M_PI/NPlotX;
	  tmp[2] = i==0?Dim*10:Dim*10*sin(h)/h;
	  hat[j1*NPlotX+i] = tmp;
	}
    }
  for (int j=0; j<NPlotR; j++)
    {
      for (int i=0; i<NPlotX-1; i++)
	{
	  fs.AddFace(
		     new Face4(tmphat, 
				     i+1+j*NPlotX,
				     i+1+(j+1)%NPlotR*NPlotX, 
				     i+(j+1)%NPlotR*NPlotX, 
				     i+j*NPlotX,
				     1,14
				     ));
	}
    }
}

#define step 3.14159/16

int main (void)
{
  Init();
  Drawhat();
  unsigned char r;
  int tps = 0;
  while ( (r=scr.Read(&w)) != kEND && (tps < 256) )
    {
      switch (r)
	{
	case kUP : teta += step; NewRot(); break;
	case kDN : teta -= step; NewRot(); break;
	case kLE : phi  += step; NewRot(); break;
	case kRI : phi  -= step; NewRot(); break;
	case kFW : psy  += step; NewRot(); break;
	case kBW : psy  -= step; NewRot(); break;
	}
      Drawhat();
      phi += 2*M_PI/128; NewRot();
      tps ++;
    }
}



