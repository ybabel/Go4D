/*****************************************************************************\
* Sample application that draw cube
\*****************************************************************************/

#ifndef _OFFVIEWERAPP_H_
#define _OFFVIEWERAPP_H_

#include "Go4D.h"

using namespace Go4D;

#define NCLUT 8
#define STEP (1/8.0)

enum CubeType { ctFlat, ctGouraud, ctLinMapped, ctQuadMapped, ctHypMapped,
ctLinShaded, ctLinBump, ctTransluent, ctLAST };

class MainApp : public ViewerApp
{
  typedef ViewerApp inherited;
private :
  Image * fPhongImage;
  Object * fFace;
  SunLight * fLight;
  SortedObject fScene;
  Font6x10 fFont;
  WriteDrawer<Pixeler> fWriter;
  void OnKeyPressed(int aKey);

public :
  MainApp(Device &, const char * OOFFile);
  void Draw();
};

#endif
