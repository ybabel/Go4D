#ifndef _CONVERT_H_
#define _CONVERT_H_

#include "PlugDrawer.h"
#include "Tools/Random.h"

namespace Go4D
{

  /**
  * The converted is a class that can convert some data from a format to an other.
  * For example, it can convert HeightField into image and vice-versa
  */
  class Converter : public PluggedDrawer
  {
    typedef PluggedDrawer inherited;
  public:
    ///
    Converter(const Image &);
    ///
    Converter(int width, int height);
    ///
    void Copy(const Image & );
    ///
    void FromHeightField(const HeightField &, real coef=1.0, real base=0.0);
    ///
    void ToHeightField(HeightField &, real amin=0.0, real amax=1.0);
    ///
    void Scale(const Image & );
  };

  /// a drawer that can blur an image, used to make some special effects
  class Blurer : public PluggedDrawer
  {
    typedef PluggedDrawer inherited;
  public:
    ///
    Blurer(const Image &);
    ///
    Blurer(int width, int height);
    ///
    void Blur(const Image & );
  };

}

#endif
