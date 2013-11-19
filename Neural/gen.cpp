/*
   AUTHOR     : Babel Yoann
   TITLE      : gen.cc
   DATE       : 28/11/95
   ACTUAL     : one algo is implemented, creatures intellignece (net wight)
                can be save on files
   BUG        :
   */

#include "gen.h"

// static members initialization
Scenery Creature::scene(WinX, WinY);
Porsche Creature::porsche(Creature::scene, VisionX, VisionY, FactorTheta,
			  FactorDepth, StartSpeed, MinSpeed, MaxSpeed, kFrott);


/*
   ------------------------------------------------------------
   Creature
   ------------------------------------------------------------
   */

#define NNeur VisionX*VisionY

Creature::Creature()
:entries(NNeur+2*FeedBack+FeedBackSpeed),
 //hidden1(NNeur/2), hidden2(NNeur/4), hidden3(NNeur/8),
 outputs(2),
 net(entries, outputs)
{
  //hidden1.Connect(entries);
  //hidden2.Connect(hidden1);
  //hidden3.Connect(hidden2);
  //outputs.Connect(hidden3);
  outputs.Connect(entries);
  //hidden1.InitRandom(0.2);
  //hidden2.InitRandom(0.2);
  //hidden3.InitRandom(0.2);
  outputs.InitRandom(0.2);
  //net.AddLayer(hidden1);
  //net.AddLayer(hidden2);
  //net.AddLayer(hidden3);
  muted = 1;
  alive = 1;
}

void Creature::Copy(const Creature & creature)
{
  net.Copy(creature.net);
  score = creature.score;
  time = creature.time;
  alive = creature.alive;
  muted = creature.muted;
}

void Creature::Mute(real percent, real factor)
{
  muted = 1;
  net.Mute(percent, factor);
  time = 0;
  alive = 1;
  score = 0;
}

void Creature::ProcessScore(int disp, Device * device, Drawer2D * win, Drawer2D * win2)
{
  if (!disp) if (!muted) return;
  //no need to process the score if it has already been done
  time=0;
  alive = 1;
  porsche.Init(StartSpeed);
  if (disp)
    win->Copy(scene);
  while ( (time != TimeLimit) && alive)
    {
      time++;
      porsche.Iterate(*win);
      alive = !porsche.collision;
      if (disp) win2->Copy(*win);
      porsche.Vision(*win2);
      if (disp) device->DisplayImage();

      for (int i=0; i<NNeur; i++)
	  entries[i].output = porsche.vision[i];
      for (int i1=NNeur; i1<NNeur+FeedBack; i1++)
	{
	  entries[i1].output = outputs[0].output;
	  entries[FeedBack+i1].output = outputs[1].output;
	}
      for (int i2=0; i2<FeedBackSpeed; i2++)
	entries[NNeur+FeedBack+i2].output = 0;
      if (FeedBackSpeed!=0)
	{
	  int ns = int(porsche.v*FeedBackSpeed/MaxSpeed);
	  ns = ns<0?0: ns>=FeedBackSpeed?FeedBackSpeed-1:ns;
	  entries[NNeur+FeedBack+ns].output = 1;
	}
      net.Process();
      porsche.SetAccel( outputs[0].output*AccMax );
      porsche.SetRotation( outputs[1].output );
    }
  score = porsche.distance;
  if (disp)
    cout << "score : " << score << "   time : " << time << kCR;
  muted = 0;
}

void Creature::Reproduction(const Creature & creature, real percent)
{
  net.Reproduction(creature.net, percent);
}

ostream & operator << (ostream &os, const Creature &creature)
{
  os << creature.net;
  return os;
}

istream & operator >> (istream &is, Creature & creature)
{
  is >> creature.net;
  return is;
}

/*
   ------------------------------------------------------------
   Population
   ------------------------------------------------------------
   */

Population::Population(int nb)
{
  nbcreatures = nb;
  creatures = new Creature [nbcreatures];
  scores = new Score [nbcreatures];
  generation = 0;
  NewMap();
  Iterate();
  Sort();
}

Population::~Population()
{
  delete creatures;
  delete scores;
}

void Population::Sort()
{  // creature 0 is the best
  for (int i1=0; i1<nbcreatures; i1++)
    { scores[i1].indice=i1; scores[i1].score=creatures[i1].score; }
  for (int i=nbcreatures-1; i>0; i--)
    for (int j=0; j<i; j++)
      if (scores[j].score<scores[j+1].score)
	{
	  Score tmp = scores[j+1];
	  scores[j+1] = scores[j];
	  scores[j] = tmp;
	}
  best = creatures[scores[0].indice].score;
  time = creatures[scores[0].indice].time;
}

void Population::Reproduction()
{
  // not done yet, test mute instead
}

void Population::Mute(real muteratio, real mutefactor)
{
#define MUTE(j,i) \
  creatures[scores[j].indice].Copy(creatures[scores[i].indice]); \
  creatures[scores[j].indice].Mute(muteratio, mutefactor)

  int nbfirst = nbcreatures/10, nbmid=nbcreatures/2;
  int i;
  for (i=0; i<nbfirst; i++)
    { // the best are reproduced 2 times
      MUTE(nbcreatures-2*i-1,i);
      MUTE(nbcreatures-2*i-2,i);
    }
  for (i=nbfirst; i<nbmid-nbfirst; i++)
    {
      MUTE(nbmid+i-nbfirst,i);
    }
}

void Population::Process(real muteratio, real mutefactor)
{
  Mute(muteratio, mutefactor);
  Iterate();
  Sort();
  generation++;
}

void Population::Iterate()
{
  for (int i=0; i<nbcreatures; i++)
      creatures[i].ProcessScore(0, NULL, NULL, NULL);
}

Creature & Population::Best()
{
  return creatures[scores[0].indice];
}

ostream & operator << (ostream &os, const Population &population)
{
  os.write( (char *)&population.generation, sizeof(int) );
  os.write( (char *)&population.nbcreatures, sizeof(int) );
  for (int i=0; i<population.nbcreatures; i++)
    os << population.creatures[i];
  return os;
}

istream & operator >> (istream &is, Population &population)
{
  int n;
  is.read( (char *)&n, sizeof(int) );
  population.generation = n;
  is.read( (char *)&n, sizeof(int) );
  int nb = min(n,population.nbcreatures);
  for (int i=0; i<nb; i++)
    is >> population.creatures[i];
  return is;
}

void Population::NewMap()
{
  Creature::scene.Generate(NbBoxes, BoxH);
  for (int i=0; i<nbcreatures; i++)
    creatures[i].muted = 1; // to force them to recalculate them self
  Iterate();
  Sort();
}

