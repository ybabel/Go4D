#ifdef BCB
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("WaterTest.res");
USEUNIT("WaterDemo.cpp");
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
USEUNIT("..\3dLib\FileIma.cpp");
USEUNIT("..\3dLib\filter.cpp");
//---------------------------------------------------------------------------
#endif

#include "WaterDemo.h"

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
   SetCurrentDirectory("C:\\Mes documents\\Mes Programmes\\All Platform Projects\\Data");
   //SetCurrentDirectory("D:\\YOB\\Perso\\3d\\All Platform Projects\\Data");
   Main::hInstance = hInstance;
   Main::hPrevInstance = hPrevInstance;
   Main::nCmdShow = nCmdShow;

   if ( ! Main::hPrevInstance ) {
      MainWindow::Register();
      }
#endif
#endif

   Palette palette;
   palette.Shade16();
   Device * dev = NewDevice(WinX, WinY, &palette);
   MainApp app(*dev, palette);

   return app.Launch();
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
