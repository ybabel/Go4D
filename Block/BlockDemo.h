#ifndef BlockDemoH
#define BlockDemoH

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
  Drawer<ZPixeler> drawer;
  ParticleArray fParticles;
  ActionArray fActions;
  ParticleSystem fSystem;
  ParticleArray fParticlesRigid;
  ActionArray fActionsRigid;
  ParticleSystem fSystemRigid;

  public :
  MainApp(Device &);
};

//---------------------------------------------------------------------------
#endif
