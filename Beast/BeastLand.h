#ifndef _BEASTLAND_H_
#define _BEASTLAND_H_

#include "Geom.h"
#include "DNA.h"

using namespace Go4D;

enum BeastRace {brRed, brBlue, brBoth};

class IGnoky;
class IBeast;

/**
* A Gnoky is the food of the beast. When a beast eat the gnoky, it retrieve it's
* NRJ and the gnoky is deleted from the area.
*/
class IGnoky
{
public:
  /// Return the amount of NRJ of the gnoky
  virtual real GetNRJ()=0;
  /// Return the position in the space (of the area) of the gnoky
  virtual Vector3 * GetPosition()=0;
  /// This method must be called when the gnoky is eated by a beast. This method set the gnoky NRJ to 0
  virtual void Eated()=0;
};

/**
* This interface is used to instanciate new gnokies.
*/
class IGnokyFactory
{
public:
  /// create a new gnoky
  virtual IGnoky * NewGnoky()=0;
};

/**
* This is the zone where "leaves" beast and where appear gnokies. The
* area have dimension that define a square in which must be the
* positions of the beasts and gnokies.
*/
class IBeastArea
{
public:
  /// return the size of the area. The position of any element of the area (gnoky or beast) must be in -Size/2 and Size/2
  virtual real GetSize()=0;
  /// return the number of gnoky that are put on the area
  virtual int GetGnokyCount()=0;
  /// return a particular gnoky. Index must be between 0 and GetGnokyCount-1
  virtual IGnoky * GetGnoky(int Index)=0;
  /// return the number of beast that are put on the area
  virtual int GetBeastCount()=0;
  /// return a particular beast. Index must be between 0 and GetBeastCount-1
  virtual IBeast * GetBeast(int Index)=0;
};

/**
* This interface defines a beast. Generaly speaking a beast is a creature
* that move on an area in order to eat gnoky and survive. The beast is
* considered dead when it's NRJ is 0. Each movement of the beast consume
* a certain amount of NRJ. The beast can accelerate in any direction but
* it's speed is limited. A beast belong to a race. Each race is concurrent
* with other race to survive.
*/
class IBeast
{
public:
  /// return the position in space of the beast
  virtual Vector3 * GetPosition()=0;
  /// return the speed of the beast
  virtual Vector3 * GetSpeed()=0;
  /// return the race of the beast.
  virtual BeastRace GetRace()=0;
  /// compute the next position of the beast, given it's current acceleration. The movement consumes NRJ.
  virtual void Move(IBeastArea * )=0;
  /// make sure that the beast position is in the area.
  virtual void InsureIsInArea(IBeastArea * )=0;
  /// return the amount of NRJ that belongs to the beast
  virtual real GetNRJ()=0;
  /** make the beast loose some NRJ. This method can be used for
  * example when 2 beast of different race fights. Don't make
  * a beast loose NRJ after having moved because the movement
  * itself make the beast loose some NRJ.
  */
  virtual void LooseNRJ(real amount)=0;
  /// Make the beast eat a gnoky. In fact the beast NRJ is incremented by the amount of NRJ the gnoky have
  virtual void Eat(IGnoky *)=0;
  /// make the beast accelerate in a direction, the acceleration is proportional to the COEF
  virtual void Accelerate(Vector3 direction, real coef)=0;

  /** IsFeared is a property of the beast that can be used by rules
  * in order to adapt the behaviour of the beast to the presence
  * of stronger enemies.
  */
  virtual bool & IsFeared()=0;
  /// IsFighting is also a property of the beast that can be used by the rules.
  virtual bool & IsFighting()=0;
  /// reset IsFeared, IsFighting, and current acceleration to 0
  virtual void Reset()=0;

  /* If this interface is supported, then the distance between gnoky and beast
  * are precomputed once, and used by all the rules (that make intensive use
  * of distances)
  */
public:
  /// start the distance optimization computing
  virtual void BeginOptimization()=0;
  /// add to the list of precomputed distance the distance of a gnoky
  virtual void ComputeGnokyDistance(IGnoky * )=0;
  /// add to the list of precomputed distance the distance of a beast
  virtual void ComputeBeastDistance(IBeast * )=0;
  /// notify the end of distance optimization computing
  virtual void EndOptimization()=0;
  /// when the distances have been precomputed, return the precomputed distance of a gnoky
  virtual real GetGnokyDistance(int)=0;
  /// when the distances have been precomputed, return the precomputed distance of a beast
  virtual real GetBeastDistance(int)=0;

  // rules coefs
public:

  // reserve a set of integer, return the start index
  //JYB: is Obsolet
  //virtual int Reserve(real defaultValue)=0;
  ///
  virtual real GetCoef(int ruleid, int index)=0;
  //JYB: New
  ///
  virtual real GetCoefFromName(int ruleid, const char *name)=0;
  ///
  virtual int ReserveCoef(int ruleid, const char *name, real defaultValue)=0;
};

/**
* This interface is used to create beast. You can request a random beast
* or set some of it's parameters
*/
class IBeastFactory
{
public:
  /// create a new completly random beast
  virtual IBeast * NewBeast()=0;
  /// create a new beast, but it's race and start NRJ is fixed by the parameters
  virtual IBeast * NewBeast(BeastRace, real nrj)=0;
};

/**
* This interface is a callback for the objects that manage a list of
* beast and gnoky, in order to keep this list up to date. For example
* the main list of beast is owned by the BeastLand class, but other
* classes like the renderer have a list of beast. If a beast is deleted
* from the beast land, the rendrer must by notified of this delete
* in order to clear the obsolete pointer that it kept on the beast.
*/
class IBeastLandSink
{
public:
  /// notify that a beast have been created
  virtual void AddBeast(IBeast *)=0;
  /// notify that a beast have been deleted
  virtual void DelBeast(IBeast *)=0;
  /// notify that a gnoky have been created
  virtual void AddGnoky(IGnoky *)=0;
  /// notify that a gnoky have been deleted
  virtual void DelGnoky(IGnoky *)=0;
};

/**
* This interface define a rule that can be applyed on beast in order to
* change their behaviour. For instance you can create a rule that make
* a beast go away of the closest beast of the other race if it's stronger.
* Or you can make a rule to make the beast go to the closest gnoky in order
* to eat it.
*/
class IRule
{
public:
  /// return on which Race the rule is applyed on
  virtual BeastRace ApplyOn()=0;
  /** a rule can register some real coefs that will be used to
  * individuate the behaviour of a beast to a particular rule.
  * For example, the rule that make a beast feared of an other beast
  * can depend wether the beast is beyond a certain distance. This
  * distance can be different for every beast since it can be stored
  * in one coef. A rule must reserve a list of such coefs in a beast.
  */
  virtual void RegisterCoefs(IBeast *)=0;
  /// Since a rule can delete or add gnoky and beast, it must tell it to the land, a sink is used to achieve this task
  virtual void SetSink(IBeastLandSink * )=0;
  /// apply the rule to a particular beast. The area is required in order that the rule have access to other beast and gnoky of the area
  virtual void Apply(IBeast *, IBeastArea *)=0;

  /// return the ID of the rule, all rules must have a different ID
  virtual int Id()=0;
  /// return the name of the rule
  virtual const char *Name()=0;
};

/**
* This class manage the list of beast and gnoky. All gnokies and beast, and rules
* belong to this class. It apply the rules on the beast in order to make them
* move, eat, reproduce and die.
*/
class BeastLand : public IBeastLandSink
{
  friend class IBeastArea;
public:
  /// create a new land, the beast will leave "on" the area given in parameter
  BeastLand(IBeastArea *);
  ///
  virtual ~BeastLand();
  /// add a sink, all the sinks are notified of the creation/deletion of gnokies and beasts.
  void AddSink(IBeastLandSink *);
  /// add a rule that will be applyed on the beasts.
  void AddRule(IRule *);
  /// delete all gnokies and beasts (the rules are not cleared
  void Clear();
  /// add a beast in the land, using the factory given in parameter
  IBeast * CreateBeast(IBeastFactory * );
  /// add a gnoky in the land, using the factory given in parameter
  IGnoky * CreateGnoky(IGnokyFactory * );
  /** apply all the rules on the beast and make them move according
  * to the acceleration computed by the rules. Make them eat gnoky
  * if they are close enough, and make them die if their NRJ is < 0
  */
  void ComputeNextPositions();

public:
  /// if a rule create a beast it must prevent the land
  virtual void AddBeast(IBeast *);
  /// if a rule delete a beast it must prevent the land
  virtual void DelBeast(IBeast *);
  /// if a rule create a gnoky it must prevent the land
  virtual void AddGnoky(IGnoky *);
  /// if a rule delete a gnoky it must prevent the land
  virtual void DelGnoky(IGnoky *);

public:
  Array<IBeast *> fBeasts; // aggregated list of beasts
  Array<IGnoky *> fGnokies; // aggregated list of Gnokies
  Array<IRule *> fRules; // aggregated list of rules

protected :
  IBeastArea * fArea;
  List<IBeastLandSink *> fSinks;
};

#endif
