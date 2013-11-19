#ifdef BCB
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("BlockTest.res");
USEUNIT("BlockDemo.cpp");
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
USEUNIT("..\3dLib\Drawer.cpp");
USEUNIT("..\3dLib\Pixeler.cpp");
USEUNIT("..\3dLib\Tools.cpp");
//---------------------------------------------------------------------------
#endif

#ifdef UNDER_CE
#include "Device_CE.h"
#endif
#ifdef WING
#include "Device_WING.h"
#endif
#ifdef LINUX
#include "DeviceX.h"
#endif


#include "BlockDemo.h"

//---------------------------------------------------------------------------
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
#ifndef UNDER_CE
  //SetCurrentDirectory("C:\\Mes documents\\Mes Programmes\\All Platform Projects\\Data");
  //SetCurrentDirectory("D:\\YOB\\Perso\\3d\\All Platform Projects\\Data");
#endif

#ifndef LINUX
#ifndef SDL
   Main::hInstance = hInstance;
   Main::hPrevInstance = hPrevInstance;
   Main::nCmdShow = nCmdShow;

   if ( ! Main::hPrevInstance ) {
      MainWindow::Register();
      }
#endif
#endif

   Palette palette;
   //palette.Fire();
   palette.Shade16();
   Device * dev = NewDevice(WinX, WinY, &palette);
   MainApp app(*dev);

//   return Main::MessageLoop(app);
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
