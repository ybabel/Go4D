#ifndef GenDemoH
#define GenDemoH

#include "Go4D.h"
using namespace Go4D;

#include "gen.h"


class MainApp : public ViewerApp
{
  typedef ViewerApp inherited;
  protected :
  void OnKeyPressed(int);
  void Draw();
  void Init();
  void Cycle();
  void LoadFromFile(char * aFileName);
  Drawer2D win;
  Drawer2D win2;
  int type;
  int n;
  int nz;
  real muteratio;
  real mutefactor;
  Population p;
  int k;
  real olds;
  int finish;


  public :
  MainApp(Device &);
};

#endif
