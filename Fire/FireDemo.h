#ifndef _FireDemo_H_
#define _FireDemo_H_

#include "Go4D.h"

typedef int FilterMatrix[8];

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

  int particleSize;
  int margin;
  int burn;
  FilterMatrix * filter;
  bool sun;
  real explosionangle;
  int explosionsize;
  real explosiondir;

  Color Blur1(Color * old, int ofs);
  Color Blur2(Color * old, int ofs);
  Color Blur3(Color * old, int ofs);
  Color Blur4(Color * old, int ofs);

  void Alea1();
  void Alea2();
  void Alea3();
  void Alea4();

  public :
  MainApp(Device &);
};



#endif
