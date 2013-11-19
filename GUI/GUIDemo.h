#ifndef GUIDemoH
#define GUIDemoH

#include "Go4D.h"

using namespace Go4D;

class MainApp : public ViewerApp, public GUI::ActionListener
{
  typedef ViewerApp inherited;
protected :
  Drawer2D drawer;
  Font6x10 font;
  GUI::Layout layout;
  GUI::System gui;
  GUI::Box * guiRoot;
  XML::Parser menu;
  void OnKeyPressed(int);
  void OnMouseClick(int mousecode, int x, int y);
  void Draw();
  void Init();
  void Cycle();
  void OnDragEnd(int mousecode, int x, int y);
  GUI::TreeNode * FillTree(GUI::TreeNode * node, XML::Element * element);

public :
  MainApp(Device &);
  virtual int Execute(GUI::Action * action, const char * NodeName);
};


//---------------------------------------------------------------------------
#endif
