#include "BlockDemo.h"

#define Diag1 (sqrt(2.0))
#define Diag2 (sqrt(1.0+sqrt(2.0)))

#define PMin 0
#define PMax 2
#define Len 2
#define Spr 500.0
#define Dmp 5.0
#define DT 0.01

MainApp::MainApp(Device & aDevice)
  : inherited(aDevice),
  drawer(fImage),
  fSystem(&fParticles, &fActions, DT),
  fSystemRigid(&fParticlesRigid, &fActionsRigid, DT)
{
  drawer.Plug(fImage);
  fCamera.SetTranslation(0,0,-4);
  fCamera.SetZoom(1);
  fCamera.SetRotation(M_PI/2.0,0,0);
  drawer.ClearAll();

  for (int i=0; i<8; i++)
    fParticles.Add(new Particle);
  fParticles.Complete();
  for (int i=0; i<8; i++)
    fParticlesRigid.Add(new Particle);
  fParticlesRigid.Complete();

  // add actions (force/constraint) on particles
  fActions.Add(new Gravity(&fParticles, -5.5));
  fActions.Add(new Ground(&fParticles, -2.0, 0.2, 0.2));
  fActions.Add(new Viscosity(&fParticles, 0.1));
  //fSystem.AddAction(new FixPosition(&fParticles, 7));
  fActionsRigid.Add(new Gravity(&fParticlesRigid, -5.5));
  fActionsRigid.Add(new Ground(&fParticlesRigid, -2.0, 0.9, 0.2));
  fActionsRigid.Add(new Viscosity(&fParticlesRigid, 0.1));
  //fSystem.AddAction(new FixPosition(&fParticles, 7));

  /*
  fActions.Add(new Spring(&fParticles, 0,1, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 1,3, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 3,2, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 2,0, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 4,5, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 5,7, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 7,6, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 6,4, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 0,4, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 1,5, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 3,7, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 2,6, Spr,Dmp,Len));
  fActions.Add(new Spring(&fParticles, 0,3, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 1,2, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 1,7, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 3,5, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 5,6, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 7,4, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 2,4, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 0,6, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 0,5, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 1,4, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 2,7, Spr,Dmp,Len*Diag1));
  fActions.Add(new Spring(&fParticles, 3,6, Spr,Dmp,Len*Diag1));
  */
  fActions.Add(new Bar(&fParticles, 0,1, DT));
  fActions.Add(new Bar(&fParticles, 1,3, DT));
  fActions.Add(new Bar(&fParticles, 3,2, DT));
  fActions.Add(new Bar(&fParticles, 2,0, DT));
  fActions.Add(new Bar(&fParticles, 4,5, DT));
  fActions.Add(new Bar(&fParticles, 5,7, DT));
  fActions.Add(new Bar(&fParticles, 7,6, DT));
  fActions.Add(new Bar(&fParticles, 6,4, DT));
  fActions.Add(new Bar(&fParticles, 0,4, DT));
  fActions.Add(new Bar(&fParticles, 1,5, DT));
  fActions.Add(new Bar(&fParticles, 3,7, DT));
  fActions.Add(new Bar(&fParticles, 2,6, DT));
  fActions.Add(new Bar(&fParticles, 0,3, DT));
  fActions.Add(new Bar(&fParticles, 1,2, DT));
  fActions.Add(new Bar(&fParticles, 1,7, DT));
  fActions.Add(new Bar(&fParticles, 3,5, DT));
  fActions.Add(new Bar(&fParticles, 5,6, DT));
  fActions.Add(new Bar(&fParticles, 7,4, DT));
  fActions.Add(new Bar(&fParticles, 2,4, DT));
  fActions.Add(new Bar(&fParticles, 0,6, DT));
  fActions.Add(new Bar(&fParticles, 0,5, DT));
  fActions.Add(new Bar(&fParticles, 1,4, DT));
  fActions.Add(new Bar(&fParticles, 2,7, DT));
  fActions.Add(new Bar(&fParticles, 3,6, DT));

  fActions.Complete();

  // Damping is empirically setted to the highest value it's here to handle precision problems
  fActionsRigid.Add(new Rigid(&fParticlesRigid, 0, 7, 0.999));
  fActionsRigid.Complete();

  Init();
}

void MainApp::Init()
{
  fParticles[0]->fPosition = Vector3(PMin, PMin, PMin);
  fParticles[1]->fPosition = Vector3(PMin, PMax, PMin);
  fParticles[2]->fPosition = Vector3(PMax, PMin, PMin);
  fParticles[3]->fPosition = Vector3(PMax, PMax, PMin);
  fParticles[4]->fPosition = Vector3(PMin, PMin, PMax);
  fParticles[5]->fPosition = Vector3(PMin, PMax, PMax);
  fParticles[6]->fPosition = Vector3(PMax, PMin, PMax);
  fParticles[7]->fPosition = Vector3(PMax, PMax, PMax);
  for (int i=0; i<fParticles.Count(); i++)
  {
    fParticles[i]->fMass = 1.0;
    fParticles[i]->fPosition.GetColor() = random(255);
    //fParticles[i]->fSpeed = Vector3(fParticles[i]->fPosition - Vector3(1,1,1)) ^ Vector3(0,0,1);
  }
  fParticles[0]->fSpeed = Vector3(1.5, -0.0, 0.0)*2.0;

  fParticlesRigid[0]->fPosition = Vector3(PMin, PMin, PMin);
  fParticlesRigid[1]->fPosition = Vector3(PMin, PMax, PMin);
  fParticlesRigid[2]->fPosition = Vector3(PMax, PMin, PMin);
  fParticlesRigid[3]->fPosition = Vector3(PMax, PMax, PMin);
  fParticlesRigid[4]->fPosition = Vector3(PMin, PMin, PMax);
  fParticlesRigid[5]->fPosition = Vector3(PMin, PMax, PMax);
  fParticlesRigid[6]->fPosition = Vector3(PMax, PMin, PMax);
  fParticlesRigid[7]->fPosition = Vector3(PMax, PMax, PMax);
  for (int i=0; i<fParticlesRigid.Count(); i++)
  {
    fParticlesRigid[i]->fMass = 1.0;
    fParticlesRigid[i]->fPosition.GetColor() = random(255);
    //fParticles[i]->fSpeed = Vector3(fParticles[i]->fPosition - Vector3(1,1,1)) ^ Vector3(0,0,1);
  }
  fParticlesRigid[0]->fSpeed = Vector3(1.5, -0.0, 0.0)*2.0;

  fSystem.Init();
  fSystem.Touch();
  fSystemRigid.Init();
  fSystemRigid.Touch();

  drawer.ClearAll();
}

void MainApp::OnKeyPressed(int aKey)
{
  switch (aKey)
  {
    case kSP : Init(); break;
    default : inherited::OnKeyPressed(aKey);
  }
}

void MainApp::Draw()
{
  drawer.ClearAll(255);
  fImage.ClearAllZ();

  // draw springs (not optimzed for the moment)
  for (int i=0; i<fActions.Count(); i++)
  {
    Action * curAction = fActions[i];
    if (_dynamic_cast_<LinkForce *>(curAction))
    {
      LinkForce * curLink = _dynamic_cast_<LinkForce *>(curAction);
      APlot cur;
      cur = fCamera.ChangeToBase(fParticles[curLink->ParticleAlpha()]->fPosition);
      fCamera.Project(cur);
      PointZ curZA = cur.GetPointZ();
      curZA.color = 111;
      cur = fCamera.ChangeToBase(fParticles[curLink->ParticleOmega()]->fPosition);
      fCamera.Project(cur);
      PointZ curZB = cur.GetPointZ();
      curZB.color = 111;
      if ((curZA.z > 0) && (curZB.z>0))
        drawer.Line(curZA, curZB);
    }
  }

  // draw particles
  for (int i=0; i<fParticles.Count(); i++)
  {
    Particle * curParticle = fParticles[i];
    APlot cur = fCamera.ChangeToBase(curParticle->fPosition);
    fCamera.Project(cur);
    PointZ curZ = cur.GetPointZ();
    if (curZ.z > 0)
    {
      int size = int(real(8.0/curZ.z))+1;
      drawer.FillCircle(curZ, size);
    }
  }

  fSystem.ProcessTimeStep();




  // draw springs (not optimzed for the moment)
  for (int i=0; i<fActionsRigid.Count(); i++)
  {
    Action * curAction = fActionsRigid[i];
    if (_dynamic_cast_<Rigid *>(curAction))
    {
      Rigid * curLink = _dynamic_cast_<Rigid *>(curAction);
      APlot cur;
      cur = fCamera.ChangeToBase(fParticlesRigid[curLink->ParticleAlpha()]->fPosition);
      fCamera.Project(cur);
      PointZ curZA = cur.GetPointZ();
      curZA.color = 211;
      cur = fCamera.ChangeToBase(fParticlesRigid[curLink->ParticleOmega()]->fPosition);
      fCamera.Project(cur);
      PointZ curZB = cur.GetPointZ();
      curZB.color = 211;
      if ((curZA.z > 0) && (curZB.z>0))
        drawer.Line(curZA, curZB);
    }
  }

  // draw particles
  for (int i=0; i<fParticlesRigid.Count(); i++)
  {
    Particle * curParticle = fParticlesRigid[i];
    APlot cur = fCamera.ChangeToBase(curParticle->fPosition);
    fCamera.Project(cur);
    PointZ curZ = cur.GetPointZ();
    if (curZ.z > 0)
    {
      int size = int(real(8.0/curZ.z))+1;
      drawer.FillCircle(curZ, size);
    }
  }

  fSystemRigid.ProcessTimeStep();

  inherited::Draw();
}

void MainApp::Cycle()
{
  Draw();
}
