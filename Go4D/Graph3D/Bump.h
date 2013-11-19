/*****************************************************************************\
*AUTHOR     : Babel Yoann
*CREATION   : 28/07/96
*DATE       : 28/07/96
*TITLE      : Bump.h , bump mapping
*OBJECT     : face and facedrawer for bump mapping
\*****************************************************************************/

#ifndef _BUMP_H_
#define _BUMP_H_

#include "Face.h"
#include "FaceDrawer.h"
#include "Graph2D/Filter.h"

namespace Go4D
{

  using namespace Obsolete;

  /// Abstract drawer to display bump mapped faces !
  class ABumpMappedFaceDrawer : public AClutMappedFaceDrawer
  {
    typedef AClutMappedFaceDrawer inherited;
  protected :
    const FilteredImage * fBumpImage;

  public :
    ABumpMappedFaceDrawer(const Image &, int PointsCount, const Image &,
      const MultiShadeClut &, const FilteredImage &);
    ABumpMappedFaceDrawer(const Image &, const Image *, const MultiShadeClut &,
      const FilteredImage &);
    virtual void Draw(int PointsCount, PointZ * Points) const =0;
    virtual AFaceDrawer * Clone(int NewPointsCount) const =0;
  };

  /// Bump mapped faces drawer with linear mapping (faster)
  class BumpLinearMappedFaceDrawer : public ABumpMappedFaceDrawer
  {
    typedef ABumpMappedFaceDrawer inherited;
  private :
    void Horiz(realtype xb, realtype xe, realtype zb, realtype ze,
      int y, realtype mxb, realtype myb,
      realtype mxe, realtype mye, const Image * ima, Color shade) const;
  public :
    BumpLinearMappedFaceDrawer(const Image &, int PointsCount, const Image &,
      const MultiShadeClut &, const FilteredImage &);
    BumpLinearMappedFaceDrawer(const Image &, const Image *,
      const MultiShadeClut &, const FilteredImage &);
    virtual void Draw(int PointsCount, PointZ * Points) const;
    virtual AFaceDrawer * Clone(int NewPointsCount) const;
  };

  /// A face containing information necessary for bump mapping
  /// eg the mapped image and the "bump" height image
  class FlatBumpFace : public FlatFace
  {
    typedef FlatFace inherited;
  protected :
    FilteredImage * fBumpImage;
    real fCos, fSin;
    real fNorme;
    virtual void AddLight(const ALight &);
    virtual Face * SplitClone(int nb_points);
    virtual void BasicDraw();

  public :
    FlatBumpFace(AFaceDrawer *, Color colormin, Color nbcolors, int nbpoints,
      FilteredImage * aBumpImage, bool alwaysvisible=true);
    FlatBumpFace(AFaceDrawer *, APlot &, APlot &, APlot &, APlot &,
      Color colormin, Color nbcolors, FilteredImage * aBumpImage,
      bool alwayvisible=true);
  };

}

#endif
