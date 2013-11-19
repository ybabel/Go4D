#include "BeastMain.h"

int main(int argc, char **argv) {

  Palette palette;
  palette.Shade16();
  Device dev(WinX, WinY, &palette);
  MainApp app(dev);

  return dev.MainLoop();
}
