#ifndef WaterDemoH
#define WaterDemoH

#include "Go4D.h"
using namespace Go4D;

class MainApp : public ViewerApp
{
  typedef ViewerApp inherited;
  protected :
  void OnKeyPressed(int);
  void OnMouseClick(int, int, int);
  void Draw();
  void Init();
  void Cycle();
  Drawer2D drawer;
  int * newWater;
  int * oldWater;
  Pcx sourceImage;
  int touchWidth;
  int touchHeight;

  public :
  MainApp(Device &, Palette &);
};



//---------------------------------------------------------------------------
#endif
