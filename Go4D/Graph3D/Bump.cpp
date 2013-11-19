/*****************************************************************************\
*AUTHOR     : Babel Yoann
*CREATION   : 28/07/96,
*DATE       : 28/07/96
*TITLE      : Bump.cpp, Bump.h implementation
*BUGS       :
\*****************************************************************************/

#include "Bump.h"

namespace Go4D
{


  /*****************************************************************************\
  * ABumpMappedFaceDrawer
  \*****************************************************************************/

  ABumpMappedFaceDrawer::ABumpMappedFaceDrawer(const Image &aImage,
    int aPointsCount, const Image &aMappedImage,
    const MultiShadeClut & aMultiClut, const FilteredImage & aBumpImage)
    : inherited(aImage, aPointsCount, aMappedImage, aMultiClut)
  {
    assert(aBumpImage.Width() == aMappedImage.Width());
    assert(aBumpImage.Height() == aMappedImage.Height());
    fBumpImage = &aBumpImage;
  }

  ABumpMappedFaceDrawer::ABumpMappedFaceDrawer(const Image &aImage,
    const Image *aMappedImage, const MultiShadeClut & aMultiClut,
    const FilteredImage & aBumpImage)
    : inherited(aImage, aMappedImage, aMultiClut)
  {
    if (aMappedImage != NULL)
    {
      assert(aBumpImage.Width() == aMappedImage->Width());
      assert(aBumpImage.Height() == aMappedImage->Height());
    }
    fBumpImage = &aBumpImage;
  }

  /*****************************************************************************\
  * BumpLinearMappedFaceDrawer
  \*****************************************************************************/

  BumpLinearMappedFaceDrawer::BumpLinearMappedFaceDrawer(const Image & aImage,
    int aPointsCount, const Image &aMappedImage,
    const MultiShadeClut & aMultiClut, const FilteredImage & aBumpImage)
    : inherited(aImage, aPointsCount, aMappedImage, aMultiClut, aBumpImage)
  {
  }

  BumpLinearMappedFaceDrawer::BumpLinearMappedFaceDrawer(const Image & aImage,
    const Image * aMappedImage, const MultiShadeClut & aMultiClut,
    const FilteredImage & aBumpMap)
    : inherited(aImage, aMappedImage, aMultiClut, aBumpMap)
  {
  }

  void BumpLinearMappedFaceDrawer::Draw(int npoints, PointZ * points) const
  {
    const Image * ima = fMappedImage;
    MapPoint * mappoints = dupmpoints;
    for (int i=0; i<fCount; i++)
      dupmpoints[i] = mpoints[i];
    Color shade = points[0].color;

#define FILL_POINTS_ROT \
  MapPoint mtmp = mappoints[0]; \
  for (int mi=0; mi<npoints-1; mi++) \
  mappoints[mi] = mappoints[mi+1]; \
  mappoints[npoints-1] = mtmp
#define FILL_LOCALS \
  realtype mxl=conv(mappoints[0].x); \
  realtype mxr=conv(mappoints[0].x); \
  realtype myl=conv(mappoints[0].y); \
  realtype myr=conv(mappoints[0].y); \
  realtype imxl=0, imxr=0, imyl=0, imyr=0
#define FILL_CALL_HORIZ Horiz(xl, xr, zl, zr, ry, mxl, myl, mxr, myr, ima, shade)
#define FILL_NEXT_LINE \
  mxl+=imxl; myl+=imyl; \
  mxr+=imxr; myr+=imyr
#define FILL_LEFT_SAME_COEF \
  mxl = conv(mappoints[left].x); \
  myl = conv(mappoints[left].y)
#define FILL_LEFT_NOT_SAME_LOCALS \
  realtype deltamx = conv(mappoints[left].x) - mxl; \
  realtype deltamy = conv(mappoints[left].y) - myl
#define FILL_LEFT_NOT_SAME_COEF \
  imxl = frapport(deltamx, deltay); \
  imyl = frapport(deltamy, deltay)
#define FILL_RIGHT_SAME_COEF \
  mxr = conv(mappoints[right].x); \
  myr = conv(mappoints[right].y)
#define FILL_RIGHT_NOT_SAME_LOCALS \
  realtype deltamx = conv(mappoints[right].x) - mxr; \
  realtype deltamy = conv(mappoints[right].y) - myr
#define FILL_RIGHT_NOT_SAME_COEF \
  imxr = frapport(deltamx, deltay); \
  imyr = frapport(deltamy, deltay)
#include "Fill.cpp"
  }

  void BumpLinearMappedFaceDrawer::Horiz(realtype xb, realtype xe,
    realtype zb, realtype ze,
    int y, realtype mxb, realtype myb,
    realtype mxe, realtype mye, const Image * ima, Color) const
  {
#define HORIZ_ASSERTIONS \
  assert(ima != NULL);\
  assert(round(mxb)>=-1 && round(mxb)<ima->Width()); \
  assert(round(mxe)>=-1 && round(mxe)<ima->Width()); \
  assert(round(myb)>=-1 && round(myb)<ima->Height()); \
  assert(round(mye)>=-1 && round(mye)<ima->Height()); \
  assert(round(mxb)+ima->Width()*round(myb)<ima->Len()); \
  assert(round(mxe)+ima->Width()*round(mye)<ima->Len()); \
  int bl=ima->Width()
#define HORIZ_LOCALS realtype mx, imx, my, imy
#define HORIZ_SWAP  SwapR(mxb, mxe); SwapR(myb, mye)
#define HORIZ_COEF_INIT \
  imx = frapport( (mxe-mxb), (xe-xb+1) ); \
  mx = mxb; \
  imy = frapport( (mye-myb), (xe-xb+1) ); \
  my = myb
#define HORIZ_CLIP \
  mx += imx*(xmin-rxb); \
  my += imy*(xmin-rxb)
#define HORIZ_LOCALS_CLIPPED \
  Color * Ima = ima->GetBuffer(); \
  Color * BumpIma = fBumpImage->GetBuffer();
#define HORIZ_PUT_PIXEL \
  assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())>=0); \
  assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())<ima->Len()); \
  assert(*(BumpIma+ round(mx)+bl*round(my))>=0); \
  assert(*(BumpIma+ round(mx)+bl*round(my))<fMultiClut->ClutsCount()); \
  *Cur = (*fMultiClut)(*(BumpIma+ round(mx)+bl*round(my)), \
  *(Ima+ round(mx)+ bl*round(my)) );
#define HORIZ_NEXT_PIXEL  \
  mx += imx; \
  my += imy
#include "Horiz.cpp"
  }

  AFaceDrawer * BumpLinearMappedFaceDrawer::Clone(int NewPointsCount) const
  {
    return new  BumpLinearMappedFaceDrawer(*fImage, NewPointsCount, *fMappedImage, *(MultiShadeClut *)fMultiClut, *fBumpImage);
  }

  /*****************************************************************************\
  * FlatBumpFace
  \*****************************************************************************/

  FlatBumpFace::FlatBumpFace(AFaceDrawer * aFaceDrawer, Color colormin, Color nbcolors, int nbpoints,
    FilteredImage * aBumpImage, bool alwaysvisible)
    : inherited(aFaceDrawer, colormin, nbcolors, nbpoints, alwaysvisible)
  {
    fBumpImage = aBumpImage;
  }

  FlatBumpFace::FlatBumpFace(AFaceDrawer *aFaceDrawer, APlot &aPlot0, APlot &aPlot1,
    APlot &aPlot2, APlot &aPlot3,
    Color colormin, Color nbcolors, FilteredImage * aBumpImage,
    bool alwayvisible)
    : inherited(aFaceDrawer,aPlot0, aPlot1, aPlot2, aPlot3, colormin, nbcolors, alwayvisible)
  {
    fBumpImage = aBumpImage;
  }

  //work for only one light.
  void FlatBumpFace::AddLight(const ALight & aLight)
  {
    inherited::AddLight(aLight);
    Vector3 xaxe = *edges[1] - *edges[0];
    Vector3 yaxe = *edges[1] - *edges[2];
    xaxe.Normalize();
    yaxe.Normalize();
    Vector3 dir = aLight.Direction(fCenter);
    dir.Normalize();
    fCos = xaxe*dir;
    fSin = yaxe*dir;
  }

  void FlatBumpFace::BasicDraw()
  {
    fCenter.ProcessColor(colormin, nbcolors);
    for (int i=0; i<npoints; i++)
    {
      edges[i]->GetColor() = fCenter.GetColor();
      points[i] = edges[i]->GetPointZ();
    }
#define ABS(x) ((x<0)?-(x):(x))
    fNorme = ABS(fCos)+ABS(fSin);
    if (fNorme < 0.01)
    {
      fBumpImage->Bias() = real(nbcolors);
      fBumpImage->Factor() = 0;
    }
    else
    {
      fBumpImage->Bias() = real(nbcolors);
      fBumpImage->Factor() = 2*real(nbcolors-1);
      for (int i=0; i<9; i++)
        fBumpImage->Filter()[i] = 0;
#define SIGN(x) ((x)<0?-1:1)
      fBumpImage->Filter()(1-SIGN(fCos),1) = -ABS(fCos)/fNorme;
      fBumpImage->Filter()(1,1+SIGN(fSin)) = -ABS(fSin)/fNorme;
      fBumpImage->Filter()(1+SIGN(fCos),1) = ABS(fCos)/fNorme;
      fBumpImage->Filter()(1,1-SIGN(fSin)) = ABS(fSin)/fNorme;
    }
    fBumpImage->Emboss();
    fFaceDrawer->Draw(npoints, points);
  }

  Face * FlatBumpFace::SplitClone(int newnbpoints)
  {
    return new FlatBumpFace(fFaceDrawer->Clone(newnbpoints), colormin, nbcolors,
      newnbpoints, fBumpImage, alwaysvisible);
  }

}
