#ifndef StereoDemoH
#define StereoDemoH

#include "Go4D.h"
using namespace Go4D;
#include "Stereo.h"

class MainApp : public ViewerApp
{
  typedef ViewerApp inherited;
  protected :
  void OnKeyPressed(int);
  void Draw();
  void Init();
  void Cycle();
  ZImage drawer;
  Stereogram stereo;
  bool move;
  int p;
  bool showStereo;


  public :
  MainApp(Device &);
};



//---------------------------------------------------------------------------
#endif
