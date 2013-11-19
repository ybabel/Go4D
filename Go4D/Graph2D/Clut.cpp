/**
*AUTHOR : Babel yoann
*TITLE  : Clut.cpp, Clut.h implementation
*CREATED: 07/07/96
*DATE   : 07/07/96
*BUGS   :
*/

#include "Clut.h"

namespace Go4D
{

  /*****************************************************************************\
  * ColorLookUpTable
  \*****************************************************************************/

  ColorLookUpTable::ColorLookUpTable(int aCount)
  {
    assert(aCount > 0);
    fCount = aCount;
    alloc(fValues = new Color[fCount]);
  }

  ColorLookUpTable::ColorLookUpTable(const ColorLookUpTable & aClut)
  {
    fCount = aClut.fCount;
    fCount = aClut.fCount;
    alloc(fValues = new Color[fCount]);
  }

  ColorLookUpTable::~ColorLookUpTable()
  {
    delete [] fValues;
  }

#ifdef DEBUGOUT
  ostream & ColorLookUpTable::DebugOut(ostream & os) const
  {
    os << "Clut" << incendl
      << "{" << iendl;
    for (int i=0; i<fCount; i++)
      os << int(fValues[i]) << " ";
    os << iendl
      << "}" << decendl;
    return os;
  }
#endif

  Color & ColorLookUpTable::operator [] (int i) const
  {
    assert(i>=0);
    assert(i<fCount);
    return fValues[i];
  }

#ifdef DEBUGOUT
  ostream & operator << (ostream & os, const ColorLookUpTable & aClut)
  {
    return aClut.DebugOut(os);
  }
#endif

  /*****************************************************************************\
  * ShadeClut
  \*****************************************************************************/

  // !!!! this is adapted for 256 color palettes
  ShadeClut::ShadeClut(const Palette & aPalette, real aPercent)
    : inherited(aPalette.Count())
  {
    for (int i=0; i<fCount; i++)
      fValues[i] = Color(aPalette.Best(aPalette[i]*aPercent));
  }

  /*****************************************************************************\
  * MultiClut
  \*****************************************************************************/

  MultiClut::MultiClut(const Palette & aPalette, int aClutsCount)
    : inherited(aPalette.Count()*aClutsCount)
  {
    fClutsCount = aClutsCount;
    pPalette = &aPalette;
    fClutValuesCount = pPalette->Count();
    for (int i=0; i<fCount; i++)
      fValues[i] = 0;
  }

  Color & MultiClut::operator () (int aClutNum, int aClutValue) const
  {
    assert(aClutNum>=0);
    assert(aClutNum<fClutsCount);
    assert(aClutValue>=0);
    assert(aClutValue<fClutValuesCount);
    return fValues[aClutNum*fClutValuesCount+aClutValue];
  }

  /*****************************************************************************\
  * MultiShadeClut
  \*****************************************************************************/

  MultiShadeClut::MultiShadeClut(const Palette &aPalette, int aClutsCount)
    : inherited(aPalette, aClutsCount)
  { }

  void MultiShadeClut::SetClut(int aClutNum, real aPercent)
  {
    for (int i=0; i<fClutValuesCount; i++)
      fValues[aClutNum*fClutValuesCount+i] =
      Color(pPalette->Best((*pPalette)[i]*aPercent));
  }

  /*****************************************************************************\
  * ClutDrawer
  \*****************************************************************************/

  TransluentClut::TransluentClut(const Palette & aPalette)
    : inherited(aPalette.Count())
  {
    pPalette = &aPalette;
  }

  TransluentClut & TransluentClut::Init(const RGBColor & RGBColor)
  {
    for (int i=0; i<fCount; i++)
      fValues[i] = Color(pPalette->Best((*pPalette)[i]^RGBColor));
    fValues[0] = Color(pPalette->Best(RGBColor));
    return *this;
  }

  TransluentClut & TransluentClut::Init(const Palette & aPalette)
  {
    for (int i=0; i<fCount; i++)
      fValues[i] = Color(pPalette->Best((*pPalette)[i]^aPalette[i]));
    fValues[0] = Color(pPalette->Best(aPalette[0]));
    return *this;
  }

  /*****************************************************************************\
  * ClutDrawer
  \*****************************************************************************/

  ClutDrawer::ClutDrawer(int aWidth, int aHeight, bool AutoAlloc)
    : inherited(aWidth, aHeight, AutoAlloc)
  {
  }

}
