#ifdef BCB
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("RobotTest.res");
USEUNIT("Common.cpp");
USEUNIT("device.cpp");
USEUNIT("drawer.cpp");
USEUNIT("face.cpp");
USEUNIT("geom.cpp");
USEUNIT("image.cpp");
USEUNIT("object.cpp");
USEUNIT("Objlist.cpp");
USEUNIT("res.cpp");
USEUNIT("robot.cpp");
USEUNIT("scene.cpp");
USEUNIT("Sim.cpp");
USEUNIT("Wingdll.cpp");
USEUNIT("zdrawer.cpp");
//---------------------------------------------------------------------------
#endif

#ifdef UNDER_CE
#include "Device_CE.h"
#endif
#ifdef WING
#include "Device_WING.h"
#endif

#include "robot.h"

//---------------------------------------------------------------------------
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
   Main::hInstance = hInstance;
   Main::hPrevInstance = hPrevInstance;
   Main::nCmdShow = nCmdShow;

   if ( ! Main::hPrevInstance ) {
      MainWindow::Register();
      }
   Device dev;
   M6RobotApp app(dev);

   return Main::MessageLoop(app);
}

//---------------------------------------------------------------------------
