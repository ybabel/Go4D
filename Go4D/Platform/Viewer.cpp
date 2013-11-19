/**
*AUTHOR : Babel yoann
*TITLE  : Viewer.cpp, Viewer.h implementation
*CREATED: 10/07/96
*DATE   : 10/07/96
*BUGS   :
*/

#include "Viewer.h"

namespace Go4D
{


  ViewerApp::ViewerApp(Device & aDevice)
    : inherited(aDevice),
    fImage(aDevice.Width(), aDevice.Height(), false),
    fCamera(aDevice.Width(), aDevice.Height()),
    drawer(aDevice.Width(), aDevice.Height(), false)
  {
    fDevice->AcceptImage(&fImage);
    drawer.Plug(fImage);
  }

  void ViewerApp::OnKeyPressed(int aKey)
  {
    switch (aKey)
    {
    case kUP : fCamera.RotateY(RotationStep); break;
    case kDN : fCamera.RotateY(-RotationStep); break;
    case kLE : fCamera.RotateX(RotationStep); break;
    case kRI : fCamera.RotateX(-RotationStep); break;
    case kLO : fCamera.RotateZ(RotationStep); break;
    case kHI : fCamera.RotateZ(-RotationStep); break;
    case kKA : fCamera.TranslateZ(TranslateStep); break;
    case kKZ : fCamera.TranslateZ(-TranslateStep); break;
    case kFW : fCamera.GetCutPlaneHeight() += TranslateStep; break;
    case kBW : fCamera.GetCutPlaneHeight() -= TranslateStep; break;
    case kEND : fDevice->Close();
    }
  }

  void ViewerApp::OnMouseClick(int mousecode, int x, int y)
  {
  }

  void ViewerApp::OnMouseDblClick(int mousecode, int x, int y)
  {
  }

  void ViewerApp::OnDragStart(int mousecode, int x, int y)
  {
  }

  void ViewerApp::OnDragOver(int mousecode, int dx, int dy)
  {
    int tX=0, tY=0, tZ=0, rX=0, rY=0, rZ=0;
    if ( (mousecode & kMRIGHT) != 0 )
    {
      if ( (mousecode & kShift) != 0 )
        rZ=dy;
      else
        tX=dx; tY=dy;
    }
    else
    {
      if ( (mousecode & kShift) != 0 )
        tZ=dy;
      else
        rX=dy; rY=dx;
    }
    fCamera.RotateX(rX*RotationStep);
    fCamera.RotateY(rY*RotationStep);
    fCamera.RotateZ(rZ*RotationStep);
    fCamera.TranslateX(tX*TranslateStep);
    fCamera.TranslateY(tY*TranslateStep);
    fCamera.TranslateZ(tZ*TranslateStep);
  }

  void ViewerApp::OnDragEnd(int mousecode, int x, int y)
  {
  }

  void ViewerApp::Draw()
  {
    fDevice->DisplayImage();
  }

  void ViewerApp::Cycle()
  {
    //Draw();
  }

  PointZ ViewerApp::GetInCameraSystem(real x, real y, real z, Color c) const
  {
    APlot aPlot(x, y, z);
    aPlot.GetColor()=c;
    APlot cur = fCamera.ChangeToBase(aPlot);
    fCamera.Project(cur);
    return cur.GetPointZ();
  }

  PointZ ViewerApp::GetInCameraSystem(const APlot & plot) const
  {
    APlot cur = fCamera.ChangeToBase(plot);
    fCamera.Project(cur);
    return cur.GetPointZ();
  }

  PointZ ViewerApp::DrawPlot(real x, real y, real z, Color c)
  {
    PointZ curZ = GetInCameraSystem(x, y, z, c);
    if (curZ.z > 0)
      drawer.FillCircle(curZ, 2);
    return curZ;
  }

  void ViewerApp::DrawLine(PointZ &alpha, PointZ &omega)
  {
    if ( (alpha.z > 0) && (omega.z > 0) )
      drawer.Line(alpha, omega);
  }

  void ViewerApp::DrawBase()
  {
    PointZ Origin=GetInCameraSystem(0,0,0, 64);
    PointZ XAxis=GetInCameraSystem(1,0,0, 64);
    PointZ YAxis=GetInCameraSystem(0,1,0, 64);
    PointZ ZAxis=GetInCameraSystem(0,0,1, 64+16); // Change color to indicate Z axis
    DrawLine(Origin, XAxis);
    DrawLine(Origin, YAxis);
    DrawLine(ZAxis, Origin); // ZAxis if first, because it determine the color of the line
  }

}
