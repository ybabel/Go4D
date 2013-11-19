/*
  AUTHOR : Babel yoann
  TITLE  : map.cc
  CREATED: 15/11/95
  DATE   : 15/11/95
  BUGS   : none
  */

#include <math.h>
#include <ostream.h>
#include  "device.h"
#include "geom.h"
#include "face.h"
#include "common.h"

#define NPlotX 20   //along x axe  
#define NPlotY 20   //in rotation
#define NPlot NPlotX*NPlotY

Device scr;
Drawer w(320,200,0);
PlotSet map(NPlot), tmpmap(NPlot);
SortedObject fs((NPlotX-1)*(NPlotY-1));
Rotation rot;
real Dim = 200;
real phi=0, psy=0, teta=0;
Vector3 translate;
Vector3 light;
real time=0;
Bmp b("pam.bmp");

void Drawmap(real time)
{
  tmpmap = map;

  for (int j=0; j<NPlotY; j++)
    for (int i=0; i<NPlotX; i++)
      {
	real tmp;
	real hx= real(i-NPlotX/2)*2*M_PI/15;
	real hy= real(j-NPlotY/2)*2*M_PI/15;
	tmp = Dim/8*sin(hx/3+time)*sin(hy/3);
	tmpmap[j*NPlotX+i][2] = tmp;
      }

  tmpmap.Rotate(rot);
  tmpmap.Translate(translate);
  w.Clear();
  fs.Draw(w,light);
  scr.DisplayImage(&w);
}

void NewRot(void)
{
  rot.RotateEuler(teta, phi, psy);
  light[0] = cos(M_PI/4); light[1] = 0; light[2] = sin(M_PI/4);
}

void Init(void)
{
  scr.AddImage(&w);
  scr.ImaPalette(&w,b);
  NewRot();
  translate.Set(160,100,0);
  for (int j=0; j<NPlotY; j++)
    for (int i=0; i<NPlotX; i++)
      map[j*NPlotX+i] = Plot((Dim*(i-NPlotX/2))/NPlotX,
			     (Dim*(j-NPlotY/2))/NPlotY,
			     0);
  {
    for (int j=0; j<NPlotY-1; j++)
      {
	for (int i=0; i<NPlotX-1; i++)
	  {
	    fs.AddFace(
	      new MappedFace4(tmpmap, 
			      i+j*NPlotX,
			      i+1+j*NPlotX,
			      i+1+(j+1)*NPlotX, 
			      i+(j+1)*NPlotX, 
			      b,
			      (i*(b.Width()))/NPlotX,
			      (j*(b.Height()))/NPlotY,
			      ((i+1)*(b.Width()))/NPlotX,
			      ((j+1)*(b.Height()))/NPlotY
			      ));
	  }
      }
  }
}

#define step 3.14159/16

void main (void)
{
  Init();
  Drawmap(time);
  unsigned char r;
  while ( (r=scr.Read(&w)) != kEND )
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
      Drawmap(time);
      time += 0.2;
    }
}



