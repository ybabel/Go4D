#ifndef StereoH
#define StereoH

#include "PlugDrawer.h"

using namespace Go4D;

/*****************************************************************************\
 *STEREOGRAM : give it a zbufferized image, and it will build the
 *  correspondant stereogram
\*****************************************************************************/

class Stereogram : public PluggedDrawer
{
  typedef PluggedDrawer inherited;
  public :
  Stereogram(const Image&, int peyedist, real pfocal);
  Stereogram(int width, int height, int peyedist, real pfocal);
  ~Stereogram();
  Stereogram & operator = (const ZImage & );
  void AleaSource();
  void ImageSource(const Image &);
  void CopyImage(const ZImage & );
  int eyedist;
  real focal;

  private :
  void Pixel(int x, int y, Color color);
  Color * source;
};

inline void Stereogram::Pixel(int x, int y, Color color)
{
  assert((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight));
  *(fBuffer+x+fWidth*y) = color;
}

//---------------------------------------------------------------------------
#endif
