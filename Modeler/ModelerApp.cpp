#ifdef BCB
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
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
USEFORM("frmMain.cpp", frm3DEditor);
USEUNIT("..\Editor\Editor.cpp");
USEUNIT("..\3dLib\Drawer.cpp");
USEUNIT("..\3dLib\Tools.cpp");
USEUNIT("..\3dLib\Pixeler.cpp");
USEUNIT("..\3dLib\WriteDrawer.cpp");
USEUNIT("..\3dLib\Font.cpp");
USEUNIT("..\3dLib\Particle.cpp");
USEUNIT("..\3dLib\Actions.cpp");
USEUNIT("..\Editor\Editable3D.cpp");
USEUNIT("EditablePhysic.cpp");
USEUNIT("ModelerDemo.cpp");
USEUNIT("..\3dLib\Face.cpp");
USEUNIT("..\3dLib\Light.cpp");
USEUNIT("..\3dLib\facedrawer.cpp");
USEUNIT("..\3dLib\TemplateFaceDrawer.cpp");
USEUNIT("..\3dLib\Mapper.cpp");
USEUNIT("..\3dLib\Plotter.cpp");
USEUNIT("..\3dLib\Shader.cpp");
//---------------------------------------------------------------------------
#endif

#ifdef UNDER_CE
#include "Device_CE.h"
#endif
#ifdef WING
#include "Device_WING.h"
#endif


#include "ModelerGUI.h"

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

#ifdef BCB
        try
        {
                 Forms::Application->Initialize();
                 Forms::Application->CreateForm(__classid(Tfrm3DEditor), &frm3DEditor);
                 Forms::Application->Run();
        }
        catch (Exception &exception)
        {
                 Forms::Application->ShowException(&exception);
        }
        return 0;
#else
  Palette * palette = new Palette();
  palette->Shade16();
  Device * dev = NewDevice(WinX, WinY, palette);
  ModelerGUI app(*dev);
  int result=app.Launch();
  delete palette;
  return result;
#endif
}
//---------------------------------------------------------------------------
