#include "ParticleDemo.h"

#define ParticleCount 32

MainApp::MainApp(Device & aDevice)
  : inherited(aDevice),
  drawer(fImage, fImage.Width(), fImage.Height(), true/*circlecaps*/),
  blurer(fImage.Width(), fImage.Height(), true/*autoalloc*/,true/*circlecaps*/),
  fParticles(),
  fActions(),
  fSystem(&fParticles, &fActions, 0.02)
{
  drawer.Plug(fImage);
  blur = false;
  fCamera.SetTranslation(0,0,-5);
  fCamera.SetZoom(2);
  fCamera.SetRotation(M_PI/2.0,0,0);

  Init();

  fActions.Add(new Gravity(&fParticles, -2.0));
  fActions.Add(new Ground(&fParticles, -1.5, 0.5));
  fActions.Add(new Viscosity(&fParticles, 0.5));
  fActions.Complete();
}

void MainApp::Init()
{
  fParticles.Clear();
  for (int i=0; i<ParticleCount; i++)
  {
    Particle * curParticle = new Particle();
    fParticles.Add(curParticle);
    curParticle->fPosition = Vector3(frand*2.0-1.0, frand*2.0-1.0, frand*2.0-1.0);
    curParticle->fSpeed =
      Vector3(curParticle->fPosition[0]/2.0,curParticle->fPosition[1]/2.0,0);
    curParticle->fMass = 1;
  }
  fParticles.Complete();
  drawer.ClearAll();
  blurer.ClearAll();
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
  if (blur)
    blurer.Copy(drawer);
  else
    drawer.ClearAll();
  Color color = 255;
  for (int i=0; i<fParticles.Count(); i++)
  {
    Particle * curParticle = fParticles[i];
    APlot cur = fCamera.ChangeToBase(curParticle->fPosition);
    fCamera.Project(cur);
    PointZ curZ = cur.GetPointZ();
    curZ.color = color;
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

void MainApp::Cycle()
{
  Draw();
}