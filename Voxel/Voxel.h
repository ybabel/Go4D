#ifndef _VOXEL_H_
#define _VOXEL_H_

#include "Drawer2D.h"
#include "Viewer.h"

using namespace Go4D;

#define MapX 256
#define MapY 256
#define MaxZ 64
#define k1 6
#define k2 19
#define k3 16
#define NbColors 255
extern int prof;
#define focal 0.7
#define echx 256
#define echy 128
#define water  40
#define marge 1/sqrt(2)

class VoxelMap
{
public :
  VoxelMap(int pMaxColor);
  ~VoxelMap();
  void PutColor(Drawer2D & w);
  void PutMapZ(Drawer2D & w);
  void precalc(real, real, real, Drawer2D &);
  void VOXEL(real, real, Drawer2D & w);
  void BlurZ();
  void BlurZ0();
  void BlurZ1();
  void BlurZ2();
  void BlurC();
  void SetColor();
  void Satur();
  realtype * fMapZ;
  int  margedepth;
  real penteadd;

private :
  Color * fMapC;
  int fMaxColor;
  void SetZ(int xb, int yb, int xe, int ye);
  void SetZ1(int xb, int yb, int xe, int ye);
  int Noise(int pValue1, int pValue2, int pHeight);
  int * oldy, * horiz;
  Color * oldc;
  real scrx, oz, scrz, decal;
  real teta;
  real * tmy, * tstepy, * tbegu, *tbegv, * tstepu, * tstepv;
  real c,s; //cos teta, sin teta
};

#define MapZ(x,y) fMapZ[(int(x)&(MapX-1))+MapX*(int(y)&(MapY-1))]
#define MapC(x,y) fMapC[(int(x)&(MapX-1))+MapX*(int(y)&(MapY-1))]
#define CurMapZ(x,y) CurMapZ[(int(x)&(MapX-1))+MapX*(int(y)&(MapY-1))]
#define CurMapC(x,y) CurMapC[(int(x)&(MapX-1))+MapX*(int(y)&(MapY-1))]


#endif