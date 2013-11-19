#ifndef _ACTIONS_H_
#define _ACTIONS_H_

/**
*AUTHOR : Babel yoann
*TITLE  : Actions.h, actions that can be performed on the particles
*CREATED: 18/05/99
*DATE   : 18/05/99
*OBJECT : Basic actions : gravity, ground, springs, viscosity, ...
*/

#include "Particle.h"

namespace Go4D
{


  /// The gravity force. The attrication can be fixed
  class Gravity : public GlobalForce
  {
    typedef GlobalForce inherited;
  public :
    RTTI_DECL;
    /// constructor
    Gravity(ParticleArray *, real g);
    /// apply the force to a particle
    virtual bool Apply(int aParticleIndex) const;

  protected :
    real fG;
  };

  /// this constraint create a "ground" for the particles.
  /// they can go through the ground, and they are slowed by frictions
  /// on it
  class Ground : public GlobalConstraint
  {
    typedef GlobalConstraint inherited;
  public :
    RTTI_DECL;
    /// constuctor
    Ground(ParticleArray *, real z, real restitution, real frot=1.0);
    ///
    virtual bool Apply(int aParticleIndex) const;

  protected :
    real fZ;
    real fRestitution;
    real fFrot;
  };

  /// add a global viscosity to the system. All small viscosity
  /// simulated the space, an average simulate the air and a strong
  /// one simulate the water
  class Viscosity : public GlobalForce
  {
    typedef GlobalForce inherited;
  public :
    RTTI_DECL;
    ///
    Viscosity(ParticleArray *, real coef);
    ///
    virtual bool Apply(int aParticleIndex) const;

  protected :
    real fCoef;
  };

  /// this global force can increase or decrease the mass of a particle depending
  /// of it's position
  /// for the moment only the z position is tested but in final many methods
  /// can be used
  class MassModifier : public GlobalForce
  {
    typedef GlobalForce inherited;
  public :
    RTTI_DECL;
    ///
    MassModifier(ParticleArray *, real coef);
    ///
    virtual bool Apply(int aParticleIndex) const;
    ///
    virtual bool IsIn(int aParticleIndex) const=0;

  protected :
    real fCoef;
  };

  /// a mass modifier based on the Z position of the particle
  class ZMassModifier : public MassModifier
  {
    typedef MassModifier inherited;
  public :
    RTTI_DECL;
    ///
    ZMassModifier(ParticleArray *, real coef, real Z, bool mustBeAbove);
    ///
    virtual bool IsIn(int aParticleIndex) const;

  protected :
    real fZ;
    bool fMustBeAbove;
  };

  /// Fix the position of a particle to a constant location
  class FixPosition : public LocalConstraint
  {
    typedef LocalConstraint inherited;
  public :
    RTTI_DECL;
    ///
    FixPosition(ParticleArray *, int particle);
    ///
    virtual bool Apply(int aParticleIndex) const;
    ///
    bool fixX;
    bool fixY;
    bool fixZ;

  protected :
    int fParticle;
  };

  /// abstract class for all "dual" forces that link 2 particles
  /// like a spring, or the gravity or electric forces
  class LinkForce : public LocalForce
  {
    typedef LocalForce inherited;
  public :
    RTTI_DECL;
    ///
    LinkForce(ParticleArray *, int particleAlpha, int particleOmega);
    ///
    int ParticleAlpha() const { return fParticleAlpha; }
    ///
    int ParticleOmega() const { return fParticleOmega; }

  protected :
    int fParticleAlpha;
    int fParticleOmega;
  };

  /// simulate a spring that lies 2 particles
  class Spring : public LinkForce
  {
    typedef LinkForce inherited;
    friend class Muscle;
  public :
    RTTI_DECL;
    ///
    Spring(ParticleArray *, int particleAlpha, int particleOmega,
      real spring, real damping, real length);
    ///
    virtual bool Apply(int aParticleIndex) const;

  protected :
    real fSpring;
    real fDamping;
    real fLength;
  };

  /// a muscle is a spring that is contracted with a certain perdiod
  /// of time
  class Muscle : public LocalForce
  {
    typedef LocalForce inherited;
  public :
    RTTI_DECL;
    ///
    Muscle(ParticleArray *, Spring *, real contractCoef, real relaxCoef, int contractTime, int relaxTime, int startTime);
    ///
    virtual bool Apply(int aParticleIndex) const;

  protected :
    mutable Spring * fSpring;
    mutable real fInitLen;
    mutable real fContractCoef;
    mutable real fRelaxCoef;
    mutable int fContractTime;
    mutable int fRelaxTime;
    mutable int fStartTime;
    mutable int fCurrentTime;
  };

  /// a bar is a "rigid" spring that link 2 particles
  /// not implemented for the moment
  class Bar : public LinkForce
  {
    typedef LinkForce inherited;
  protected :
    real fDeltaTime;
    real fLength;

  public :
    RTTI_DECL;
    ///
    Bar(ParticleArray *, int particleAlpha, int particleOmega, real deltaTime);
    ///
    virtual bool Apply(int aParticleIndex) const;
    ///
    virtual void Touch();
  };

  /// all the particles in the body behave together what ever force is applyed on it
  class Rigid : public RigidForce
  {
    typedef RigidForce inherited;
    Vector3 fMomentum;
    Vector3 fSpeed;
    real fDamping; // Damping
  public:
    RTTI_DECL;
    ///
    Rigid(ParticleArray*, int particleAlpha, int particleOmega, real damping);
    ///
    virtual bool Apply(int aParticleIndex) const;
    ///
    virtual void Touch();
    ///
    int ParticleAlpha() const { return fParticleAlpha; }
    ///
    int ParticleOmega() const { return fParticleOmega; }

  protected :
    int fParticleAlpha;
    int fParticleOmega;
  };

}

#endif
