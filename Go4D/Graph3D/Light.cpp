/*****************************************************************************\
*AUTHOR : Babel yoann
*TITLE  : Light.cpp, Light.h implementation
*CREATED: 14/07/96
*DATE   : 14/07/96
*BUGS   :
\*****************************************************************************/

#include "Light.h"

namespace Go4D
{


  ALight::ALight(int aIntensity)
  {
    fIntensity = aIntensity;
  }

  SunLight::SunLight(const Vector3 & aDirection, int aIntensity)
    : inherited(aIntensity)
  {
    fDirection = aDirection;
  }

  APlot & SunLight::EnLight(APlot & aPlot, const Vector3 & aNormal) const
  {
    real enlightning = aNormal*fDirection;
    enlightning = fabs(enlightning);
    return aPlot.AddColor(int(enlightning*fIntensity));
  }

  SunLight & SunLight::SetDirection(real teta, real phi, real psi)
  {
    fTet = teta;
    fPhi = phi;
    fPsi = psi;
    Rotation r;
    r.RotateEuler(fTet, fPhi, fPsi);
    fDirection = r*Vector3(0,0,1);
    return *this;
  }

  SunLight & SunLight::RotateX(real step)
  {
    fTet += step;
    Rotation r;
    r.RotateEuler(fTet, fPhi, fPsi);
    fDirection = r*Vector3(0,0,1);
    return *this;
  }

  SunLight & SunLight::RotateY(real step)
  {
    fPhi += step;
    Rotation r;
    r.RotateEuler(fTet, fPhi, fPsi);
    fDirection = r*Vector3(0,0,1);
    return *this;
  }

  SunLight & SunLight::RotateZ(real step)
  {
    fPsi += step;
    Rotation r;
    r.RotateEuler(fTet, fPhi, fPsi);
    fDirection = r*Vector3(0,0,1);
    return *this;
  }


}
