#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include "Device.h"
#include "Image.h"
#include "gen.h"

Device dev;
Drawer win(WinX, WinY, 0);
Drawer win2(WinX, WinY, 0);


int main(int argc, char ** argv)
{
  int type=0;
  int n;
  int nz = 0;
  real muteratio=MuteRatio, mutefactor=MuteFactor;
  if (argc==1) n=10; //default value
  else n = atoi(argv[1]);
  Population p(n);
  if (argc>2)
    {
      ifstream is(argv[2], ios::in);
      is >> p;
    }
  dev.AddWin(&win2);
  dev.DefaultPalette(&win2);
  win2.Copy(Creature::scene);
  dev.DisplayWin(&win2);
  int k=0;
  real olds=0;
  int finish=0;
  while (!finish)
    {
      p.Process(muteratio, mutefactor);
      cout << "generation : " << p.generation
	<< "  current best score : " << p.best
	  << "   progression : " << p.best-olds << kCR;
      switch (dev.Read(&win2))
	{
	case kEND : finish = 1;
	case kSP :
	  {
	    p.Best(&dev, &win, &win2);
	    ofstream os("weights", ios::out);
	    os << p;
	  } break;
	case kFW : cout << "MuteRatio : " << (muteratio+=0.01) << kCR; break;
	case kBW : cout << "MuteRatio : " << (muteratio-=0.01) << kCR; break;
	case kRI : cout << "MuteFactor : " << (mutefactor+=1) << kCR; break;
	case kLE : cout << "MuteFactor : " << (mutefactor-=1) << kCR; break;
	case kDN :
	  {
	    p.NewMap();;
	    cout << "New Map !!!" << kCR;
	  } break;
	}
      olds = p.best;
    }
}
