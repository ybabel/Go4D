#ifndef _RANDOMBEAST_H_
#define _RANDOMBEAST_H_

#include "BeastLand.h"
#include "Drawable.h"
#include "Random.h"
#include "DNA.h"
#include "RTTI.h"


using namespace Go4D;
RTTI_USE;

/** JYB
* Template <coeftype ...>
* a random gnoky appear somewhere on the map depending on the
* gnoky factory
*/
class RandomGnoky : RTTI_BASE, public IGnoky, public IDrawable
{
public:
  ///
  RandomGnoky();
  ///
  virtual ~RandomGnoky() {}
  ///
  int fZone;
  ///
  RTTI_DECL;

public: // IGnoky
  ///
  virtual real GetNRJ();
  ///
  virtual Vector3 * GetPosition();
  ///
  virtual void Eated();

public: // IDrawable
  ///
  virtual Vector3 * GetDirection();
  ///
  virtual byte GetColor();
  ///
  virtual DrawableShape GetShape();
  ///
  virtual real GetSize();

protected:
  real fNRJ;
  Vector3 fPosition;
  Vector3 fDirection;
};

/// create randomly positionned gnoky on the map
/// the ramdomizer is unifor, so there is the same amount
/// of gnoky that appear every where
class RandomGnokyFactory : public IGnokyFactory
{
public:
  ///
  RandomGnokyFactory();
  ///
  virtual IGnoky * NewGnoky();
};


/// This gnoky factory create gnoky, the distribution is not
/// uniform, so there can be more gnoky that appear at some
/// points.
class ZoneGnokyFactory : public IGnokyFactory
{
public:
  ///
  ZoneGnokyFactory(HeightField * field);
  ///
  virtual IGnoky * NewGnoky();

protected:
  HeightField * fField;
  NonUniformVector3Randomizer fRandomizer;
};

/// This is a beast that is created by the evolution of other beasts
class RandomBeast : RTTI_BASE, public IBeast, public IDrawable, public IDNA
{
public:
  ///
  RandomBeast();
  ///
  RandomBeast(BeastRace, real nrj);
  ///
  virtual ~RandomBeast() {}
  ///
  RTTI_DECL;

public: // IDrawable
  ///
  virtual Vector3 * GetPosition();
  ///
  virtual Vector3 * GetDirection();
  ///
  virtual byte GetColor();
  ///
  virtual DrawableShape GetShape();
  ///
  virtual real GetSize();

public: // IBeast
  ///
  virtual Vector3 * GetSpeed();
  ///
  virtual BeastRace GetRace();
  ///
  virtual void Move(IBeastArea * );
  ///
  virtual void InsureIsInArea(IBeastArea * );
  ///
  virtual real GetNRJ();
  ///
  virtual void LooseNRJ(real );
  ///
  virtual void Eat(IGnoky *);
  ///
  virtual void Accelerate(Vector3, real);
  ///
  virtual bool & IsFeared();
  ///
  virtual bool & IsFighting();
  ///
  virtual void Reset();
  virtual void ModifyDNA(real globalpercentage);

public: // IBeast optimized
  ///
  virtual void BeginOptimization();
  ///
  virtual void ComputeGnokyDistance(IGnoky * gnoky);
  ///
  virtual void ComputeBeastDistance(IBeast * beast);
  ///
  virtual void EndOptimization();
  ///
  virtual real GetGnokyDistance(int);
  ///
  virtual real GetBeastDistance(int);

public: // IBeast coefs
  //virtual int Reserve(real defaultValue);
  ///
  int ReserveCoef(int ruleid, const char *name, real defaultValue);
  ///
  virtual real GetCoef(int ruleid, int Index);
  ///
  virtual real GetCoefFromName(int ruleid, const char *);

protected :
  Vector3 fPosition;
  Vector3 fSpeed;
  Vector3 fDirection;
  Vector3 fAcc;
  BeastRace fRace;
  real fNRJ;
  real SetInLimit(real , real limit);
  bool fIsFeared;
  bool fIsFighting;
  Array<real *> fGnokyDist;
  Array<real *> fBeastDist;
  void Init();
};

/// This is a creator of beasts
class RandomBeastFactory : public IBeastFactory
{
public:
  ///
  RandomBeastFactory();
  ///
  virtual IBeast * NewBeast();
  ///
  virtual IBeast * NewBeast(BeastRace, real nrj);
};

/// this is the area where the gnoky appear, and the beasts fight for their
/// survival
class RandomBeastArea : RTTI_BASE, public IDrawableMapped, public IBeastArea, public IBeastLandSink
{
public:
  RTTI_DECL;
  ///
  RandomBeastArea(real size=5.0);
  ///
  virtual ~RandomBeastArea() {}
  ///
  void Init(BeastLand *, Renderer *);
  ///
  void SetMappedImage(const Image * );

public: // IDrawable
  ///
  virtual Vector3 * GetPosition();
  ///
  virtual Vector3 * GetDirection();
  ///
  virtual byte GetColor();
  ///
  virtual DrawableShape GetShape();
  ///
  virtual real GetSize();

public: // IDrawableMapped
  ///
  virtual const Image * GetMappedImage();

public: // IBeastArea
  ///
  virtual int GetGnokyCount();
  ///
  virtual IGnoky * GetGnoky(int Index);
  ///
  virtual int GetBeastCount();
  ///
  virtual IBeast * GetBeast(int Index);

public: // IBeastLandSink
  ///
  virtual void AddBeast(IBeast *);
  ///
  virtual void DelBeast(IBeast *);
  ///
  virtual void AddGnoky(IGnoky *);
  ///
  virtual void DelGnoky(IGnoky *);

protected:
  BeastLand * fBeastLand;
  Renderer * fRenderer;
  const Image * fMappedImage;
  real fSize;
  Vector3 fPosition;
  Vector3 fDirection;
};

enum
{
  RULEID_UNKNOWN = -1,
  RULEID_EATGNOKY = 0,
  RULEID_SEARCHGNOKY,
  RULEID_DEFENDGNOKY,
  RULEID_FEAROTHERRACE,
  RULEID_HORTOFEAROTHERRACE,
  RULEID_DONTCOLLIDE,
  RULEID_FIGHTOTHERRACE,
  RULEID_FINDFRIEND,
  RULEID_FINDFRIENDWHENFEARED,
  RULEID_CLONEWHENSTRONG,
  RULEID_FRIENDSBARYCENTER,
  RULEID_ENNEMIESBARYCENTER
};


/// this is a rule that can be applyed to randomgnokies and randombeasts
class RandomRule : public IRule
{
protected:
  BeastRace fApplyOn;
  //int fStartIndex;
  real GetCoef(IBeast *, int);

  IBeastLandSink * fSink;
  IGnoky * GetClosestGnoky(IBeastArea *, IBeast *);
  // search beast with race is NOT BeastRace
  IBeast * GetClosestBeast(IBeastArea *, IBeast *, BeastRace );
  IBeast * GetClosestBeast(IBeastArea *, IBeast *);

public: // IRule default implementation
  ///
  RandomRule(BeastRace race=brBoth);
  ///
  virtual void SetSink(IBeastLandSink * );
  ///
  virtual BeastRace ApplyOn();
  ///
  virtual inline int Id() { return RULEID_UNKNOWN; }
  ///
  virtual inline const char *Name() { return "RandomGenericRule"; }
};

/// this rule determine the conditions for a gnoky to be eated by a beast
class EatGnokyRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  EatGnokyRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_EATGNOKY; }
  ///
  virtual inline const char *Name() { return "EatGnokyRule"; }
};

/// this rule determine how a beast search a gnoky
class SearchGnokyRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  SearchGnokyRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_SEARCHGNOKY; }
  ///
  virtual inline const char *Name() { return "SearchGnokyRule"; }
};

/// this rule determine how a beast defend itself against
/// beasts of the other races
class DefendGnokyRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  DefendGnokyRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_DEFENDGNOKY; }
  ///
  virtual inline const char *Name() { return "DefendGnokyRule"; }
};

/// this rule determine when a beast is feared by other beasts
class FearOtherRaceRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  FearOtherRaceRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_FEAROTHERRACE; }
  ///
  virtual inline const char *Name() { return "FearOtherRaceRule"; }
};

/// this rule determine a kinf of reaction to escape other
/// beasts when it's frightned
class HortoFearOtherRaceRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  HortoFearOtherRaceRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_HORTOFEAROTHERRACE; }
  ///
  virtual inline const char *Name() { return "HortoFearOtherRaceRule"; }
};

/// this rule prevent beasts to collide
class DontCollideRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  DontCollideRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_DONTCOLLIDE; }
  ///
  virtual inline const char *Name() { return "DontCollideRule"; }
};

/// this rule determine what append when 2 beast of
/// opposed rase encounter themselves
class FightOtherRaceRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  FightOtherRaceRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_FIGHTOTHERRACE; }
  ///
  virtual inline const char *Name() { return "FightOtherRaceRule"; }
};

/// This rule says to beasts to find friends when fight is needed
class FindFriendRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  FindFriendRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_FINDFRIEND; }
  ///
  virtual inline const char *Name() { return "FindFriendRule"; }

};

/// this rule says that a beasts must find a friend
/// when it's feared
class FindFriendWhenFearedRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  FindFriendWhenFearedRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_FINDFRIENDWHENFEARED; }
  ///
  virtual inline const char *Name() { return "FindFriendWhenFearedRule"; }
};

/// this rule says that gnoky make childs when their are enough strong
/// and what happent to their enery
class CloneWhenStrongRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  CloneWhenStrongRule(IBeastFactory *, BeastRace race=brBoth);
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_CLONEWHENSTRONG; }
  ///
  virtual inline const char *Name() { return "CloneWhenStrongRule"; }

protected:
  IBeastFactory * fFactory;
};


/// this rule says where are friend beasts, in average
class FriendsBarycenterRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  FriendsBarycenterRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_FRIENDSBARYCENTER; }
  ///
  virtual inline const char *Name() { return "FriendsBarycenterRule"; }

protected:
  IBeastFactory * fFactory;
};


/// this rule determine where are located enemies in average
class EnnemiesBarycenterRule : public RandomRule
{
  typedef RandomRule inherited;
public:
  ///
  EnnemiesBarycenterRule(BeastRace race=brBoth) : inherited(race) {}
  ///
  virtual void RegisterCoefs(IBeast *);
  ///
  virtual void Apply(IBeast *, IBeastArea *);
  ///
  virtual inline int Id() { return RULEID_ENNEMIESBARYCENTER; }
  ///
  virtual inline const char *Name() { return "EnnemiesBarycenterRule"; }

protected:
  IBeastFactory * fFactory;
};


#endif
