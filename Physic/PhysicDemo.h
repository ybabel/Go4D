#ifndef PhysicDemoH
#define PhysicDemoH

#include "Go4D.h"

using namespace Go4D;

class MainApp : public ViewerApp
{
  typedef ViewerApp inherited;

  protected :
  void OnKeyPressed(int);
  void Draw();
  void Init();
  void Cycle();
  Drawer2D drawer;
  Drawer2D blurer;
  ParticleArray fParticles;
  ActionArray fActions;
  ParticleSystem fSystem;
  bool blur;

  public :
  MainApp(Device &);
};


//---------------------------------------------------------------------------
#endif
