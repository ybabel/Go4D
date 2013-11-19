#include "Actions.h"

namespace Go4D
{

  RTTI_IMPL_2(Gravity, Action, GlobalForce);

  Gravity::Gravity(ParticleArray *pa, real g)
    : inherited(pa)
  {
    fG = g;
  }

  bool Gravity::Apply(int aIndex) const
  {
    Particle * particle = (*fParticles)[aIndex];
    particle->fForce += Vector3(0,0,fG)*particle->fMass;
    return false;
  }

  RTTI_IMPL_2(Ground, Action, GlobalConstraint);

  Ground::Ground(ParticleArray *pa, real z, real restitution, real frot)
    : inherited(pa)
  {
    fZ = z;
    fRestitution = -restitution;
    fFrot = frot;
  }

  bool Ground::Apply(int aIndex) const
  {
    Particle * particle = (*fParticles)[aIndex];
    if (particle->fPosition[2] < fZ)
    {
      particle->fSpeed[0] *= fFrot;
      particle->fSpeed[1] *= fFrot;
      particle->fSpeed[2] *= fRestitution;
      particle->fPosition[2] = fZ;
      return true;
    }
    return false;
  }

  RTTI_IMPL_2(Viscosity, Action, GlobalForce);

  Viscosity::Viscosity(ParticleArray *pa, real coef)
    : inherited(pa)
  {
    fCoef = coef;
  }

  bool Viscosity::Apply(int aIndex) const
  {
    Particle * particle = (*fParticles)[aIndex];
    particle->fForce -= particle->fSpeed*fCoef;
    return false;
  }

  RTTI_IMPL_2(MassModifier, Action, GlobalForce);

  MassModifier::MassModifier(ParticleArray *pa, real coef)
    : inherited(pa)
  {
    fCoef = coef;
  }

  bool MassModifier::Apply(int aIndex) const
  {
    Particle * particle = (*fParticles)[aIndex];
    if (IsIn(aIndex))
      particle->fMass += fCoef;
    return false;
  }

  RTTI_IMPL_3(ZMassModifier, MassModifier, Action, GlobalForce);

  ZMassModifier::ZMassModifier(ParticleArray *pa, real coef, real Z, bool mustBeAbove)
    : inherited(pa, coef)
  {
    fZ = Z;
    fMustBeAbove = mustBeAbove;
  }

  bool ZMassModifier::IsIn(int aIndex) const
  {
    Particle * particle = (*fParticles)[aIndex];
    if (fMustBeAbove)
      return particle->fPosition[2] > fZ;
    else
      return particle->fPosition[2] < fZ;
  }

  RTTI_IMPL_2(FixPosition, Action, LocalConstraint);

  FixPosition::FixPosition(ParticleArray *pa, int particle)
    : inherited(pa)
  {
    fParticle = particle;
    fixX = true;
    fixY = true;
    fixZ = true;
  }

  bool FixPosition::Apply(int) const
  {
    /*(*fParticles)[fParticle]->fForce = Vector3(0,0,0);
    (*fParticles)[fParticle]->fSpeed = Vector3(0,0,0);*/
    if (fixX)
    {
      (*fParticles)[fParticle]->fForce[0]=0;
      (*fParticles)[fParticle]->fSpeed[0]=0;
    }
    if (fixY)
    {
      (*fParticles)[fParticle]->fForce[1]=0;
      (*fParticles)[fParticle]->fSpeed[1]=0;
    }
    if (fixZ)
    {
      (*fParticles)[fParticle]->fForce[2]=0;
      (*fParticles)[fParticle]->fSpeed[2]=0;
    }
    return true;
  }

  RTTI_IMPL_2(LinkForce, Action, LocalForce);

  LinkForce::LinkForce(ParticleArray *pa, int particleAlpha, int particleOmega)
    : inherited(pa)
  {
    fParticleAlpha = particleAlpha;
    fParticleOmega = particleOmega;
  }

  RTTI_IMPL_3(Spring, Action, LocalForce, LinkForce);

  Spring::Spring(ParticleArray *pa, int particleA, int particleO,
    real spring, real damping, real length)
    : inherited(pa, particleA, particleO)
  {
    fSpring = spring;
    fDamping = damping;
    fLength = length;
  }

#define pA (*(*fParticles)[fParticleAlpha])
#define pO (*(*fParticles)[fParticleOmega])
  bool Spring::Apply(int) const
  {
    Vector3 pAO = pA.fPosition-pO.fPosition;
    Vector3 vAO = pA.fSpeed-pO.fSpeed;
    real pAONorme = pAO.Norme();
    if (pAONorme<epsilonreal) return false;
    Vector3 pAONormalized=pAO/pAONorme;
    real forceCoef = fSpring*(pAONorme-fLength)+fDamping*(vAO*pAONormalized);
    Vector3 force=pAONormalized*forceCoef;

    pA.fForce -= force;
    pO.fForce += force;
    return false;
  }


  RTTI_IMPL_2(Muscle, Action, LocalForce);

  Muscle::Muscle(ParticleArray *pa, Spring *s,
    real contractCoef, real relaxCoef, int contractTime, int relaxTime, int startTime)
    : inherited(pa)
  {
    fContractCoef = contractCoef;
    fRelaxCoef = relaxCoef;
    fContractTime = contractTime;
    fRelaxTime = relaxTime;
    fStartTime = startTime;
    fCurrentTime = fStartTime;
    fSpring = s;
    fInitLen = fSpring->fLength;
  }

  bool Muscle::Apply(int aParticleIndex) const
  {
    int time = fCurrentTime;
    while (time > fContractTime+fRelaxTime)
      time -= fContractTime+fRelaxTime;
    fSpring->fLength = fInitLen;
    if (time<fContractTime)
      fSpring->fLength *= fContractCoef;
    else
      fSpring->fLength *= fRelaxCoef;

    fCurrentTime++;
    return false;
  }

  RTTI_IMPL_3(Bar, Action, LocalForce, LinkForce);

  Bar::Bar(ParticleArray *pa, int particleA, int particleO, real deltaTime)
    : inherited(pa, particleA, particleO)
  {
    fDeltaTime = deltaTime;
    fLength = 0;
  }

  void Bar::Touch()
  {
    if (fLength == 0)
      fLength = Vector3(pA.fPosition - pO.fPosition).Norme();
  }

  bool Bar::Apply(int) const
  {
    // we use a Spring approximation : super strong spring, and strong damping
    // damping is done by computing the next position of the particle (+=speed/deltaTime)
    // instead of the actual position ! the trick works well
    // the force is corrected by a coef of SQRT(deltaTime)
    real coef = 1/fDeltaTime;
    Vector3 pAO = (pA.fPosition+pA.fSpeed*fDeltaTime) - (pO.fPosition+pO.fSpeed*fDeltaTime);
    real pAONorme = pAO.Norme();
    if (pAONorme<epsilonreal) return false;
    Vector3 pAONormalized=pAO/pAONorme;
    real forceCoef = (pAONorme-fLength)*coef;
    Vector3 force=pAONormalized*forceCoef*sqrt(coef);
    pA.fForce -= force;
    pO.fForce += force;
    return false;
  }

  RTTI_IMPL_2(Rigid, Action, RigidForce);

  Rigid::Rigid(ParticleArray *pa, int particleA, int particleO, real damping)
    : inherited(pa)
  {
    fParticleAlpha = particleA;
    fParticleOmega = particleO;
    fDamping = damping;
  }

#define pI ((*fParticles)[i])
#define massI (pI->fMass)
#define speedI (pI->fSpeed)
#define posI (pI->fPosition)
  void Rigid::Touch()
  {
      // compute the center of mass
      int i;
      Vector3 center(0,0,0);
      Vector3 speed(0,0,0);
      real mass=0;
      //cout << "BODY ITERATION -----------------" << endl;
      for (i=fParticleAlpha; i<fParticleOmega+1; i++)
      {
          center += posI * massI;
          speed += speedI * massI;
          mass += massI;
          //cout << "    Particle " << i << " - POS " << posI << " - SPEED " << speedI << endl;
      }
      center /= mass;
      speed /= mass;
      //cout << "  CENTER " << center << " - AVGSPPED " << speed << endl;
      /// compute the average speed and momentum
      Vector3 speedrel(0,0,0);
      Vector3 momentum(0,0,0);
      for (i=fParticleAlpha; i<fParticleOmega+1; i++)
      {
        // speed
        Vector3 dI = posI - center;
        Vector3 speedIrel = speedI - speed;
        real cosinus = dI.Normalized() * speedIrel.Normalized();
        cosinus = abs(cosinus) < zero ? 0 : cosinus;
        Vector3 speedIproj = speedIrel * cosinus;
        speedrel += speedIproj * massI;

        // momentum
        Vector3 rotI = speedIrel - speedIproj;
        momentum += (rotI ^ dI) * massI;
        //cout << "    SPEEDiREL " << i << "  " << speedIrel << " speedrel " << speedrel << " COS " << cosinus;
        //cout << "  SPEEDiPROJ " << speedIproj << " ROTi " << rotI << " momenttum " << momentum << endl;
      }
      speedrel /= mass;
      momentum /= mass*2/fDamping;
      //cout << "  SPEEDREL " << speedrel << " - MOMENTUM " << momentum << endl;
      fMomentum = momentum;
      fSpeed = speed + speedrel;
      /// reset the velocity of each point depending on the speed and momentum
      for (i=fParticleAlpha; i<fParticleOmega+1; i++)
      {
        speedI = fSpeed;
      }
      //cout << "  SPEED " << fSpeed << " - MOMENTUM " << fMomentum << endl;
  }

  bool Rigid::Apply(int) const
  {
      int i;
      Vector3 center(0,0,0);
      Vector3 speed(0,0,0);
      real mass=0;
      for (i=fParticleAlpha; i<fParticleOmega+1; i++)
      {
          center += posI * massI;
          speed += speedI * massI;
          mass += massI;
      }
      speed /= mass;
      center /= mass;
      /// reset the velocity of each point depending on the speed and momentum
      for (i=fParticleAlpha; i<fParticleOmega+1; i++)
      {
        Vector3 dI = posI - center;
        speedI = speed;
        speedI += dI ^ fMomentum;
      }
      return false;
  }

}
