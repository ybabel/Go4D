#ifndef IntelligentCarH
#define IntelligentCarH

/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : IntelligentCar.h
 *CREATED: 24/04/99
 *DATE   : 24/04/99
 *OBJECT : IntelligentCar class
\*****************************************************************************/

#include "Common.h"
#include "Drawer2D.h"
using namespace Go4D;

#include "nn.h"
#include "gen.h"
#include "car.h"

class Creature
{
  friend Population;
  public :
  Creature();
  void ProcessScore(int, Device *, Drawer2D *, Drawer2D *);
    //displaying store the score in score
  void Copy(const Creature &);
  void Mute(real, real); // percentage, factor
  void Reproduction(const Creature &, real); // percentage
  friend ostream & operator << (ostream &, const Creature &);
  friend istream & operator >> (istream &, Creature & );

  real score;
  int time;
  int alive;

  static Drawer2D vision;
  static Scenery scene;

  private :
  int muted;
  Layer entries, outputs, hidden1, hidden2, hidden3;
  Net net;

  static Porsche porsche;
};



//---------------------------------------------------------------------------
#endif
