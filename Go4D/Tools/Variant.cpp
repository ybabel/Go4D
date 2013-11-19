#include "Variant.h"

/******************************************************************************\
* Variant
\******************************************************************************/

namespace Go4D
{


  Variant::Variant()
  {
    fType = vtUnk;
  }

  Variant::Variant(int i)
  {
    fType=vtInt;
    fValue.fInteger=i;
  }

  Variant::Variant(real r)
  {
    fType=vtReal;
    fValue.fReal=r;
  }

  Variant::Variant(const char * s)
  {
    fType=vtString;
    CopyStr(fValue.fString, s);
  }

  char * Variant::ToString()
  {
    switch (fType)
    {
    case vtInt : Int2Str(fValue.fInteger, fValue.fString); break;
    case vtString : break;
    case vtReal : Real2Str(fValue.fReal, fValue.fString); break;
    case vtUnk : assert(false); break;
    }
    return fValue.fString;
  }

  void Variant::FromString(const char * s)
  {
    switch (fType)
    {
    case vtInt :  Str2Int(fValue.fInteger, s); break;
    case vtString : CopyStr(fValue.fString, s); break;
    case vtReal : Str2Real(fValue.fReal, s); break;
    case vtUnk : assert(false); break;
    }
  }

  real Variant::ToReal()
  {
    switch (fType)
    {
    case vtInt :  fValue.fReal = real(fValue.fInteger); break;
    case vtString : Str2Real(fValue.fReal, fValue.fString); break;
    case vtReal : break;
    case vtUnk : assert(false); break;
    }
    return fValue.fReal;
  }

  int Variant::ToInt()
  {
    switch (fType)
    {
    case vtInt : break;
    case vtString : Str2Int(fValue.fInteger, fValue.fString); break;
    case vtReal : fValue.fInteger = int(fValue.fReal); break;
    case vtUnk : assert(false); break;
    }
    return fValue.fInteger;
  }


}
