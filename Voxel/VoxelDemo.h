#ifndef VoxelDemoH
#define VoxelDemoH

#include "Go4D.h"
#include "Voxel.h"

class MainApp : public ViewerApp
{
  typedef ViewerApp inherited;
protected :
  void OnKeyPressed(int aKey);
  void Draw();

  Drawer2D voxeldrawer;
  VoxelMap m;
  int following;
  real x,y, d;
  real z;
  real t;
  int r;
  int stop;
  int  margedepth;
  real penteadd;

public :
  MainApp(Device &);
};

#endif
