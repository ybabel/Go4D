/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : special.h
 *CREATED: 29/06/96
 *DATE   : 29/06/96
 *OBJECT : here are regrouped some special graphic classes linke stereogram,
 *         starfields
\*****************************************************************************/

#ifndef _SPECIAL_H_
#define _SPECIAL_H_

#include <string.h>
//#include "zdrawer.h"
#include "Camera.h"

namespace Go4D
{
/*****************************************************************************\
 *STEREOGRAM : give it a zbufferized image, and it will build the
 *  correspondant stereogram
\*****************************************************************************/

class Stereogram : public Image
{
  typedef Image inherited;
  public :
  Stereogram(int width, int height, int peyedist, real pfocal);
  ~Stereogram();
  Stereogram & operator = (const ZDrawer & );
  void AleaSource();
  void ImageSource(const Image &);
  int eyedist;
  real focal;

  private :
  void Pixel(int x, int y, byte color);
  byte * source;
};

/*****************************************************************************\
 *STARFIELD : can be amelioarated : fix for each point a way of moving.
\*****************************************************************************/

class StarField
{
  public :
  StarField(int nbstars, byte color, int size=3, real radius = 1);
  ~StarField();
  void Draw(const Camera &, Drawer & );

  private :
  APlot * stars;
  int nb_stars;
  byte color;
  int size;
};

/*****************************************************************************\
 *MESSAGER
\*****************************************************************************/

class Messager
{
  public :
  Messager(Fonts &, int time, int maxmessages, byte color=1);
  ~Messager();
  void PutMessage(string message);
  void Draw(Drawer &);

  private :
  void DecreaseTime();
  string * messages;
  int * timer;
  int last;
  int time;
  byte color;
  int max_messages;
  Fonts * font;
};

/*****************************************************************************\
 *Inline methods, and properties
\*****************************************************************************/

inline void Stereogram::Pixel(int x, int y, byte color)
{
  assert((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight));
  *(fBuffer+x+fWidth*y) = color;
}

}
#endif
