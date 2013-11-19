#ifdef BCB
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("FractalTest.res");
USEUNIT("..\3dLib\Common.cpp");
USEUNIT("..\3DLIB\device.cpp");
USEUNIT("..\3dLib\Image.cpp");
USEUNIT("..\3dLib\palette.cpp");
USEUNIT("..\3dLib\PlugDrawer.cpp");
USEUNIT("..\3dLib\Viewer.cpp");
USEUNIT("..\3dLib\Wingdll.cpp");
USEUNIT("..\3dLib\geom.cpp");
USEUNIT("..\3dLib\Camera.cpp");
USEUNIT("..\3dLib\Plot.cpp");
USEUNIT("..\3dLib\Drawer2D.cpp");
USEUNIT("..\3dLib\WriteDrawer.cpp");
USEUNIT("FractalDemo.cpp");
//---------------------------------------------------------------------------
#endif

#ifdef UNDER_CE
#include "Device_CE.h"
#endif
#ifdef WING
#include "Device_WING.h"
#endif

#include "FractalDemo.h"

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
{
#ifndef UNDER_CE
  SetCurrentDirectory("C:\\Mes documents\\Mes Programmes\\All Platform Projects\\Data");
  //SetCurrentDirectory("D:\\YOB\\Perso\\3d\\All Platform Projects\\Data");
#endif

   Main::hInstance = hInstance;
   Main::hPrevInstance = hPrevInstance;
   Main::nCmdShow = nCmdShow;

   if ( ! Main::hPrevInstance ) {
      MainWindow::Register();
      }

   Palette fractalPalette;
   fractalPalette.Shade16();
   Device *dev = NewDevice(WinX, WinY, &fractalPalette);
   MainApp app(*dev);

   return Main::MessageLoop(app);
}

/*
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
*/
//---------------------------------------------------------------------------
