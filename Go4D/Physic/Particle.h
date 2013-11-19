#ifndef _PARTICLE_H_
#define _PARTICLE_H_

/**
*AUTHOR : Babel yoann
*TITLE  : Particle.h, particle system core implementation
*CREATED: 19/05/99
*DATE   : 19/05/99
*OBJECT : Here are minimum classes that are required to build a particle
*         system. Specific forces must be added in an other unit
*         (see actions.h).
*/

#include "Graph3D/Plot.h"
#include "Tools/List.h"
#include "Tools/RTTI.h"

namespace Go4D
{

  RTTI_USE;

#define epsilonreal 0.0001

  /// a particle that "moves" while the time spend.
  struct Particle
  {
    /// the position of the particule
    APlot fPosition;
    /// it's speed
    Vector3 fSpeed;
    /// the force that is currently applying to it
    Vector3 fForce;
    /// the mass of the particule
    real fMass;
    /// some user data that can be associated to the particule
    void * fData;
  };

  /// a pointer to the particles
  typedef Particle * PParticle;

  /// an array of particles
  class ParticleArray : public Array<PParticle>
  {
    typedef Array<PParticle> inherited;
  public :
    /// constructor
    ParticleArray();
  };

  /// the kind of actions that can be applyed on the particles
  /// the kind determine the order that the actions are applyed
  /// the constraints
  enum ActionKind { akGlobalForce, akLocalForce, akGlobalConstraint, akLocalConstraint, akRigidForce };

  /// an action is a kind of force that is applyed on a particle
  class Action : RTTI_BASE
  {
  public :
    /// constructor
    Action(ParticleArray *);
    /// -1 means a specific particle, apply an action, a force or a constraint
    /// on a particle
    /// return true when collision occurs
    virtual bool Apply(int aParticleIndex) const = 0;
    /// the kind of action determine the order of applyance
    virtual ActionKind Kind() const = 0;
    /// for rigid body only, when there is a collision
    virtual void Touch() {}

  protected :
    ParticleArray * fParticles;
  };

  /// pointer to an action
  typedef Action * PAction;

  /// an array of actions used to stored all the actions that are applyed
  /// on the particles universe
  class ActionArray : public Array<PAction>
  {
    typedef Array<PAction> inherited;
  public :
    /// constructor
    ActionArray();
  };

  /// this kind of forces are applyed on all the particles
  class GlobalForce : public Action
  {
    typedef Action inherited;
  public :
    /// constructor
    GlobalForce(ParticleArray *);
    /// return the "globalForce" kind
    virtual ActionKind Kind() const;
  };

  /// this kind of force is applyed only on one or some particles
  class LocalForce : public Action
  {
    typedef Action inherited;
  public :
    /// constructor
    LocalForce(ParticleArray *);
    /// return the "localForce" kind
    virtual ActionKind Kind() const;
  };

  /// this kind of constraint is applyed on all the particles
  class GlobalConstraint : public Action
  {
    typedef Action inherited;
  public :
    /// constructor
    GlobalConstraint(ParticleArray *);
    ///
    virtual ActionKind Kind() const;
  };

  /// this kind of constraint is applyed only on one or some
  /// particles
  class LocalConstraint : public Action
  {
    typedef Action inherited;
  public :
    ///
    LocalConstraint(ParticleArray *);
    ///
    virtual ActionKind Kind() const;
  };

  class RigidForce : public Action
  {
    typedef Action inherited;
  public :
    ///
    RigidForce(ParticleArray *);
    ///
    virtual ActionKind Kind() const;
  };


  /// the universe of particles, contains all the particles
  /// and all the actions that will be applyed on them.
  /// each action is applyed for a "delta" of time and is
  /// integrated (with the euler method)
  class ParticleSystem
  {
  public :
    /// constructor
    ParticleSystem(ParticleArray *, ActionArray *, real deltaTime);
    /// destructor
    ~ParticleSystem();
    /// initialize th system
    void Init();
    /// compute the next positions of the particles and the new
    /// forces that are applyed on them
    void ProcessTimeStep();
    /// When a rigid body is touch, recompute the forces
    void Touch();

  protected :
    void InitForces(ParticleArray * );
    void Integrate(ParticleArray * initial, ParticleArray * src, ParticleArray * dst, real deltaTime);
    bool ApplyActions(ParticleArray * );
    real fDeltaTime;
    ActionArray * fActions;

    ParticleArray * fParticles;
    ParticleArray * fTmp;
    ParticleArray * fTarget;
  };

}

#endif
