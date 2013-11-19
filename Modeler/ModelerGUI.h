#ifndef _MODELERGUI_H_
#define _MODELERGUI_H_

#include "Go4D.h"
#include "3DView.h"

using namespace Go4D;

class ModelerGUI : public ViewerApp, public GUI::ActionListener
{
  typedef ViewerApp inherited;
protected :
  void OnDragStart(int mousecode, int x, int y);
  void OnDragOver(int mousecode, int dx, int dy);
  void OnDragEnd(int mousecode, int x, int y);
  void OnMouseClick(int mousecode, int x, int y);
  int Execute(GUI::Action * action, const char * NodeName);
  void Cycle();
  void Draw();
  Font6x10 font;
  GUI::Layout layout;
  GUI::ANode * guiRoot;
  GUI::System gui;
  XML::Parser guiParser;
  DirectoryBrowser dirRoot;
  bool fLoading;

  Editable * GetCurrent() const;
  void RefreshTree() const;
  void RefreshProperties() const;
  void RefreshLinks() const;
  void RefreshActions() const;
  void RefreshTypes() const;
  void RefreshOptions() const;
  void RefreshFileList() const;
  void OpenFile();
  void LoadFile(const char *);
  void SaveFile(const char *);

  GUI::Modeler3DView * GetModeler() const;
  GUI::Tree * GetNodes() const;

public :
  ModelerGUI(Device &);
  ~ModelerGUI();
};

#endif
