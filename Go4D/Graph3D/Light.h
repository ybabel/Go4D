#ifndef _LIGHT_H_
#define _LIGHT_H_

/*
*AUTHOR : Babel yoann
*TITLE  : Light.h : Images enlightning
*CREATED: 14/07/96
*DATE   : 14/07/96
*OBJECT : Defines the differents kind of light.
*/

#include "Plot.h"

namespace Go4D
{


  /**
  * ALIGHT
  *   Abstract light.
  */
  class ALight
  {
  protected :
    int fIntensity;

  public :
    /// 
    ALight(int Intensity=255);
    /// 
    virtual APlot& EnLight(APlot &, const Vector3 & Normal) const =0;
    /// 
    virtual Vector3 Direction(const APlot &) const =0;
  };

  /// simulate a light like the sun, with parallel ray of lights
  class SunLight : public ALight
  {
    typedef ALight inherited;
  private :
    Vector3 fDirection;
    real fTet, fPhi, fPsi;

  public :
    /// 
    SunLight(const Vector3 & Direction, int Intensity=255);
    /// 
    virtual ~SunLight() {}
    /// 
    virtual APlot & EnLight(APlot &, const Vector3 & Normal) const;
    /// 
    virtual Vector3 Direction(const APlot &) const;
    SunLight & SetDirection(real teta, real phi, real psi); 
    SunLight & RotateX(real step);
    SunLight & RotateY(real step);
    SunLight & RotateZ(real step);
  };


  /*****************************************************************************\
  * inline metyhods and properties
  \*****************************************************************************/

  inline Vector3 SunLight::Direction(const APlot &) const
  {
    return fDirection;
  }

}

#endif
