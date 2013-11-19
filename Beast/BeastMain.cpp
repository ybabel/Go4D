#include "BeastMain.h"

#include "RandomBeast.h"

MainApp::MainApp(Device & aDevice)
: inherited(aDevice),
writeDrawer(aDevice.Width(), aDevice.Height()),
// image is not accepted, so it will be plugged later
font("font6x10.fon")
{
  writeDrawer.Plug(fImage);
  fArea = new RandomBeastArea(8);
  fRenderer = new Renderer(&fImage);
  fBeastLand = new BeastLand(fArea);
  fBeastFactory = new RandomBeastFactory();

  fGnokyZone = newq Image(256, 256, true);
  fGnokyZone->ClearAll();
  Drawer<Pixeler> drw(*fGnokyZone);
  drw.Plug(*fGnokyZone);
  //for (int x=0; x<255; x++)
  //  for (int y=0; y<255; y++)
  //    if ( ((x/16)%2+(y/16)%2)%2==0 )
  //      drw.Pixel(x, y, 255);
  drw.FillCircle(PointZ(64, 64, 255, 0), 32);
  drw.FillCircle(PointZ(64, 256-64, 255, 0), 32);
  drw.FillCircle(PointZ(256-64, 64, 255, 0), 32);
  drw.FillCircle(PointZ(256-64, 256-64, 255, 0), 32);
  fZoneField = new HeightField(64, 64, 0.01);
  Converter convert(*fGnokyZone);
  convert.ToHeightField(*fZoneField);
  dynamic_cast<RandomBeastArea*>(fArea)->SetMappedImage(fGnokyZone);

  fGnokyFactory = new ZoneGnokyFactory(fZoneField);

  dynamic_cast<RandomBeastArea*>(fArea)->Init(fBeastLand, fRenderer);
  fBeastLand->AddSink(dynamic_cast<IBeastLandSink *>(fArea));

  fBeastLand->AddRule(dynamic_cast<IRule *>(new DontCollideRule()));
  fBeastLand->AddRule(dynamic_cast<IRule *>(new EatGnokyRule()));
  fBeastLand->AddRule(dynamic_cast<IRule *>(new SearchGnokyRule()));

  //fBeastLand->AddRule(dynamic_cast<IRule *>(new DefendGnokyRule(brBoth)));
  fBeastLand->AddRule(dynamic_cast<IRule *>(new FearOtherRaceRule(brRed)));
  fBeastLand->AddRule(dynamic_cast<IRule *>(new FindFriendWhenFearedRule(brRed)));
  //fBeastLand->AddRule(dynamic_cast<IRule *>(new FindFriendRule(brBlue)));
  fBeastLand->AddRule(dynamic_cast<IRule *>(new FriendsBarycenterRule(brBlue)));
  fBeastLand->AddRule(dynamic_cast<IRule *>(new EnnemiesBarycenterRule(brBlue)));
  fBeastLand->AddRule(dynamic_cast<IRule *>(new FightOtherRaceRule()));

  // Always Last !!
  fBeastLand->AddRule(dynamic_cast<IRule *>(new CloneWhenStrongRule(fBeastFactory)));

  Init();
}

MainApp::~MainApp()
{
  delete fRenderer;
  delete fBeastFactory;
  delete fGnokyFactory;
  delete fBeastLand;
  /// BUG : should be deleted here, but cause a bug, so I assume it's delete elsewhere
  /// but I don't known where !
  //delete fArea;
}

void MainApp::Init()
{
  fTimeCounter=0;
  fDisplay = true;
  fCamera.SetTranslation(0,0,-10);
  fCamera.SetZoom(0.5);
  fCamera.SetFocal(5);
  fCamera.SetRotation(M_PI/2.0+M_PI/2.0,0,0);

  fBeastLand->Clear();
  fRenderer->Clear();
  fRenderer->AddDrawable(dynamic_cast<IDrawable *>(fArea));

  for (int i=0; i<5; i++)
    fBeastLand->CreateGnoky(fGnokyFactory);
  for (int i=0; i<4; i++)
    fBeastLand->CreateBeast(fBeastFactory);

}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
  case kSP :
    fBeastLand->CreateBeast(fBeastFactory);
    break;
  case kTB :
    fBeastLand->CreateGnoky(fGnokyFactory);
    break;
  case kKI :
    Init();
    break;
  case kKQ :
    fDisplay = !fDisplay;
    break;
  default : inherited::OnKeyPressed(aKey);
  }
}

void MainApp::Cycle()
{
    Draw();
}

void MainApp::Draw()
{
  if (fTimeCounter++ > 2)
  {
    fBeastLand ->CreateGnoky(fGnokyFactory);
    fTimeCounter=0;
  }
  if (fDisplay)
  {
    fImage.ClearAll(255);
    fRenderer->Draw(&fCamera);
  }

  fBeastLand->ComputeNextPositions();

  real blue=0.0;
  for (int i=0; i<fArea->GetBeastCount(); i++)
    if (fArea->GetBeast(i)->GetRace()==brBlue)
      blue+=fArea->GetBeast(i)->GetNRJ();
  real red=0.0;
  for (int i=0; i<fArea->GetBeastCount(); i++)
    if (fArea->GetBeast(i)->GetRace()==brRed)
      red+=fArea->GetBeast(i)->GetNRJ();

#define MSGColor 111

  writeDrawer.WriteString(PointZ(0,0,MSGColor,0.0), font, "Blue");
  writeDrawer.WriteInt(PointZ(50,0,MSGColor,0.0), font, Trunc(blue*100.0/(blue+red)), 3);

  writeDrawer.WriteString(PointZ(100,0,MSGColor,0.0), font, "Red");
  writeDrawer.WriteInt(PointZ(150,0,MSGColor,0.0), font, Trunc(red*100.0/(blue+red)), 3);

  inherited::Draw();
}
