/*****************************************************************************\
*AUTHOR     : Babel Yoann
*CREATED    : 06/08/96
*DATE       : 06/08/96
*TITLE      : cube.h , 3d predefined cube (for demo and debug)
*OBJECT     : map, shad, bump, transluent cubes...
\*****************************************************************************/

#ifndef _CUBE_H_
#define _CUBE_H_

#include "Object.h"
#include "Bump.h"

namespace Go4D
{

  /**
  * Cube : just sample cube to debug the facedrawing
  */
  class Cube : public Object
  {
    typedef Object inherited;
  protected :
    virtual void NewFace(int num_face, int p1, int p2, int p3, int p4,
      Color color, Color nbcolor, bool av)=0;

  public :
    Cube(Image * aImage);
    void FaceInit();
    virtual void SetSize(real newsize);
  };

  /// A cube with flat drawed faces
  class FlatCube : public Cube
  {
    typedef Cube inherited;
  protected :
    virtual void NewFace(int num_face, int p1, int p2, int p3, int p4,
      Color color, Color nbcolor, bool av);

  public :
    FlatCube(Image * aImage);
  };

  /// a cube with gouraud shaded drawed faces
  class GouraudCube : public Cube
  {
    typedef Cube inherited;
  protected :
    virtual void NewFace(int num_face, int p1, int p2, int p3, int p4,
      Color color, Color nbcolor, bool av);

  public :
    GouraudCube(Image * aImage);
  };

  /// a cube with a linear mapped image drawed in each face
  class LinearMappedCube : public Cube
  {
    typedef Cube inherited;
  protected :
    const Image * fMappedImage;
    virtual void NewFace(int num_face, int p1, int p2, int p3, int p4,
      Color color, Color nbcolor, bool av);

  public :
    LinearMappedCube(Image * aImage, const Image * aMappedImage);
  };

  /// a cube with an image mapped quadratically for each face
  class QuadraticMappedCube : public Cube
  {
    typedef Cube inherited;
  protected :
    const Image * fMappedImage;
    virtual void NewFace(int num_face, int p1, int p2, int p3, int p4,
      Color color, Color nbcolor, bool av);

  public :
    QuadraticMappedCube(Image * aImage, const Image * aMappedImage);
  };

  /// a cube with image mapping, perspective correct
  class HypperbolicMappedCube : public Cube
  {
    typedef Cube inherited;
  protected :
    const Image * fMappedImage;
    virtual void NewFace(int num_face, int p1, int p2, int p3, int p4,
      Color color, Color nbcolor, bool av);

  public :
    HypperbolicMappedCube(Image * aImage, const Image * aMappedImage);
  };

  /// A cube with mapped image + linear shading
  class ShadedLinearMappedCube : public LinearMappedCube
  {
    typedef LinearMappedCube inherited;
  protected :
    const MultiShadeClut * fMultiClut;
    virtual void NewFace(int num_face, int p1, int p2, int p3, int p4,
      Color color, Color nbcolor, bool av);

  public :
    ShadedLinearMappedCube(Image * aImage, const Image * aMappedImage, const MultiShadeClut * aMultiClut);
  };

  /// a cube with bump mapped image on it
  class BumpLinearMappedCube : public ShadedLinearMappedCube
  {
    typedef ShadedLinearMappedCube inherited;
  protected :
    FilteredImage * fBumpImage;
    virtual void NewFace(int num_face, int p1, int p2, int p3, int p4,
      Color color, Color nbcolor, bool av);

  public :
    BumpLinearMappedCube(Image * aImage, const Image * aMappedImage,
      const MultiShadeClut * aMultiClut, FilteredImage * aBumpImage);
  };

  /// only 2 faces are transluent
  /// draw method : draw all the faces in z-order, no backface-culling.
  class TransluentCube : public LinearMappedCube
  {
    typedef LinearMappedCube inherited;
  protected :
    const TransluentClut * fTransluentClut;
    virtual void NewFace(int num_face, int p1, int p2, int p3, int p4,
      Color color, Color nbcolor, bool av);

  public :
    TransluentCube(Image * aImage, const Image * aMappedImage,
      const TransluentClut * aTransluentClut);
  };

}

#endif
