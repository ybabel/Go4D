/*
  AUTHOR   : Babel Yoann
  TITLE    : cube.cc
  DATE     : 20/12/95
  OBJECT   : 2 rotating cubes, test of face cutting sorting algo
*/

#include <math.h>
#include <ostream.h>
#include "device.h"
#include "common.h"
#include "geom.h"
#include "face.h"
#include "scene.h"


Device scr;
Drawer w(320,200,0,1); //zbuffer
PlotSet cube1(8), cube2(8), tmpcube1(8), tmpcube2(8);
FaceSet fs(12);
Rotation rot1, rot2, rx, ry, rz;
Plot t, translate;
real Dim = 100;
real phi1=0, psy1=0, teta1=0;
real phi2=0, psy2=0, teta2=0;
Vector3 light;
real focal = 100;
real zoom = 5*Dim/100;

int faceinit[6][4] =
{
  {0,1,3,2},
  {4,6,7,5},
  {0,4,5,1},
  {2,3,7,6},
  {0,2,6,4},
  {1,5,7,3}
};
 
void DrawCube(void)
{
  Vector3 z; z[0]=0; z[1]=0; z[2]=1;
  tmpcube1 = cube1;
  tmpcube1.Rotate(rot1);
  tmpcube1.Translate(translate);
  tmpcube2 = cube2;
  tmpcube2.Rotate(rot2);
  tmpcube2.Translate(t+translate);
  w.Clear();
  fs.Draw(w,light, focal, zoom);
  scr.DisplayImage(&w);
}

void NewRot(void)
{
  rot1.RotateEuler(teta1, phi1, psy1);
  rot2.RotateEuler(teta2, phi2, psy2);
  light[0] = 0; light[1] = 0; light[2] = 1;
}

void Init(void)
{
  scr.AddImage(&w);
  scr.DefaultPalette(&w);
  int l=0;
  for (int i1=0; i1<2; i1++)
    for (int j=0; j<2; j++)
      for (int k=0; k<2; k++,l++)
	{
	  cube1[l] = Plot(i1*Dim, j*Dim, k*Dim);
	  cube2[l] = Plot(i1*Dim, j*Dim, k*Dim);
	}
  t[0] = 0;
  t[1] = 0;
  t[2] = 0;
  translate.Set(0,0,5*Dim);
  NewRot();
  for (int i=0; i<6; i++)
    {
      fs.AddFace(new Face4(tmpcube1, faceinit[i][0], faceinit[i][1], 
			   faceinit[i][2], faceinit[i][3], 0, 15, FALSE));
      fs.AddFace(new Face4(tmpcube2, faceinit[i][0], faceinit[i][1], 
			   faceinit[i][2], faceinit[i][3], 64, 15, FALSE));
    }
}

#define step 3.14159/16

int main (void)
{
  Init();
  teta1 += step*2;
  teta2 = M_PI/3;
  psy2 = -M_PI/16;
  teta1 = M_PI/3;
  psy1 = -M_PI/16;
  NewRot();
  DrawCube();
  unsigned char r;
  while ( (r=scr.Read(&w)) != kEND )
    {
      switch (r)
	{
	case kUP : teta1 += step; NewRot(); break;
	case kDN : teta1 -= step; NewRot(); break;
	case kLE : phi1  += step; NewRot(); break;
	case kRI : phi1  -= step; NewRot(); break;
	case kFW : psy1  += step; NewRot(); break;
	case kBW : psy1  -= step; NewRot(); break;
	case kLO : t[0]  += 8; break;
	case kHI : t[0]  -= 8; break;
	}
      DrawCube();
    }
  return 0;
}



