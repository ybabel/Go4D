#ifndef FractalDemoH
#define FractalDemoH

#include "Go4D.h"
using namespace Go4D;

enum eFracType {ftMandel, ftJulia, ftSilverMandel, ftSilverJulia};
enum eFracMode {fmParam, fmMove, fmZoom};

class MainApp : public ViewerApp
{
  typedef ViewerApp inherited;
  protected :
  void OnKeyPressed(int);
  void Draw();
  void Init();

  real startr;
  real starti;
  eFracType fractype;
  eFracMode fracmode;
  real offsX, offsY, maxZoom;
  real paramStep;
  int maxIter;
  int ColorPrecision;
  Drawer2D drawer;

  public :
  MainApp(Device &);
};

//---------------------------------------------------------------------------
#endif
