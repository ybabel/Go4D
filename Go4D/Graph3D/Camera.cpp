/*****************************************************************************\
*AUTHOR : Babel yoann
*TITLE  : Camera.cpp, Camera.h implementation
*CREATED: 10/07/96
*DATE   : 10/07/96
*BUGS   :
\*****************************************************************************/

#include "Camera.h"

namespace Go4D
{


  /*****************************************************************************\
  *  Camera
  \*****************************************************************************/

  Camera::Camera(int aScreenWidth, int aScreenHeight)
  {
    assert(aScreenWidth > 0);
    assert(aScreenHeight > 0);
    fScreenWidth = aScreenWidth;
    fScreenHeight = aScreenHeight;
    fMiddleX = fScreenWidth/2;
    fMiddleY = fScreenHeight/2;
    fMinWidthHeight = min(fScreenWidth, fScreenHeight);
    fFocal = 1;
    fZoom = 1;
    fTet = 0;
    fPhi = 0;
    fPsi = 0;
    fRotation.RotateEuler(fTet,fPhi,fPsi);
    fCutPlaneHeight = 0;
  }

  void Camera::SetScreen(int x, int y, int width, int height)
  {
    fScreenWidth = width;
    fScreenHeight = height;
    fMiddleX = x+fScreenWidth/2;
    fMiddleY = y+fScreenHeight/2;
    fMinWidthHeight = min(fScreenWidth, fScreenHeight);
  }

  void Camera::SetScreenCenter(int x, int y)
  {
    fMiddleX = x;
    fMiddleY = y;
  }

  APlot & Camera::Project(APlot & aPlot) const
  {
    aPlot.ProjectionPerspective(fFocal, fMiddleX, fMiddleY,
      fZoom, fMinWidthHeight);
    return aPlot;
  }

  APlot Camera::ChangeToBase(const APlot & plot) const
  {
    APlot result = plot;
    result = fRotation*plot;
    result -= fTranslation;
    return result;
  }

  APlot Camera::ReturnToOldBase(const APlot & plot) const
  {
    APlot result = plot;
    result += fTranslation;
    result = (!fRotation)*result;
    return result;
  }

  Camera & Camera::RotateX(real aStep)
  {
    fTet += aStep;
    fRotation.RotateEuler(fTet,fPhi,fPsi);
    return *this;
  }

  Camera & Camera::RotateY(real aStep)
  {
    fPhi += aStep;
    fRotation.RotateEuler(fTet,fPhi,fPsi);
    return *this;
  }

  Camera & Camera::RotateZ(real aStep)
  {
    fPsi += aStep;
    fRotation.RotateEuler(fTet,fPhi,fPsi);
    return *this;
  }

  Camera & Camera::TranslateX(real aStep)
  {
    fTranslation[0] += aStep;
    return *this;
  }

  Camera & Camera::TranslateY(real aStep)
  {
    fTranslation[1] += aStep;
    return *this;
  }

  Camera & Camera::TranslateZ(real aStep)
  {
    fTranslation[2] += aStep;
    return *this;
  }

  Camera & Camera::TranslateLookDir(real aStep)
  {
    fTranslation = Vector3(fTranslation+GetLookDir()*aStep);
    return *this;
  }

  Camera & Camera::SetRotation(real aTet, real aPhi, real aPsi)
  {
    fTet = aTet;
    fPhi = aPhi;
    fPsi = aPsi;
    fRotation.RotateEuler(fTet,fPhi,fPsi);
    return *this;
  }

  Camera & Camera::SetTranslation(real aX, real aY, real aZ)
  {
    fTranslation.Set(aX, aY, aZ);
    return *this;
  }

  Camera & Camera::SetFocal(real aFocal)
  {
    fFocal = aFocal;
    return *this;
  }

  Camera & Camera::SetZoom(real aZoom)
  {
    fZoom = aZoom;
    return *this;
  }

  APlot Camera::GetPlot(int x, int y, const Plane & plane) const
  {
    real tx = real(fScreenWidth)/2;
    real ty = real(fScreenHeight)/2;
    real f = real( min(tx,ty))*fZoom;
    real sx = (x-tx)/f;
    real sy = (y-ty)/f;
    Vector3 origin(0,0,-fFocal);
    origin = origin+fTranslation;
    origin = (!fRotation)*origin;
    Vector3 pointed(sx, sy, 0);
    pointed = pointed+fTranslation;
    pointed = (!fRotation)*pointed;
    Vector3 director = pointed-origin;
    Vector3 normal = plane.Normal();

    real denom = director*normal;
    if (abs(denom) < zero) return APlot();
    real t = (plane.GetDist() - real(origin*normal))/denom;

    Vector3 result = origin + director*t;
    return result;
  }

  Vector3 Camera::GetLookDir() const
  {
    return Vector3(fRotation(0,2), fRotation(1,2), fRotation(2,2));
  }

  real Camera::ProjectSize(PointZ p, real size) const
  {
    return fMinWidthHeight/2*fZoom*fFocal/(p.z+fFocal)*size;
  }

  bool Camera::IsVisible(PointZ p) const
  {
    return p.z > fCutPlaneHeight;
  }

}
