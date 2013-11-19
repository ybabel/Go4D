#ifndef BeastMainH
#define BeastMainH

#include "Viewer.h"
#include "BeastLand.h"
#include "Drawable.h"
#include "Convert.h"
#include "WriteDrawer.h"

using namespace Go4D;

class MainApp : public ViewerApp
{
  typedef ViewerApp inherited;
protected:
  void OnKeyPressed(int);
  void Draw();
  void Init();

  Image * fGnokyZone;
  HeightField * fZoneField;

  IBeastArea * fArea;
  Renderer * fRenderer;
  BeastLand * fBeastLand;
  IBeastFactory * fBeastFactory;
  IGnokyFactory * fGnokyFactory;
  int fTimeCounter;
  bool fDisplay;

  Font6x10 font;
  WriteDrawer<Pixeler> writeDrawer;

public:
  MainApp(Device &);
  void Cycle();
  ~MainApp();

};


#endif
