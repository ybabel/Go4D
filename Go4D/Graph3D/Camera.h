#ifndef _CAMERA_H_
#define _CAMERA_H_

/*
*AUTHOR : Babel yoann
*TITLE  : Camera.h
*CREATED: 10/07/96
*DATE   : 10/07/96
*OBJECT : 3d camera to project APlots in the device coordintate system.
*/

#include "Tools/Geom.h"
#include "Plot.h"

namespace Go4D
{


  /**
  *CAMERA : simple perspective camera. This camera is the more basic but it's
  *  sufficient for most of the drawing in 3D
  */
  class Camera
  {
  protected :
    Rotation fRotation;
    Vector3 fTranslation;
    real fFocal;
    real fZoom;
    real fTet, fPhi, fPsi;
    real fCutPlaneHeight;
    int fScreenWidth, fScreenHeight;
    int fMiddleX, fMiddleY, fMinWidthHeight;

  public :
    ///
    Camera(int ScreenWidth, int ScreenHeight);
    ///
    void SetScreen(int x, int y, int width, int height);
    ///
    void SetScreenCenter(int x, int y);
    ///  project the plot in the database, you must use GetPointZ to get the result of the projection
    APlot & Project(APlot &) const;
    ///  don't project the plot, just change the base in which it's decribed, from the canonical base to the base of the Camera
    APlot ChangeToBase(const APlot &) const;
    ///  does the inverse than ChangeToBase
    APlot ReturnToOldBase(const APlot &) const;
    ///  Change the base direction
    Camera & RotateX(real step);
    ///  Change the base direction
    Camera & RotateY(real step);
    ///  Change the base direction
    Camera & RotateZ(real step);
    ///  Change the base translation
    Camera & TranslateX(real step);
    ///  Change the base translation
    Camera & TranslateY(real step);
    ///  Change the base translation
    Camera & TranslateZ(real step);
    ///  Change the base translation
    Camera & TranslateLookDir(real step);
    ///  Change the base direction
    Camera & SetRotation(real teta, real phi, real psi);
    ///  Change the base translation
    Camera & SetTranslation(real x, real y, real z);
    ///  Change the base focal
    Camera & SetFocal(real focal);
    ///  Change the base zooming factor
    Camera & SetZoom(real zoom);
    ///  return the focal of the base
    real Focal() const;
    ///  return the zooming factor of the base
    real Zoom() const;
    ///  return the size of the screen in which are projected the points
    int ScreenWidth() const;
    ///  return the size of the screen in which are projected the points
    int ScreenHeight() const;
    ///  return the direction of the base
    Rotation GetRotation() const;
    ///  return the position of the base is space
    Vector3 GetTranslation() const;
    ///  return the looking direction of the camera
    Vector3 GetLookDir() const;
    ///  get the plot, which coordinate is (x,y,0) when projected, assuming that it belong to a particular plane
    APlot GetPlot(int x, int y, const Plane &) const;
    ///  change the Height below which face must be cutted
    Camera & SetCutPlaneHeight(real);
    ///  return the Height below which face must be cutted
    real & GetCutPlaneHeight();
    ///  change the Height below which face must be cutted
    real CutPlaneHeight() const;

    /** don't project a plot, but only a size, assuming that the size is at the
    * distance of the plot. For example this is useful if you want to draw a
    * sphere, the size of the sphere in the camera system is given by this method
    * assuming that you give it the center of the sphere as parameter
    */
    real ProjectSize(PointZ p, real size) const;
    ///  determine is a projected point is visisble (Z>CutPlaneHeight)
    bool IsVisible(PointZ p) const;
  };

  /*****************************************************************************\
  * Inline methods and properties
  \*****************************************************************************/

  inline real Camera::Focal() const
  {
    return fFocal;
  }

  inline real Camera::Zoom() const
  {
    return fZoom;
  }

  inline int Camera::ScreenWidth() const
  {
    return fScreenWidth;
  }

  inline int Camera::ScreenHeight() const
  {
    return fScreenHeight;
  }

  inline Rotation Camera::GetRotation() const
  {
    return fRotation;
  }

  inline Vector3 Camera::GetTranslation() const
  {
    return fTranslation;
  }

  inline Camera & Camera::SetCutPlaneHeight(real aCutPlaneHeight )
  {
    fCutPlaneHeight = aCutPlaneHeight;
    return *this;
  }

  inline real & Camera::GetCutPlaneHeight()
  {
    return fCutPlaneHeight;
  }

  inline real Camera::CutPlaneHeight() const
  {
    return fCutPlaneHeight;
  }

}

#endif
