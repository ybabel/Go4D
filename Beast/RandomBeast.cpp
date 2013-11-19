#include "RandomBeast.h"
#include "Common.h"
#include <string.h>

#define TIMEEPSILON 0.1
#define NRJCONSUME 0.25

/*****************************************************************************\
*  RandomGnoky
\*****************************************************************************/

RTTI_IMPL_2(RandomGnoky, IGnoky, IDrawable);

RandomGnoky::RandomGnoky()
{
  fNRJ = 25.0;
  fPosition[0] = frand*5.0-2.5;
  fPosition[1] = frand*5.0-2.5;
  fPosition[2] = 0.0f;
  fDirection[0] = 1.0f;
  fDirection[1] = 0.0f;
  fDirection[2] = 0.0f;
}

real RandomGnoky::GetNRJ()
{
  return fNRJ;
}

void RandomGnoky::Eated()
{
  fNRJ = 0.0;
}

Vector3 * RandomGnoky::GetPosition()
{
  return &fPosition;
}

Vector3 * RandomGnoky::GetDirection()
{
  return &fDirection;
}

byte RandomGnoky::GetColor()
{
  return 64;
}

DrawableShape RandomGnoky::GetShape()
{
  return dsSquare;
}

real RandomGnoky::GetSize()
{
  return 0.1;
}

/*****************************************************************************\
*  RandomGnokyFactory
\*****************************************************************************/

RandomGnokyFactory::RandomGnokyFactory()
{
}

IGnoky * RandomGnokyFactory::NewGnoky()
{
  return new RandomGnoky();
}

/*****************************************************************************\
*  ZoneGnokyFactory
\*****************************************************************************/

ZoneGnokyFactory::ZoneGnokyFactory(HeightField * field)
: fField(field),
fRandomizer(fField)
{
  fRandomizer.SetAffineTransform(8, -4);
}

IGnoky * ZoneGnokyFactory::NewGnoky()
{
  IGnoky * gnok = new RandomGnoky();
  *gnok->GetPosition() = fRandomizer.Generate();
  (*gnok->GetPosition())[2] = 0;
  return gnok;
}

/*****************************************************************************\
*  RandomBeast
\*****************************************************************************/

RTTI_IMPL_3(RandomBeast, IBeast, IDrawable, IDNA);

RandomBeast::RandomBeast()
{
  fRace = (frand>0.5)?brRed:brBlue;
  fNRJ = 50.0;
  Init();
}

RandomBeast::RandomBeast(BeastRace race, real nrj)
{
  fRace = race;
  fNRJ = nrj;
  Init();
}

void RandomBeast::Init()
{
  fPosition[0] = frand*5.0-2.5;
  fPosition[1] = frand*5.0-2.5;
  fPosition[2] = 0.0f;
  fDirection[0] = 1.0f;
  fDirection[1] = 0.0f;
  fDirection[2] = 0.0f;
}

Vector3 * RandomBeast::GetPosition()
{
  return &fPosition;
}

Vector3 * RandomBeast::GetDirection()
{
  return &fDirection;
}

Vector3 * RandomBeast::GetSpeed()
{
  return &fSpeed;
}

byte RandomBeast::GetColor()
{
  byte result = 0;
  switch (fRace)
  {
  case brRed : result = 0; break;
  case brBlue : result = 160; break;
  default : return 0;
  }
  if (fIsFighting)
    result = 255;
  else if (fIsFeared)
    result = byte(result+8);
  return result;
}

DrawableShape RandomBeast::GetShape()
{
  return dsTriangle;
}

real RandomBeast::GetSize()
{
  return 0.2;
}

BeastRace RandomBeast::GetRace()
{
  return fRace;
}

void RandomBeast::Move(IBeastArea * area)
{
  fSpeed += fAcc*TIMEEPSILON;
  fPosition += fSpeed*TIMEEPSILON;
  real norme = fSpeed.Norme();
  if (norme > 1.0)
  {
    fSpeed.Normalize();
    norme = 1.0;
  }
  fNRJ -= norme*NRJCONSUME;
  if (norme > 0.001)
  {
    fDirection = fSpeed;
    fDirection.Normalize();
  }
  else
    fDirection = Vector3(1,0,0);
}

void RandomBeast::Accelerate(Vector3 acc, real coef)
{
  fAcc += acc*coef;
}

real RandomBeast::GetNRJ()
{
  return fNRJ;
}

void RandomBeast::LooseNRJ(real quantity)
{
  fNRJ -= quantity;
}

void RandomBeast::Eat(IGnoky * gnoky)
{
  fNRJ += gnoky->GetNRJ();
}

real RandomBeast::SetInLimit(real r, real limit)
{
  if (r<0.0)
  {
    if (r<-limit)
      r = -limit;
  }
  else
  {
    if (r>limit)
      r = limit;
  }
  return r;
}

void RandomBeast::InsureIsInArea(IBeastArea * area)
{
  fPosition[0] = SetInLimit(fPosition[0], area->GetSize()/2.0);
  fPosition[1] = SetInLimit(fPosition[1], area->GetSize()/2.0);
}

void RandomBeast::BeginOptimization()
{
  fGnokyDist.Clear();
  fBeastDist.Clear();
}

void RandomBeast::ComputeGnokyDistance(IGnoky * gnoky)
{
  real dist = Vector3(*gnoky->GetPosition()-fPosition).Norme();
  fGnokyDist.Add( new real(dist) );
}

void RandomBeast::ComputeBeastDistance(IBeast * beast)
{
  real dist;
  if (beast==this)
    dist = FLT_MAX;
  else
    dist = Vector3(*beast->GetPosition()-fPosition).Norme();
  fBeastDist.Add( new real(dist) );
}

void RandomBeast::EndOptimization()
{
  fGnokyDist.Complete();
  fBeastDist.Complete();
}

real RandomBeast::GetGnokyDistance(int i)
{
  return *fGnokyDist[i];
}

real RandomBeast::GetBeastDistance(int i)
{
  return *fBeastDist[i];
}

bool & RandomBeast::IsFeared()
{
  return fIsFeared;
}

bool & RandomBeast::IsFighting()
{
  return fIsFighting;
}

void RandomBeast::Reset()
{
  fAcc = Vector3(0,0,0);
  fIsFeared = false;
  fIsFighting = false;
}


int RandomBeast::ReserveCoef(int ruleid, const char *name, real defaultValue)
{
  assert(ruleid!=RULEID_UNKNOWN);
  if(ruleid>=GetDNA()->Count()) 
  {
    int idx = GetDNA()->Count(); 
    for(; idx<=ruleid; idx++) 
    {
      GetDNA()->Add(new Array<Coef *>());
    }
    GetDNA()->Complete();
  }
  (*GetDNA())[ruleid]->Add(new Coef(name, defaultValue));
  (*GetDNA())[ruleid]->Complete();
  return (*GetDNA())[ruleid]->Count()-1;
}


real RandomBeast::GetCoef(int ruleid, int Index)
{
  return (*(*GetDNA())[ruleid])[Index]->Value();
}


real RandomBeast::GetCoefFromName(int ruleid, const char *name)
{
  int Count = (*GetDNA())[ruleid]->Count();
  int Index = 0;
  for(; Index<Count; Index++) 
  {
    if(strcmp(name, (*(*GetDNA())[ruleid])[Index]->Name())==0) 
      break;
  }
  //if Index==Count then no constant found!! return NAN
  assert(Index<Count);

  return (*(*GetDNA())[ruleid])[Index]->Value();
}


/// globalpercentage is a float value between 0.0 and 1.0
void RandomBeast::ModifyDNA(real globalpercentage)
{
  cout << "percentage = " << globalpercentage << endl;
  assert((globalpercentage>0.0)&&(globalpercentage<=1.0));
  /**
  * This routine is responsible for adding some salt to
  * the DNA of the current Beast. The <i>globalpercentage</i>
  * value determine the "ecart" of the computed DNA
  * compared to the initial DNA.
  *
  * Warning: the DNA is actually modified! so it may 
  * be necessary to clone the DNA before.
  *
  * TODO: Each Rule should also contain a percentage value
  * which indicates what percentage is to apply for every
  * coefficient of the given Rule ...
  */
  Array<Coef *> *cChromosome = NULL;
  Coef *cCoef = NULL;
  for(int idx=0; idx<GetDNA()->Count(); idx++)
  {
    cChromosome = (*GetDNA())[idx];
    for(int jdx=0; jdx<cChromosome->Count(); jdx++)
    {
      cCoef = (*cChromosome)[jdx];
      cCoef->ValueIs(cCoef->Value()
        *(frandomf((1-globalpercentage)*100))
        /100);
    }
  }
#ifndef NDEBUG
  showDNA();
#endif
}



/*****************************************************************************\
*  RandomBeastFactory
\*****************************************************************************/

RandomBeastFactory::RandomBeastFactory()
{
}

IBeast * RandomBeastFactory::NewBeast()
{
  return new RandomBeast();
}

IBeast * RandomBeastFactory::NewBeast(BeastRace race, real nrj)
{
  return new RandomBeast(race, nrj);
}

/*****************************************************************************\
*  RandomBeastArea
\*****************************************************************************/

RTTI_IMPL_4(RandomBeastArea, IDrawable, IDrawableMapped, IBeastArea, IBeastLandSink);

void RandomBeastArea::Init(BeastLand * beastland, Renderer * renderer)
{
  fRenderer = renderer;
  fBeastLand = beastland;
}

RandomBeastArea::RandomBeastArea(real size)
{
  fSize = size;
  fPosition[2] = -0.0;
  fDirection[0] = 1.0; // set a direction
}

Vector3 * RandomBeastArea::GetPosition()
{
  return &fPosition;
}

Vector3 * RandomBeastArea::GetDirection()
{
  return &fDirection;
}

byte RandomBeastArea::GetColor()
{
  return 80; 
}

DrawableShape RandomBeastArea::GetShape()
{
  return dsSquareMapped;
}

real RandomBeastArea::GetSize()
{
  return fSize;
}

int RandomBeastArea::GetGnokyCount()
{
  return fBeastLand->fGnokies.Count();
}

IGnoky * RandomBeastArea::GetGnoky(int Index)
{
  return fBeastLand->fGnokies[Index];
}

int RandomBeastArea::GetBeastCount()
{
  return fBeastLand->fBeasts.Count();
}

IBeast * RandomBeastArea::GetBeast(int Index)
{
  return fBeastLand->fBeasts[Index];
}

void RandomBeastArea::AddBeast(IBeast * beast)
{
  fRenderer->AddDrawable(dynamic_cast<IDrawable*>(beast));
}

void RandomBeastArea::DelBeast(IBeast * beast)
{
  fRenderer->DelDrawable(dynamic_cast<IDrawable*>(beast));
}

void RandomBeastArea::AddGnoky(IGnoky * gnoky)
{
  fRenderer->AddDrawable(dynamic_cast<IDrawable*>(gnoky));
}

void RandomBeastArea::DelGnoky(IGnoky * gnoky)
{
  fRenderer->DelDrawable(dynamic_cast<IDrawable*>(gnoky));
}

void RandomBeastArea::SetMappedImage(const Image * image)
{
  fMappedImage = image;
}

const Image * RandomBeastArea::GetMappedImage()
{
  return fMappedImage;
}

/*****************************************************************************\
*  RandomRule
\*****************************************************************************/

real Dist(Vector3 * a, Vector3 * b)
{
  return Vector3( *a - *b).Norme();
}

Vector3 & Ortho(Vector3 & vec)
{
  Swap(vec[0], vec[1]);
  vec[1] *= -1;
  return vec;
}

RandomRule::RandomRule(BeastRace apply)
{
  //fStartIndex = -1;
  //DBG::JYB
  //2ADD : initialization LIST/ARRAY
  fApplyOn = apply;
}

BeastRace RandomRule::ApplyOn()
{
  return fApplyOn;
}

void RandomRule::SetSink(IBeastLandSink * sink)
{
  fSink = sink;
}

IGnoky * RandomRule::GetClosestGnoky(IBeastArea * area, IBeast * beast)
{
  real dist = FLT_MAX;
  int Index=NOTFOUND;
  for (int i=0; i<area->GetGnokyCount(); i++)
    if (beast->GetGnokyDistance(i) < dist)
    {
      dist = beast->GetGnokyDistance(i);
      Index = i;
    }
    return Index==NOTFOUND?NULL:area->GetGnoky(Index);
}

IBeast * RandomRule::GetClosestBeast(IBeastArea * area, IBeast * beast)
{
  real dist = FLT_MAX;
  int Index=NOTFOUND;
  for (int i=0; i<area->GetBeastCount(); i++)
    if (beast->GetBeastDistance(i) < dist)
    {
      dist = beast->GetBeastDistance(i);
      Index = i;
    }
    return Index==NOTFOUND?NULL:area->GetBeast(Index);
}

IBeast * RandomRule::GetClosestBeast(IBeastArea * area, IBeast * beast, BeastRace race)
{
  real dist = FLT_MAX;
  int Index=NOTFOUND;
  for (int i=0; i<area->GetBeastCount(); i++)
    if ( area->GetBeast(i)->GetRace() != race )
      if ( (beast->GetBeastDistance(i) < dist) && (beast!=area->GetBeast(i)) )
      {
        dist = beast->GetBeastDistance(i);
        Index = i;
      }
      return Index==NOTFOUND?NULL:area->GetBeast(Index);
}

real RandomRule::GetCoef(IBeast *beast, int Index)
{
  return beast->GetCoef(Id(), Index);
}

/*****************************************************************************\
*  EatGnokyRule
\*****************************************************************************/

void EatGnokyRule::RegisterCoefs(IBeast * beast)
{
}

void EatGnokyRule::Apply(IBeast * beast, IBeastArea * area)
{
  for (int j=0; j<area->GetGnokyCount(); j++)
    if (beast->GetGnokyDistance(j) < 0.1)
    {
      beast->Eat(area->GetGnoky(j));
      area->GetGnoky(j)->Eated();
    }
}

/*****************************************************************************\
*  SearchGnokyRule
\*****************************************************************************/

void SearchGnokyRule::RegisterCoefs(IBeast * beast)
{
  beast->ReserveCoef(Id(), "weight", 1.0);     // Rule weight
  beast->ReserveCoef(Id(), "hunger", 10.0);      // Hunger NRJ threshold
  beast->ReserveCoef(Id(), "acceleration", 1.0); // Acceleration when Hungry
  beast->ReserveCoef(Id(), "threshold", 100.0);  // Threshold for not searcing gnoks
}

void SearchGnokyRule::Apply(IBeast * beast, IBeastArea * area)
{
  IGnoky * gnoky = GetClosestGnoky(area, beast);
  if ( gnoky==NULL )
  {
    Vector3 diff = *beast->GetPosition();
    diff.Normalize();
    beast->Accelerate(Ortho(diff), 1.0);
    return;
  }
  if ( (beast->GetNRJ() < GetCoef(beast, 3)) )
  {
    Vector3 diff = *gnoky->GetPosition() - (*beast->GetPosition() + *beast->GetSpeed()*TIMEEPSILON);
    diff.Normalize();
    if ( beast->GetNRJ() < GetCoef(beast, 1) )
      diff = diff*GetCoef(beast, 1);
    beast->Accelerate(diff, GetCoef(beast, 2));
    return;
  }
}

/*****************************************************************************\
*  DefendGnokyRule
\*****************************************************************************/

void DefendGnokyRule::RegisterCoefs(IBeast * beast)
{
  beast->ReserveCoef(Id(), "weight", 1.0); // rule weight
  beast->ReserveCoef(Id(), "defence", 50.0); // defend threshold
}

void DefendGnokyRule::Apply(IBeast * beast, IBeastArea * area)
{
  IGnoky * gnoky = GetClosestGnoky(area, beast);
  IBeast * other = GetClosestBeast(area, beast, beast->GetRace());
  if (other==NULL) return;
  if ( gnoky==NULL ) return;
  else if ( beast->GetNRJ() > other->GetNRJ() )
    if ( (beast->GetNRJ() > GetCoef(beast, 1)) ) // non homogene
    {
      // defend gnoky
      Vector3 middle = Vector3(*other->GetPosition()+*gnoky->GetPosition())*0.5;
      Vector3 diff = middle - *beast->GetPosition();
      diff.Normalize();
      beast->Accelerate(diff, GetCoef(beast, 0));
    }
}

/*****************************************************************************\
*  FearOtherRaceRule
\*****************************************************************************/

void FearOtherRaceRule::RegisterCoefs(IBeast * beast)
{
  beast->ReserveCoef(Id(), "weight", 1.0);
  beast->ReserveCoef(Id(), "hunger", 10.0);  // Hanger --> no fear threshold
  beast->ReserveCoef(Id(), "fear", 1.0);     // Fear distance
}

void FearOtherRaceRule::Apply(IBeast * beast, IBeastArea * area)
{
  if (beast->GetNRJ() < GetCoef(beast, 1)) return;
  for (int i=0; i<area->GetBeastCount(); i++)
    if (beast->GetRace() != area->GetBeast(i)->GetRace())
    {
      if (beast->GetBeastDistance(i) < GetCoef(beast, 2))
        if ( beast->GetNRJ() < area->GetBeast(i)->GetNRJ() )
        {
          Vector3 diff =
            *beast->GetPosition() - *area->GetBeast(i)->GetPosition();
          diff.Normalize();
          beast->Accelerate(diff, GetCoef(beast, 0));
          beast->IsFeared() = true;
        }
    }
}

/*****************************************************************************\
*  HortoFearOtherRaceRule
\*****************************************************************************/

void HortoFearOtherRaceRule::RegisterCoefs(IBeast * beast)
{
  beast->ReserveCoef(Id(), "weight", 1.0);
  beast->ReserveCoef(Id(), "hunger", 10.0); // Hanger --> no fear threshold
  beast->ReserveCoef(Id(), "fear", 1.0);    // Fear distance
}

void HortoFearOtherRaceRule::Apply(IBeast * beast, IBeastArea * area)
{
  if (beast->GetNRJ() < GetCoef(beast, 1)) return;
  for (int i=0; i<area->GetBeastCount(); i++)
    if (beast->GetRace() != area->GetBeast(i)->GetRace())
    {
      if (beast->GetBeastDistance(i) < GetCoef(beast, 2))
        if ( beast->GetNRJ() < area->GetBeast(i)->GetNRJ() )
        {
          Vector3 diff = 
            *beast->GetPosition() - *area->GetBeast(i)->GetPosition();
          diff.Normalize();
          beast->Accelerate(Ortho(diff), GetCoef(beast, 0));
          beast->IsFeared() = true;
        }
    }
}

/*****************************************************************************\
*  DontCollideRule
\*****************************************************************************/

void DontCollideRule::RegisterCoefs(IBeast * beast)
{
  beast->ReserveCoef(Id(), "weight", 1.0);   // Rule weight
  beast->ReserveCoef(Id(), "collision", 0.2);  // Collision distance
}

void DontCollideRule::Apply(IBeast * beast, IBeastArea * area)
{
  for (int i=0; i<area->GetBeastCount(); i++)
    if (beast->GetRace() == area->GetBeast(i)->GetRace())
      if (beast->GetBeastDistance(i) < GetCoef(beast, 1))
      {
        Vector3 diff = 
          *beast->GetPosition() - *area->GetBeast(i)->GetPosition();
        diff.Normalize();
        beast->Accelerate(diff, GetCoef(beast, 0));
      }
}

/*****************************************************************************\
*  FightOtherRaceRule
\*****************************************************************************/

void FightOtherRaceRule::RegisterCoefs(IBeast * beast)
{
}

void FightOtherRaceRule::Apply(IBeast * beast, IBeastArea * area)
{
  for (int i=0; i<area->GetBeastCount(); i++)
    if (beast->GetRace() != area->GetBeast(i)->GetRace())
      if (beast->GetBeastDistance(i) < 0.1)
      {
        //beast->LooseNRJ(area->GetBeast(i)->GetNRJ());
        //area->GetBeast(i)->LooseNRJ(beast->GetNRJ());
        beast->LooseNRJ(5.0);
        area->GetBeast(i)->LooseNRJ(5.0);
        beast->IsFighting() = true;
        area->GetBeast(i)->IsFighting() = true;
      }
}

/*****************************************************************************\
*  CloneWhenStrongRule
\*****************************************************************************/

CloneWhenStrongRule::CloneWhenStrongRule(IBeastFactory * factory, BeastRace race)
: RandomRule(race)
{
  fFactory = factory;
}

void CloneWhenStrongRule::RegisterCoefs(IBeast * beast)
{
}

void CloneWhenStrongRule::Apply(IBeast * beast, IBeastArea * area)
{
  if (beast->GetNRJ() > 100.0)
  {
    IBeast *newbeast =
      fFactory->NewBeast(beast->GetRace(), beast->GetNRJ());
    *newbeast->GetPosition() = *beast->GetPosition();
    (*newbeast->GetPosition())[0] += frand*0.5-0.25;
    (*newbeast->GetPosition())[1] += frand*0.5-0.25;

    fSink->AddBeast(newbeast);

    beast->LooseNRJ(4.0*beast->GetNRJ()/5.0);
    newbeast->LooseNRJ(4.0*beast->GetNRJ()/5.0);

    dynamic_cast<IDNA *>(newbeast)->ModifyDNA(0.25);
  }
}

/*****************************************************************************\
*  FindFriendRule
\*****************************************************************************/

void FindFriendRule::RegisterCoefs(IBeast * beast)
{
  beast->ReserveCoef(Id(), "weight", 1.0);       // Rule weight
  beast->ReserveCoef(Id(), "frienddistance", 2.0); // find friend distance
}

// find friend only if it's feared //JYB???<- comment is not right!!
void FindFriendRule::Apply(IBeast * beast, IBeastArea * area)
{
  for (int i=0; i<area->GetBeastCount(); i++)
    if (beast->GetRace() == area->GetBeast(i)->GetRace())
      if (beast->GetBeastDistance(i) < GetCoef(beast, 1))
      {
        Vector3 diff = 
          *area->GetBeast(i)->GetPosition() - *beast->GetPosition();
        diff.Normalize();
        beast->Accelerate(diff, GetCoef(beast, 0));
      }
}

/*****************************************************************************\
*  FindFriendWhenFearedRule
\*****************************************************************************/

void FindFriendWhenFearedRule::RegisterCoefs(IBeast * beast)
{
  beast->ReserveCoef(Id(), "weight", 1.0); // Rule weight
  beast->ReserveCoef(Id(), "frienddistance", 2.0); // find friend distance
}

// find friend only if it's feared
void FindFriendWhenFearedRule::Apply(IBeast * beast, IBeastArea * area)
{
  if (! beast->IsFeared() ) return;
  for (int i=0; i<area->GetBeastCount(); i++)
    if (beast->GetRace() == area->GetBeast(i)->GetRace())
      if (beast->GetBeastDistance(i) < GetCoef(beast, 1))
      {
        Vector3 diff = 
          *area->GetBeast(i)->GetPosition() - *beast->GetPosition();
        diff.Normalize();
        beast->Accelerate(diff, GetCoef(beast, 0));
      }
}


/*****************************************************************************\
*  *BarycenterRule
\*****************************************************************************/
// This rule states that a beast is attracted by the barycenter of its
// friendly neighboors and repulsed by its neighboring ennemies' barycenter.
// extension: we could add a coef for each beast : the leader always "weights"
// more than the others ...
void FriendsBarycenterRule::RegisterCoefs(IBeast * beast)
{
  beast->ReserveCoef(Id(), "weight", 0.2);
  beast->ReserveCoef(Id(), "visiondist", 4.0);
}


void FriendsBarycenterRule::Apply(IBeast * beast, IBeastArea * area)
{
  int cfriends = 0;
  Vector3 bFriends = *beast->GetPosition();

  for (int i=0; i<area->GetBeastCount(); i++)
    if (beast->GetBeastDistance(i) < GetCoef(beast, 1)) //visiondist
    {
      if(beast->GetRace() == area->GetBeast(i)->GetRace()) 
      {
        cfriends++;
        bFriends += *area->GetBeast(i)->GetPosition();
      } 
    }
    if(cfriends>0) {
      bFriends /= cfriends; //JYB: not really usefull since Normilization !!
      bFriends.Normalize();
      beast->Accelerate(bFriends, GetCoef(beast, 0));
    }
}


void EnnemiesBarycenterRule::RegisterCoefs(IBeast * beast)
{
  beast->ReserveCoef(Id(), "weight", 0.2);
  beast->ReserveCoef(Id(), "visiondist", 4.0);
}


void EnnemiesBarycenterRule::Apply(IBeast * beast, IBeastArea * area)
{
  int cEnnemies = 0;
  Vector3 bEnnemies = *beast->GetPosition();

  for (int i=0; i<area->GetBeastCount(); i++)
    if (beast->GetBeastDistance(i) < GetCoef(beast, 1)) //visiondist
    {
      if(beast->GetRace() != area->GetBeast(i)->GetRace()) 
      {
        cEnnemies++;
        bEnnemies -= *area->GetBeast(i)->GetPosition();
      }
    }
    if(cEnnemies>0) {
      bEnnemies /= cEnnemies; //JYB: not really usefull since Normilization !!
      bEnnemies.Normalize();
      beast->Accelerate(bEnnemies, GetCoef(beast, 0));
    }
}



