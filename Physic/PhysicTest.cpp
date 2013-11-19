#ifdef BCB
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("PhysicTest.res");
USEUNIT("PhysicDemo.cpp");
USEUNIT("..\3dLib\Common.cpp");
USEUNIT("..\3DLIB\Device.cpp");
USEUNIT("..\3dLib\Image.cpp");
USEUNIT("..\3dLib\Palette.cpp");
USEUNIT("..\3dLib\PlugDrawer.cpp");
USEUNIT("..\3dLib\Viewer.cpp");
USEUNIT("..\3dLib\Wingdll.cpp");
USEUNIT("..\3dLib\Geom.cpp");
USEUNIT("..\3dLib\Camera.cpp");
USEUNIT("..\3dLib\Plot.cpp");
USEUNIT("..\3dLib\Drawer2D.cpp");
USEUNIT("..\3dLib\Particle.cpp");
USEUNIT("..\3dLib\Actions.cpp");
//---------------------------------------------------------------------------
#endif


#ifdef UNDER_CE
#include "Device_CE.h"
#endif
#ifdef WING
#include "Device_WING.h"
#endif


#include "PhysicDemo.h"

#ifdef UNDER_CE
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    cmdLine,
					int       nCmdShow)
#endif
#ifdef WING
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine,
						  int nCmdShow )
#endif
#ifdef LINUX
int main()
#endif
#ifdef SDL
int main()
#endif
{
#ifndef LINUX
#ifndef SDL
  //SetCurrentDirectory("C:\\Mes documents\\Mes Programmes\\Data");
  SetCurrentDirectory("D:\\YOB\\Perso\\3d\\Mes Programmes\\Data");
   Main::hInstance = hInstance;
   Main::hPrevInstance = hPrevInstance;
   Main::nCmdShow = nCmdShow;

   if ( ! Main::hPrevInstance ) {
      MainWindow::Register();
      }
#endif
#endif
  Palette * palette = new Palette();
  palette->Fire();
  Device * dev = NewDevice(WinX, WinY, palette);
  MainApp app(*dev);
  int result=app.Launch();
  delete palette;
  return result;


/*   Palette palette;
   palette.Fire();
   Device * dev = NewDevice(WinX, WinY, &palette);
   MainApp app(*dev);

   return Main::MessageLoop(app);
   */
}
