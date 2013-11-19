#include "IntelligentCar.h"

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


