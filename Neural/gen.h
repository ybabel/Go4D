/*
  AUTHOR     : Babel Yoann
  TITLE      : gen.h
  DATE       : 28/11/95
  OBJECT     : genetic algorithms
  */

#ifndef _GEN_H_
#define _GEN_H_

#include "Image.h"
#include "Common.h"
#include "Drawer2D.h"
#include "Device.h"
#include "Viewer.h"

using namespace Go4D;

#include "nn.h"
#include "car.h"

//scenery options
#define BoxH 30
#define NbBoxes 20

//porsche options
#define VisionX 10
#define VisionY 15
#define FactorTheta 20
#define FactorDepth 5
#define AccMax 10
#define kFrott 0
#define MaxSpeed 30
#define MinSpeed 4
#define StartSpeed 20

//genetic algo+net options
#define TimeLimit 1000
#define MuteRatio real(0.2)
#define MuteFactor real(10)
#define FeedBack 0
#define FeedBackSpeed 0

struct Score
{
  int indice;
  real score;
};

class Population;

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

class Population
{
  public :
  Population(int); //nb individuals
  ~Population();
  void Process(real, real);
  Creature & Best();
  void NewMap();
  friend ostream & operator << (ostream &, const Population & );
  friend istream & operator >> (istream &, Population & );
  real best;
  int time;
  unsigned int generation;

  private :
  void Sort();
  void Reproduction();
  void Mute(real ,real);
    // make the population mute, scores array must be ordered
  void Iterate(); // new scores of the population
  int nbcreatures;
  Creature * creatures;
  Score * scores;
};

#endif
