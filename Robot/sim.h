/*****************************************************************************\
 * Small robot simulation, made by LeBab and Sebman
\*****************************************************************************/

#ifndef _SIM_H_
#define _SIM_H_

#include "common.h"
#include "drawer.h"
#include "scene.h"
#include "object.h"

#ifdef PALETTE
#define NCOL 8
#define CLS      255
#define BLACK    249
#define WHITE    240
#define BLUE     160
#define MAG      192
#define PALBLUE  208
#define PALGREEN 128
#define GREEN    80
#define RED      0
#define WRITE    64
#define LINE     208
#define STAR     243
#else
#define NCOL 4
#define CLS      0
#define BLACK    0
#define WHITE    1
#define BLUE     2
#define MAG      5
#define PALBLUE  6
#define PALGREEN 7
#define GREEN    3
#define RED      4
#define WRITE    1
#define LINE     8
#define STAR     1
#endif


#define SDIM 0.06
#define CONVTIME 300

#define TI0 0.5
#define PAST 0.01
#define PASA 0.3
#define ADJT 0.1




#define ZONE 6
#define DEPTH 0.01
#define LEN 1.25
#define RIGOL 0.05
#define WALL -0.05
#define CUBEDIM 0.1

class Area : public Object
{
  public :
  Area();
  void SetCenter(Plot &);
};

struct Position
{
  real x;
  real y;
	int speed ;
  real ang ;
  real ti ;
	int type ;

	Position(real x, real y,real speed,real angle, real inittime);
  friend ostream & operator << (ostream &, const Position &);
  friend istream & operator >> (istream &, const Position &);
};

struct PositionElem
{
  Position position;
  PositionElem * next;
	PositionElem(real x, real y, real speed,real, real);
};

typedef PositionElem * PPositionElem;

class PositionList
{
  public :
  PositionList();
  ~PositionList();
  void AddPosition(real x, real y, real speed,real);
  void DelLast();
  int GetNbPositions() const { return nbelems; }
  Position & operator [] (int); //modify oldj, oldcur
  void Flush();
  int Empty();
  void Trace(Drawer &, const Camera &, const Area &);
  void BeginRead();
  int ReadNext(real & x, real & y, real & angle);
  void Calc(real,real);
  int Next();
  int Previous();
  int CurvLeft();
  int CurvRight();
  int SpeedUp();
  int SlowDown();
  real IncrTime();
  real DecrTime();
  void SetConst(real convtime, real inittime, real deltatime, real deltaangle, real adusttime,
                                                                          real pangle0,real *ptabv,real *ptabtype);
  void Save(); //robot.crb
  void Load(); //robot.crb
  friend ostream & operator << (ostream & ,PositionList &);

  private :
  PPositionElem root, queue;
  int nbelems;
  int oldi;
  int current;
  real oldt;
  real convtime, inittime, deltatime, deltaangle, adjusttime;
  real tabv[32];
  real tabtype[10];
  real angle0;
  void Draw3dLine(Drawer &, const Camera &, const Area &, Plot begin, Plot end, byte color);
  int oldj;
  PPositionElem oldcur;

  private : // simulation calculation methods
  void GetInitPos(int i, real &posx, real &posy, real &ang);
  void SetInitPos(int i, real posx, real posy, real ang);
  void GetInitSpeed(int i, real &v0, real &v1, int &type0, int &type1);
  void ComputeNextPos(real t, real &posx, real &posy, real &ang,
    real v0, real v1, int type0, int type1);
};

#endif
