#include "PhysicDemo.h"

#define ParticleCount 32

MainApp::MainApp(Device & aDevice)
  : inherited(aDevice),
  drawer(fImage, fImage.Width(), fImage.Height(), true),
  blurer(fImage.Width(), fImage.Height(), true/*autoalloc*/, true/*circlecaps*/),
  fParticles(),
  fActions(),
  fSystem(&fParticles, &fActions, 0.02)
{
  blurer.Plug(fImage);
  drawer.Plug(fImage);
  blur = false;
  fCamera.SetTranslation(0,0,-4);
  fCamera.SetZoom(2);
  fCamera.SetRotation(M_PI/2.0,0,0);

  Init();

  // add actions (force/constraint) on particles
  fActions.Add(new Gravity(&fParticles, -0.5));
  fActions.Add(new Ground(&fParticles, -2.0, 0.9));
  fActions.Add(new Viscosity(&fParticles, 0.5));
  fActions.Add(new FixPosition(&fParticles, 0));

  for (int i=1; i<fParticles.Count(); i++)
  {
    fActions.Add(new Spring(&fParticles, 0,i, 1.0,0.02,0.5));
  }
  fActions.Complete();

}

void MainApp::Init()
{
  fParticles.Clear();
  Particle * curParticle;

  // define positions of particles
  for (int i=0; i<ParticleCount; i++)
  {
    curParticle = new Particle();
    fParticles.Add(curParticle);
    curParticle->fPosition = Vector3(frand*2.0-1.0, frand*2.0-1.0, frand*2.0-1.0);
    curParticle->fSpeed =
      Vector3(curParticle->fPosition[0]*2.0,curParticle->fPosition[1]*2.0,0);
    curParticle->fMass = 1;
  }
  fParticles.Complete();

  drawer.ClearAll();
  blurer.ClearAll();
}

void MainApp::Cycle()
{
  Draw();
}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
    case kTB : blur = !blur; break;
    case kSP : Init(); break;
    default : inherited::OnKeyPressed(aKey);
  }
}

void MainApp::Draw()
{
  int i;
  if (blur)
    blurer.Copy(drawer);
  else
    drawer.ClearAll();

  // draw springs (not optimzed for the moment)
  APlot cur;
  cur = fCamera.ChangeToBase(fParticles[0]->fPosition);
  fCamera.Project(cur);
  PointZ curZ0 = cur.GetPointZ();
  curZ0.color = 111;
  for (i=1; i<fParticles.Count(); i++)
  {
    cur = fCamera.ChangeToBase(fParticles[i]->fPosition);
    fCamera.Project(cur);
    PointZ curZi = cur.GetPointZ();
    curZi.color = 111;

    if ((curZ0.z > 0) && (curZi.z>0))
      if (blur)
        blurer.Line(curZ0, curZi);
      else
        drawer.Line(curZ0, curZi);
  }

  // draw particles
  for (i=0; i<fParticles.Count(); i++)
  {
    APlot cur = fCamera.ChangeToBase(fParticles[i]->fPosition);
    fCamera.Project(cur);
    PointZ curZ = cur.GetPointZ();
    curZ.color = 234;
    if (curZ.z > 0)
    {
      int size = int(real(8.0/curZ.z))+1;
      if (blur)
        blurer.FillCircle(curZ, size);
      else
        drawer.FillCircle(curZ, size);
    }
  }

  if (blur)
    drawer.BlurAttract(blurer, 0, 256-8);

  fSystem.ProcessTimeStep();
  inherited::Draw();
}


