#ifndef _VIEWER_H_
#define _VIEWER_H_

/**
*AUTHOR : Babel yoann
*TITLE  : Viewer.h
*CREATED: 10/07/96
*DATE   : 19/05/99
*OBJECT : A simple application class with a drawer and a camera in order to
*         draw an view 3D stuff. By default the camera is moved by the mouse.
*         (you can drag with Left and Right button + shit to control depth)
*/

#include "Device.h"
#include "Graph3D/Camera.h"
#include "Graph2D/Drawer.h"
#include "Graph3D/Pixeler.h"

namespace Go4D
{


#define RotationStep 0.01
#define TranslateStep 0.05
#ifdef UNCER_CE
#define WinX 240
#define WinY 320
#else
#define WinX 512
#define WinY 512
#endif

  /**
  * A simple application used to visualise 3D stuffs. It has a predfined camera
  * controled by the mouse movements
  */
  class ViewerApp : public Application
  {
    typedef Application inherited;
  protected :
    ZImage fImage;
    Camera fCamera;
    Drawer<ZPixeler> drawer;
    PointZ GetInCameraSystem(real x, real y, real z, Color c) const;
    PointZ GetInCameraSystem(const APlot & plot) const;
    PointZ DrawPlot(real x, real y, real z, Color c);
    void DrawLine(PointZ &, PointZ &);
    void DrawBase();

  public :
    ///
    ViewerApp(Device &);
    ///
    void Draw();
    ///
    virtual void Cycle();
    ///
    virtual void OnKeyPressed(int keycode);
    ///
    virtual void OnMouseClick(int mousecode, int x, int y);
    ///
    virtual void OnMouseDblClick(int mousecode, int x, int y);
    ///
    virtual void OnDragStart(int mousecode, int x, int y);
    ///
    virtual void OnDragOver(int mousecode, int dx, int dy);
    ///
    virtual void OnDragEnd(int mousecode, int x, int y);
  };

}

#endif
