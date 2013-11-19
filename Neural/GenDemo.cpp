#include "GenDemo.h"

#define PopulationCount 32

MainApp::MainApp(Device & aDevice)
  : inherited(aDevice),
  win(WinX, WinY, true),
  win2(fImage, fImage.Width(), fImage.Height()),
  p(PopulationCount)
{
  win2.Plug(fImage);
  Init();
}

void MainApp::Init()
{
  type=0;
  n=PopulationCount;
  nz = 0;
  muteratio=MuteRatio;
  mutefactor=MuteFactor;
  k=0;
  olds=0;
  finish=0;
  win2.Copy(Creature::scene);
  fDevice->DisplayImage();
}

void MainApp::LoadFromFile(char * aFileName)
{
  ifstream is(aFileName, ios::in);
  is >> p;
}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
    case kEND :
      {
        finish = 1;
        ofstream os("weights", ios::out);
        os << p;
      } break;
    case kSP :
      {
        p.Best().ProcessScore(1, fDevice, &win, &win2);
        ofstream os("weights", ios::out);
        os << p;
      } break;
    case kFW : cout << "MuteRatio : " << (muteratio+=0.01) << kCR; break;
    case kBW : cout << "MuteRatio : " << (muteratio-=0.01) << kCR; break;
    case kRI : cout << "MuteFactor : " << (mutefactor+=1) << kCR; break;
    case kLE : cout << "MuteFactor : " << (mutefactor-=1) << kCR; break;
    case kDN :
      {
        p.NewMap();
        cout << "New Map !!!" << kCR;
        win2.Copy(Creature::scene);
        fDevice->DisplayImage();
      } break;
    default : inherited::OnKeyPressed(aKey);
  }
}

void MainApp::Draw()
{
  p.Process(muteratio, mutefactor);
  cout << "generation : " << real(p.generation)
    << "  current best score : " << real(p.best)
      << "   progression : " << real(p.best-olds) << kCR;
  olds = p.best;
  if (p.time == TimeLimit)
  {
    p.Best().ProcessScore(1, fDevice, &win, &win2);
    p.NewMap();
    cout << "NewMap !!!" << kCR;
  }
  //inherited::Draw();
}

void MainApp::Cycle()
{
  Draw();
}

