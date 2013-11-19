/*****************************************************************************\
*AUTHOR : Babel yoann
*TITLE  : Fileter.h
*CREATED: 27/07/96
*DATE   : 27/07/96
*OBJECT : image filtering
\*****************************************************************************/

#ifndef _FILTER_H_
#define _FILTER_H_

#include "PlugDrawer.h"
#include "Tools/Geom.h"

namespace Go4D
{

  /**
  * Apply some filter to the image. Used to make some special effects
  * it can be used to make effect like fire or other stuff like that
  * This filter is particularly used to make the "blur motion", when
  * animation go realy fast ;-)
  */
  class FilteredImage : public PluggedDrawer
  {
    typedef PluggedDrawer inherited;
  protected :
    const Image * fSource;
    Matrix3x3 fFilter;
    real fBias;
    real fFactor; // real factor = fFactor/256

  public :
    FilteredImage(const Image &, const Image &);
    FilteredImage(int aWidth, int aHeight, const Image &);
    Matrix3x3 & Filter();
    real & Bias();
    real & Factor();
    FilteredImage & Process();
    FilteredImage & BlurMotion();
    FilteredImage & Emboss();
    const Image * GetSource() const { return fSource; }
  };

  /*****************************************************************************\
  * inline methods and properties
  \*****************************************************************************/

  inline Matrix3x3 & FilteredImage::Filter()
  {
    return fFilter;
  }

  inline real & FilteredImage::Bias()
  {
    return fBias;
  }

  inline real & FilteredImage::Factor()
  {
    return fFactor;
  }

}
#endif
