/*****************************************************************************\
*AUTHOR     : Babel Yoann
*CREATED    : 06/08/96
*DATE       : 06/08/96
*TITLE      : cube.cpp, cube.h implementation
*BUGS       :
\*****************************************************************************/

#include "Cube.h"
#include "TemplateFaceDrawer.h"
#include "Plotter.h"
#include "Shader.h"
#include "Mapper.h"

namespace Go4D
{


  /*****************************************************************************\
  * Cube
  \*****************************************************************************/

  Cube::Cube(Image * aImage) : inherited(aImage, 8,6)
  {
    SetSize(1);
  }

  void Cube::FaceInit()
  {

    NewFace(0, 0, 1, 2, 3, Color(0),Color(255),false);
    NewFace(1, 0, 3, 7, 4, Color(0),Color(255),false);
    NewFace(2, 0, 4, 5, 1, Color(0),Color(255),false);
    NewFace(3, 4, 7, 6, 5, Color(0),Color(255),false);
    NewFace(4, 1, 5, 6, 2, Color(0),Color(255),false);
    NewFace(5, 2, 6, 7, 3, Color(0),Color(255),false);
    /*
    // No visible face determination : be sure you use an algorithm to determine
    // which face is visible of*r not, like ZBuffer
    NewFace(0, 0, 1, 2, 3, byte(0),byte(255),true);
    NewFace(1, 0, 3, 7, 4, byte(0),byte(255),true);
    NewFace(2, 0, 4, 5, 1, byte(0),byte(255),true);
    NewFace(3, 4, 7, 6, 5, byte(0),byte(255),true);
    NewFace(4, 1, 5, 6, 2, byte(0),byte(255),true);
    NewFace(5, 2, 6, 7, 3, byte(0),byte(255),true);
    */
  }

  void Cube::SetSize(real dim)
  {
    plots[0] = APlot(-dim,-dim, dim);
    plots[1] = APlot(-dim, dim, dim);
    plots[2] = APlot( dim, dim, dim);
    plots[3] = APlot( dim,-dim, dim);
    plots[4] = APlot(-dim,-dim,-dim);
    plots[5] = APlot(-dim, dim,-dim);
    plots[6] = APlot( dim, dim,-dim);
    plots[7] = APlot( dim,-dim,-dim);
  }

  /*****************************************************************************\
  * FlatCube
  \*****************************************************************************/

  FlatCube::FlatCube(Image * aImage)
    : inherited(aImage)
  {
  }

  void FlatCube::NewFace(int num_face, int p1, int p2, int p3, int p4,
    Color color, Color nbcolor, bool av)
  {
    assert(num_face>=0 && num_face<nfaces);
    faces[num_face] =
      new FlatFace(new Obsolete::FlatFaceDrawer(*fImage, 4), dupplots[p1],
      dupplots[p2], dupplots[p3], dupplots[p4], color, nbcolor, av);
  }

  /*****************************************************************************\
  * GouraudCube
  \*****************************************************************************/

  GouraudCube::GouraudCube(Image * aImage)
    : inherited(aImage)
  {
  }

  void GouraudCube::NewFace(int num_face, int p1, int p2, int p3, int p4,
    Color color, Color nbcolor, bool av)
  {
    assert(num_face>=0 && num_face<nfaces);
    faces[num_face] = new GouraudFace(new Obsolete::GouraudFaceDrawer(*fImage, 4), dupplots[p1],
      dupplots[p2], dupplots[p3],	dupplots[p4], color, nbcolor, av);
  }

  /*****************************************************************************\
  * LinearMappedCube
  \*****************************************************************************/

  LinearMappedCube::LinearMappedCube(Image * aImage, const Image * aMappedImage)
    : inherited(aImage)
  {
    fMappedImage = aMappedImage;
  }

  void LinearMappedCube::NewFace(int num_face, int p1, int p2, int p3, int p4,
    Color color, Color nbcolor, bool av)
  {
    assert(num_face>=0 && num_face<nfaces);
    //faces[num_face] = new FlatFace(
    //  new MappedFaceDrawer<ZImagePlotter, NullShader, HypperbolicMapper>(*fImage, DrawerImageData(fMappedImage)),
    //  dupplots[p1], dupplots[p2], dupplots[p3], dupplots[p4], color, nbcolor, av);
    faces[num_face] = new EnvMapFace(
      new MappedFaceDrawer<ImagePlotter, NullShader, HypperbolicMapper>(*fImage, new DrawerImageData(fMappedImage)),
      dupplots[p1], dupplots[p2], dupplots[p3], dupplots[p4], color, nbcolor, av);
  }

  /*****************************************************************************\
  * QuadraticMappedCube
  \*****************************************************************************/

  QuadraticMappedCube::QuadraticMappedCube(Image * aImage, const Image * aMappedImage)
    : inherited(aImage)
  {
    fMappedImage = aMappedImage;
  }

  void QuadraticMappedCube::NewFace(int num_face, int p1, int p2, int p3, int p4,
    Color color, Color nbcolor, bool av)
  {
    assert(num_face>=0 && num_face<nfaces);
    faces[num_face] = new FlatFace(
      new MappedFaceDrawer<ImagePlotter, NullShader, QuadraticMapper>(*fImage, new DrawerImageData(fMappedImage)),
      dupplots[p1], dupplots[p2], dupplots[p3], dupplots[p4], color, nbcolor, av);
  }

  /*****************************************************************************\
  * HypperbolicMappedCube
  \*****************************************************************************/

  HypperbolicMappedCube::HypperbolicMappedCube(Image * aImage, const Image * aMappedImage)
    : inherited(aImage)
  {
    fMappedImage = aMappedImage;
  }

  void HypperbolicMappedCube::NewFace(int num_face, int p1, int p2, int p3, int p4,
    Color color, Color nbcolor, bool av)
  {
    assert(num_face>=0 && num_face<nfaces);
    faces[num_face] = new FlatFace(
      new MappedFaceDrawer<ImagePlotter, NullShader, HypperbolicMapper>(*fImage, new DrawerImageData(fMappedImage)),
      dupplots[p1], dupplots[p2], dupplots[p3], dupplots[p4], color, nbcolor, av);
  }

  /*****************************************************************************\
  * ShadedLinearMappedCube
  \*****************************************************************************/

  ShadedLinearMappedCube::ShadedLinearMappedCube(Image * aImage,
    const Image * aMappedImage, const MultiShadeClut* aMultiClut)
    : inherited(aImage, aMappedImage)
  {
    fMultiClut = aMultiClut;
  }

  void ShadedLinearMappedCube::NewFace(int num_face, int p1, int p2, int p3, int p4,
    Color color, Color nbcolor, bool av)
  {
    assert(num_face>=0 && num_face<nfaces);
    faces[num_face] = new FlatFace(
      new MappedFaceDrawer<ImagePlotter, MultiClutShader, LinearMapper>(*fImage, new DrawerImageClutData(fMappedImage, fMultiClut)),
      dupplots[p1], dupplots[p2], dupplots[p3], dupplots[p4], color, nbcolor, av);
  }

  /*****************************************************************************\
  * BumpLinearMappedCube
  \*****************************************************************************/

  BumpLinearMappedCube::BumpLinearMappedCube(Image * aImage,
    const Image * aMappedImage, const MultiShadeClut* aMultiClut,
    FilteredImage * aBumpImage)
    : inherited(aImage, aMappedImage, aMultiClut)
  {
    fBumpImage = aBumpImage;
  }

  void BumpLinearMappedCube::NewFace(int num_face, int p1, int p2, int p3, int p4,
    Color color, Color nbcolor, bool av)
  {
    assert(num_face>=0 && num_face<nfaces);
    faces[num_face] =
      new FlatBumpFace(
      new MappedFaceDrawer<ImagePlotter, MultiClutImageShader, LinearMapper>(*fImage, new DrawerBumpData(fMappedImage, fMultiClut, fBumpImage)),
      dupplots[p1], dupplots[p2], dupplots[p3], dupplots[p4], color, nbcolor, fBumpImage, av);
  }

  /*****************************************************************************\
  * TransluentCube
  \*****************************************************************************/

  TransluentCube::TransluentCube(Image * aImage, const Image * aMappedImage,
    const TransluentClut * aTransluentClut)
    : inherited(aImage, aMappedImage)
  {
    fTransluentClut = aTransluentClut;
  }

  void TransluentCube::NewFace(int num_face, int p1, int p2, int p3, int p4,
    Color color, Color nbcolor, bool )
  {
    assert(num_face>=0 && num_face<nfaces);
    if (num_face!=1 && num_face!=4)
      faces[num_face] = new FlatFace(
      new Obsolete::LinearMappedFaceDrawer(*fImage, fMappedImage),
      dupplots[p1], dupplots[p2], dupplots[p3], dupplots[p4], color, nbcolor, true);
    else
      faces[num_face] = new FlatFace(
      (Obsolete::AFlatFaceDrawer*)(new Obsolete::TransluentFaceDrawer(*fImage, 4, *fTransluentClut)),
      dupplots[p1], dupplots[p2], dupplots[p3], dupplots[p4], color, nbcolor, true);
  }


}
