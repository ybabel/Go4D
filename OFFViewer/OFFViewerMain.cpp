#ifdef BCB
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("FaceTest.res");
USEUNIT("FaceDemo.cpp");
USEUNIT("..\3dLib\Bump.cpp");
USEUNIT("..\3dLib\Camera.cpp");
USEUNIT("..\3dLib\Clut.cpp");
USEUNIT("..\3dLib\Common.cpp");
USEUNIT("..\3dLib\Cube.cpp");
USEUNIT("..\3DLIB\Device.cpp");
USEUNIT("..\3dLib\Face.cpp");
USEUNIT("..\3dLib\FaceDrawer.cpp");
USEUNIT("..\3dLib\FileIma.cpp");
USEUNIT("..\3dLib\Filter.cpp");
USEUNIT("..\3dLib\Font.cpp");
USEUNIT("..\3dLib\Geom.cpp");
USEUNIT("..\3dLib\Image.cpp");
USEUNIT("..\3dLib\Light.cpp");
USEUNIT("..\3dLib\Object.cpp");
USEUNIT("..\3dLib\Palette.cpp");
USEUNIT("..\3dLib\Plot.cpp");
USEUNIT("..\3dLib\PlugDrawer.cpp");
USEUNIT("..\3dLib\Viewer.cpp");
USEUNIT("..\3dLib\Wingdll.cpp");
USEUNIT("..\3dLib\TemplateFaceDrawer.cpp");
USEUNIT("..\3dLib\Shader.cpp");
USEUNIT("..\3dLib\Plotter.cpp");
USEUNIT("..\3dLib\Mapper.cpp");
USEUNIT("..\3dLib\Scene.cpp");
//---------------------------------------------------------------------------
#endif

#ifdef UNDER_CE
#include "Device_CE.h"
#endif
#ifdef WING
#include "Device_WING.h"
#endif


#include "OFFViewerApp.h"

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

        if ( ! Main::hPrevInstance )
        {
         MainWindow::Register();
        }

        Palette defPal;
        defPal.RedShade();
        Device * dev = NewDevice(WinX,WinY,&defPal);
        char FileName[512];
        if ((cmdLine == NULL) || (cmdLine[0] == '\0')) 
        {
          CopyStr(FileName, "Face.OFF");
        }
        else
        { 
          int i;
          for (i=0; cmdLine[i]!=' '; i++)
            FileName[i] = cmdLine[i];
          FileName[i++] = '\0';
        }
        MainApp app(*dev, FileName);

        Main::MessageLoop(app);
        return 0;
}


