#ifdef BCB
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("WBeasts.res");
USEUNIT("BeastMain.cpp");
USEUNIT("..\Math\Geom.cpp");
USEUNIT("..\Platform\Viewer.cpp");
USEUNIT("..\Platform\Win32\Wingdll.cpp");
USEUNIT("..\Platform\Win32\Device.cpp");
USEUNIT("..\Tools\Variant.cpp");
USEUNIT("..\Tools\Common.cpp");
USEUNIT("..\Graph2D\WriteDrawer.cpp");
USEUNIT("..\Graph2D\Font.cpp");
USEUNIT("..\Graph2D\Image.cpp");
USEUNIT("..\Graph2D\Palette.cpp");
USEUNIT("..\Graph2D\Pixeler.cpp");
USEUNIT("..\Graph2D\PlugDrawer.cpp");
USEUNIT("..\Graph2D\Drawer.cpp");
USEUNIT("..\Graph3D\TemplateFaceDrawer.cpp");
USEUNIT("..\Graph3D\Face.cpp");
USEUNIT("..\Graph3D\Light.cpp");
USEUNIT("..\Graph3D\Mapper.cpp");
USEUNIT("..\Graph3D\Plot.cpp");
USEUNIT("..\Graph3D\Plotter.cpp");
USEUNIT("..\Graph3D\Shader.cpp");
USEUNIT("..\Graph3D\Camera.cpp");
USEUNIT("..\Graph2D\Clut.cpp");
USEUNIT("..\Beast\Drawable.cpp");
USEUNIT("..\Beast\RandomBeast.cpp");
USEUNIT("..\Beast\BeastLand.cpp");
USEUNIT("..\Tools\List.cpp");
USEUNIT("..\Tools\FString.cpp");
USEUNIT("..\Tools\Fixed.cpp");
USEUNIT("..\Math\Random.cpp");
USEUNIT("..\Graph2D\Convert.cpp");
USEUNIT("..\Graph3D\FaceDrawer.cpp");
USEUNIT("..\Beast\DNA.cpp");
//---------------------------------------------------------------------------
#endif

#ifdef UNDER_CE
#include "Device_CE.h"
#endif
#ifdef WING
#include "Device_WING.h"
#endif

#include "BeastMain.h"

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
  palette->Shade16();
  Device * dev = NewDevice(WinX, WinY, palette);
  MainApp app(*dev);
  int result=app.Launch();
  delete palette;
  return result;


/* YBA2013
   Palette palette;
   palette.Shade16();
   Device * dev = NewDevice(WinX, WinY, &palette);
   MainApp app(*dev);

   return Main::MessageLoop(app);*/
}
//---------------------------------------------------------------------------
