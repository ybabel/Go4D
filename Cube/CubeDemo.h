#ifndef _DEMOCUBE_H_
#define _DEMOCUBE_H_

/*****************************************************************************\
* Sample application that draw cube
\*****************************************************************************/


#include "Go4D.h"

using namespace Go4D;

#define NCLUT 8
#define STEP (1/8.0)

enum CubeType { ctFlat, ctGouraud, ctLinMapped, ctQuadMapped, ctHypMapped,
ctLinShaded, ctLinBump, ctTransluent, ctLAST };

class MainApp : public ViewerApp
{
  typedef ViewerApp inherited;
private :
  const Image * fMappedImage;
  const MultiShadeClut * fMultiClut1;
  const MultiShadeClut * fMultiClut2;
  const TransluentClut * fTransluentClut;
  Image * fPhongImage;
  FilteredImage * fBumpImage;
  TransluentCube fTransluentCube;
  BumpLinearMappedCube fLinBumpCube;
  ShadedLinearMappedCube fLinShadeCube;
  LinearMappedCube fLinCube;
  QuadraticMappedCube fQuadCube;
  HypperbolicMappedCube fHypCube;
  FlatCube fFlatCube;
  GouraudCube fGourCube;
  CubeType fCubeType;
  void OnKeyPressed(int aKey);

public :
  MainApp(Device &, const Image * MappedImage,
    const MultiShadeClut * aMultiShadeClut1,
    const MultiShadeClut * aMultiShadeClut2,
    const TransluentClut * aTransluentClut,
    FilteredImage * BumpImage);
  void Draw();
  void Cycle();
  Palette * fDefaultPalette;
  Palette * fClutPalette;
};


int Start(char * cmdLine);

#endif
