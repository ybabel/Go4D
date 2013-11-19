/*
  AUTHOR   : Babel Yoann
  TITLE    : cube.cc
  DATE     : 20/12/95
  OBJECT   : 2 rotating cubes, test of face cutting sorting algo
*/

#include "device.h"
#include "zdrawer.h"

Device scr;
//Stereogram s(600, 400,100,30);
ZDrawer w(600,400);
int p = 250;
//Bmp b("pam.bmp");

void DrawCube(void)
{
  //scr.DisplayImage(&s);
  scr.DisplayImage(&w);
}

PointZ d [4] =
{
  {100,350,3,150},
  {100,150,3,150},
  {200,150,3,100},
  {200,350,3,100}
};

void Init(void)
{
  scr.AddImage(&w);
  w.Clear(0,90);
  w.Box(PointZ(350,p,2,80), 100, 100);
  w.Box(PointZ(200,150,1,100), 200, 200);
  //w.Fill(4,d,3);
  //s.AleaSource();
  //s.ImageSource(b);
  //scr.ImagePalette(&s);
  //s = w;
}

#define step 1
int main (void)
{
  int move=FALSE;
  Init();
  DrawCube();
  unsigned char r;
  while ( (r=scr.Read(&w)) != kEND )
    {
      switch (r)
	{
	//case kUP : cout << (s.focal += step) << kCR; s = w; break;
	//case kDN : cout << (s.focal -= step) << kCR; s = w; break;
	//case kLE : cout << (s.eyedist += step) << kCR; s = w; break;
	//case kRI : cout << (s.eyedist -= step) << kCR; s = w; break;
	case kSP : move = TRUE; break;
        case kLO : move = FALSE; break;
	}
      DrawCube();
      if (move==TRUE) 
	{
	  //cout << "bouge de la" << kCR;
	  p-=5;   
	  w.Clear(0,150);
	  w.Box(PointZ(350,p,2,80), 100, 100);
	  w.Box(PointZ(200,150,1,100), 200, 200);
	  //s.AleaSource();
	  //s = w;
	}
    }
  return 0;
}










