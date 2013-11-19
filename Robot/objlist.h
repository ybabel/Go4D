/******************************************************************************\
 *AUTHOR   : Babel Yoann
 *TITLE    : objlist.h, a list of 3D objects for robot simulation
 *DATE     : 5-5-96
\******************************************************************************/

#ifndef _OBJLIST_H_
#define _OBJLIST_H_

#include <string.h>

#include "scene.h"
#include "object.h"
#include "sim.h"

#define FOCAL 3
#define DIMX 300
#define DIMY 300

#define NBOLT 7
#define BOLTPRES 6
#define BOLTCOLOR 8
#define BOLTDIM 0.03
#define BOLTRADIUS 0.06
#define BOLTH -0.02
class Bolt : public Object
{
  public :
  Bolt();
  void SetPositionOnArea(int i, const Area &);
  void Draw(const Camera &, Drawer &, const Vector3 & light) const;
  void SetVisible(int pvisible) {visible=pvisible;}

  private :
  int visible;
};

#define PDIM 0.1
#define PDIM1 0.095
#define MH 0.04
#define ML 0.2
#define BH 0.1
class Player : public Object
{
  public :
  Player(int nplots, int nfaces);
  void SetPositionOnArea(real x, real y, real rot, const Area &);
  virtual void SetAngleAndLen(real angle, real len) = 0;
  virtual void Cinematiq(int i, const Area &, int imax)=0;

  protected :
  void Cube(int i);
  void MapCube(int i, const Image *);
};

class M6Demo;
class IieRobot : public Player
{
  friend M6Demo;
  private :
  const Image * iiemap;
  real destangle;
  real xinit, yinit;

  public :
  IieRobot(const Image *);
  void SetAngleAndLen(real angle, real len);
  void Cinematiq(int i, const Area &, int imax);
  void SetAngle(real newangle);
  void SetInitPos(real xinit, real yinit);
};

class Robot : public Player
{
  public :
  Robot();
  void SetAngleAndLen(real angle, real len);
  void Cinematiq(int i, const Area &, int imax);
};

class SmallRobot : public Player
{
  private :
  PositionList * ppath;

  public :
  SmallRobot(PositionList &);
  void SetAngleAndLen(real angle, real len);
  void Cinematiq(int i, const Area &, int imax);
};

#define NBSTARS 900
class StarField
{
  public :
  StarField(int nbstars, byte color, int size=3, real radius = 1);
  ~StarField();
  void Draw(const Camera &, Drawer & );

  private :
  Plot * stars;
  int nb_stars;
  byte color;
  int size;
};

#define INTRON 100
class M6Demo
{
  public :
  M6Demo(int nbstars, byte color, const Image * iiemap, PositionList &);
  void NewArea(const Camera &, byte clscolor);
  void Draw(const Camera &, ZDrawer &, int draw_second);
  void SetVisibleBolt(int);
  void Intro1(Camera & camera, ZDrawer & drawer,
      	      real teta, real phi, real psi, int i, int imax, byte clscolor);
  void Intro2(Camera & camera, Drawer * drawer,
      	      real teta, real phi, real psi, int i, int imax, byte clscolor);

  IieRobot one;
  Robot two;
  SmallRobot tree;
  Area area;
  Bolt bolts[NBOLT];
  ZDrawer area_image;
  Cube cubes[ZONE];
  StarField star_field;
};

class Messager
{
  public :
  Messager(Fonts &, int time, int maxmessages, byte color=1);
  ~Messager();
  void PutMessage(::string message);
  void Draw(Drawer &);

  private :
  void DecreaseTime();
  ::string * messages;
  int * timer;
  int last;
  int time;
  byte color;
  int max_messages;
  Fonts * font;
};

#endif