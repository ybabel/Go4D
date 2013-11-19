#include "BeastLand.h"
#include "RTTI.h"
RTTI_USE;

BeastLand::BeastLand(IBeastArea * area)
//: fBeasts(true),
//fGnokies(true),
: fBeasts(false), //YBA2013
fGnokies(false), //YBA2013
fSinks(false)
{
  fArea = area;
}

BeastLand::~BeastLand()
{
}

void BeastLand::Clear()
{
  fBeasts.Clear();
  fGnokies.Clear();
  fBeasts.Complete();
  fGnokies.Complete();
}

void BeastLand::AddRule(IRule * rule)
{
  rule->SetSink(dynamic_cast<IBeastLandSink *>(this));
  fRules.Add(rule);
  fRules.Complete();
}


IBeast * BeastLand::CreateBeast(IBeastFactory * factory)
{
  IBeast * result;
  result = factory->NewBeast();
  AddBeast(result);
  return result;
}

IGnoky * BeastLand::CreateGnoky(IGnokyFactory * factory)
{
  IGnoky * result;
  result = factory->NewGnoky();
  AddGnoky(result);
  return result;
}

void BeastLand::AddSink(IBeastLandSink *sink)
{
  fSinks.Add(sink);
}

void BeastLand::ComputeNextPositions()
{
  for (int i=0; i<fBeasts.Count(); i++)
    fBeasts[i]->Reset();

  // Optimzed distance calculations
  for (int i=0; i<fBeasts.Count(); i++)
  {
    fBeasts[i]->BeginOptimization();
    for (int j=0; j<fGnokies.Count(); j++)
      fBeasts[i]->ComputeGnokyDistance(fGnokies[j]);
    for (int j=0; j<fBeasts.Count(); j++)
      fBeasts[i]->ComputeBeastDistance(fBeasts[j]);
    fBeasts[i]->EndOptimization();
  }

  // Apply rules
  for (int j=0; j<fRules.Count(); j++)
  {
    BeastRace race = fRules[j]->ApplyOn();
    if (race == brBoth)
      for (int i=0; i<fBeasts.Count(); i++)
        fRules[j]->Apply(fBeasts[i], fArea);
    else
      for (int i=0; i<fBeasts.Count(); i++)
        if (fBeasts[i]->GetRace()==race)
          fRules[j]->Apply(fBeasts[i], fArea);
  }

  // Make them move
  for (int i=0; i<fBeasts.Count(); i++)
    fBeasts[i]->Move(fArea);
  for (int i=0; i<fBeasts.Count(); i++)
    fBeasts[i]->InsureIsInArea(fArea);

  // Delete all that have NRJ <= 0
  fBeasts.First();
  while (fBeasts.Current() != NULL)
    if (fBeasts.Current()->GetNRJ()<=0.0)
      DelBeast(fBeasts.Current());
    else
      fBeasts.Next();
  fGnokies.First();
  while (fGnokies.Current() != NULL)
    if (fGnokies.Current()->GetNRJ()<=0.0)
      DelGnoky(fGnokies.Current());
    else
      fGnokies.Next();
}

void BeastLand::AddBeast(IBeast * beast)
{
  for (int j=0; j<fRules.Count(); j++)
    fRules[j]->RegisterCoefs(beast);
  fBeasts.Add(beast);
  for (int i=0; i<fSinks.Count(); i++)
    fSinks[i]->AddBeast(beast);
  fBeasts.Complete();
}

void BeastLand::DelBeast(IBeast * beast)
{
  for (int k=0; k<fSinks.Count(); k++)
    fSinks[k]->DelBeast(beast);
  fBeasts.Del(beast);
  fBeasts.Complete();
}

void BeastLand::AddGnoky(IGnoky * gnoky)
{
  fGnokies.Add(gnoky);
  for (int i=0; i<fSinks.Count(); i++)
    fSinks[i]->AddGnoky(gnoky);
  fGnokies.Complete();
}

void BeastLand::DelGnoky(IGnoky * gnoky)
{
  for (int k=0; k<fSinks.Count(); k++)
    fSinks[k]->DelGnoky(gnoky);
  fGnokies.Del(gnoky);
  fGnokies.Complete();
}


