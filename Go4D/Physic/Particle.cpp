#include "Particle.h"

namespace Go4D
{


  ParticleArray::ParticleArray()
    : inherited(true)
  {
  }

  ActionArray::ActionArray()
    : inherited(true)
  {
  }

  Action::Action(ParticleArray * pa)
    : fParticles(pa)
  {
  }

  GlobalForce::GlobalForce(ParticleArray *pa)
    : inherited(pa)
  {
  }

  ActionKind GlobalForce::Kind() const
  {
    return akGlobalForce;
  }

  LocalForce::LocalForce(ParticleArray *pa)
    : inherited(pa)
  {
  }

  ActionKind LocalForce::Kind() const
  {
    return akLocalForce;
  }

  GlobalConstraint::GlobalConstraint(ParticleArray *pa)
    : inherited(pa)
  {
  }

  ActionKind GlobalConstraint::Kind() const
  {
    return akGlobalConstraint;
  }

  LocalConstraint::LocalConstraint(ParticleArray *pa)
    : inherited(pa)
  {
  }

  ActionKind LocalConstraint::Kind() const
  {
    return akLocalConstraint;
  }

  RigidForce::RigidForce(ParticleArray *pa)
    : inherited(pa)
  {
  }

  ActionKind RigidForce::Kind() const
  {
    return akRigidForce;
  }

  ParticleSystem::ParticleSystem(ParticleArray *pa, ActionArray *aa, real deltaTime)
    : fParticles(pa),
    fActions(aa),
    fTmp(new ParticleArray()),
    fTarget(new ParticleArray()),
    fDeltaTime(deltaTime)
  {
  }

  ParticleSystem::~ParticleSystem()
  {
  }

  void ParticleSystem::Init()
  {
    fTarget->Copy(*fParticles);
    fTmp->Copy(*fParticles);
  }


  void ParticleSystem::InitForces(ParticleArray * particles)
  {
    for (int j=0; j<particles->Count(); j++)
      (*particles)[j]->fForce = Vector3(0,0,0);
  }

  void ParticleSystem::Integrate(ParticleArray * initial, ParticleArray * src, ParticleArray * dst, real deltaTime)
  {
    int j;
    assert(src->Count() == dst->Count());
    assert(src->Count() == initial->Count());
    for (j=0; j<src->Count(); j++)
    {
      //(*dst)[j]->fSpeed += (*initial)[j]->fForce*(fDeltaTime/(*src)[j]->fMass);
      (*dst)[j]->fSpeed[0] += (*initial)[j]->fForce[0]*(fDeltaTime/(*src)[j]->fMass);
      (*dst)[j]->fSpeed[1] += (*initial)[j]->fForce[1]*(fDeltaTime/(*src)[j]->fMass);
      (*dst)[j]->fSpeed[2] += (*initial)[j]->fForce[2]*(fDeltaTime/(*src)[j]->fMass);
    }
    for (j=0; j<src->Count(); j++)
    {
      //(*dst)[j]->fPosition += (*initial)[j]->fSpeed*fDeltaTime;
      (*dst)[j]->fPosition[0] += (*initial)[j]->fSpeed[0]*fDeltaTime;
      (*dst)[j]->fPosition[1] += (*initial)[j]->fSpeed[1]*fDeltaTime;
      (*dst)[j]->fPosition[2] += (*initial)[j]->fSpeed[2]*fDeltaTime;
    }
  }

  bool ParticleSystem::ApplyActions(ParticleArray * particles)
  {
    int i;
    bool result = false;
    // apply global forces
    for (i=0; i<fActions->Count(); i++)
    {
      const Action * curAction = (*fActions)[i];
      if (curAction->Kind() == akGlobalForce)
        for (int j=0; j<particles->Count(); j++)
          result |= curAction->Apply(j);
    }

    // apply local forces
    for (i=0; i<fActions->Count(); i++)
    {
      const Action * curAction = (*fActions)[i];
      if (curAction->Kind() == akLocalForce)
        result |= curAction->Apply(-1);
    }

    // apply global constraints
    for (i=0; i<fActions->Count(); i++)
    {
      const Action * curAction = (*fActions)[i];
      if (curAction->Kind() == akGlobalConstraint)
        for (int j=0; j<particles->Count(); j++)
          result |= curAction->Apply(j);
    }

    // apply local constraints
    for (i=0; i<fActions->Count(); i++)
    {
      const Action * curAction = (*fActions)[i];
      if (curAction->Kind() == akLocalConstraint)
        result |= curAction->Apply(-1);
    }

    if (result) Touch();

    // apply rigid forces
    for (i=0; i<fActions->Count(); i++)
    {
      const Action * curAction = (*fActions)[i];
      if (curAction->Kind() == akRigidForce)
        //if (!result)
        result |= curAction->Apply(-1);
    }

    return result;
  }

  void ParticleSystem::Touch()
  {
    int i;
    // apply global forces
    for (i=0; i<fActions->Count(); i++)
    {
      Action * curAction = (*fActions)[i];
      curAction->Touch();
    }
  }

  void ParticleSystem::ProcessTimeStep()
  {
    // Modified Euler method !!!, work better than Euler and Midpoint
    InitForces(fParticles);
    ApplyActions(fParticles);
    Integrate(fParticles, fParticles, fParticles, fDeltaTime);

    /*
    // Euler method
    InitForces(*fParticles);
    ApplyActions(*fParticles);
    Integrate(*fParticles, *fParticles, *fTarget, fDeltaTime);
    const ParticleArray * tmp = fParticles;
    fParticles = fTarget;
    fTarget = tmp;
    */

    /*
    // Midpoint method
    InitForces(*fParticles);
    ApplyActions(*fParticles);
    Integrate(*fParticles, *fParticles, *fTmp, fDeltaTime/2.0);
    InitForces(*fTmp);
    ApplyActions(*fTmp);
    Integrate(*fParticles, *fTmp, *fParticles, fDeltaTime);
    //Integrate(*fParticles, *fTmp, *fTarget, fDeltaTime);
    //const ParticleArray * tmp = fParticles;
    //fParticles = fTarget;
    //fTarget = tmp;
    */
  }

}
