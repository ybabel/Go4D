#include "StereoDemo.h"

MainApp::MainApp(Device & aDevice)
  : inherited(aDevice),
  drawer(fImage.Width(), fImage.Height()),
  stereo(fImage, 100, 30)
{
  move = false;
  showStereo = true;
  Init();
}

void MainApp::Init()
{
  p = 250;
  drawer.ClearAll(0);
  drawer.ClearAllZ(fconv(150));
  drawer.Box(PointZ(300,p,55,80), 100, 100);
  drawer.Box(PointZ(150,150,123,100), 200, 200);
  stereo.AleaSource();
  if (showStereo)
    stereo = drawer;
  else
    stereo.CopyImage(drawer);
}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
    case kHI :
      showStereo = true;
      Init();
      break;
    case kLO :
      showStereo = false;
      Init();
      break;
    case kTB :
      move = !move;
      break;
    case kSP :
      Init();
      break;
    default : inherited::OnKeyPressed(aKey);
  }
}

void MainApp::Draw()
{
  if (move)
  {
    p-=5;
    drawer.ClearAll(0);
    drawer.ClearAllZ(fconv(150));
    drawer.Box(PointZ(300,p,55,80), 100, 100);
    drawer.Box(PointZ(150,150,123,100), 200, 200);
    stereo.AleaSource();
    if (showStereo)
      stereo = drawer;
    else
      stereo.CopyImage(drawer);
  }
  inherited::Draw();
}

void MainApp::Cycle()
{  
  Draw();
}