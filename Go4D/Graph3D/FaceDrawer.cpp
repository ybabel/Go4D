/*****************************************************************************\
*AUTHOR : Babel yoann
*TITLE  : PluggedDrawer.cpp, PluggedDrawer.h implementation
*CREATED: 13/07/96
*DATE   : 13/07/96
*BUGS   :
\*****************************************************************************/

#include "FaceDrawer.h"

namespace Go4D
{

    /*****************************************************************************\
    * AFaceDrawer
    \*****************************************************************************/

    AFaceDrawer::AFaceDrawer(const Image & aImage, int aCount)
      : inherited(aImage)
    {
      fCount = aCount;
      fImage = & aImage;
      xmin = 0;
      ymin = 0;
      xmax = fImage->Width();
      ymax = fImage->Height();
    }

  namespace Obsolete
  {
    /*****************************************************************************\
    * AFlatFaceDrawer
    \*****************************************************************************/

    AFlatFaceDrawer::AFlatFaceDrawer(const Image & aImage, int aCount)
      : inherited(aImage, aCount)
    {
    }

    void AFlatFaceDrawer::SplitExtrem(AFaceDrawer *, int, int, int, int,
      real, real)
    {
      // nothign to do
    }

    /*****************************************************************************\
    * FlatFaceDrawer
    \*****************************************************************************/

    FlatFaceDrawer::FlatFaceDrawer(const Image & aImage, int aCount)
      : inherited(aImage, aCount)
    {
    }

    AFaceDrawer * FlatFaceDrawer::Clone(int aCount) const
    {
      return new FlatFaceDrawer(*fImage, aCount);
    }

    void FlatFaceDrawer::Draw(int npoints, PointZ * points) const
    {
      Color color = points[0].color;
#define FILL_CALL_HORIZ Horiz(xl, xr, zl, zr, ry,color)
#include "Fill.cpp"
    }

    void FlatFaceDrawer::Horiz(realtype xb, realtype xe, realtype zb, realtype ze, int y, Color color) const
    {
#define HORIZ_PUT_PIXEL *Cur = color
#include "Horiz.cpp"
    }

    /*****************************************************************************\
    * GouraudFaceDrawer
    \*****************************************************************************/

    GouraudFaceDrawer::GouraudFaceDrawer(const Image & aImage, int aCount)
      : inherited(aImage, aCount)
    {
    }

    AFaceDrawer * GouraudFaceDrawer::Clone(int NewPointsCount) const
    {
      return new GouraudFaceDrawer(*fImage, NewPointsCount);
    }

    void GouraudFaceDrawer::Draw(int npoints, PointZ * points) const
    {
#define FILL_LOCALS \
  realtype cl=conv(points[0].color); \
  realtype cr=conv(points[0].color); \
  realtype icl=0, icr=0
#define FILL_CALL_HORIZ Horiz(xl, xr, zl, zr, ry, cl, cr)
#define FILL_NEXT_LINE cl+=icl; cr+=icr
#define FILL_LEFT_SAME_COEF cl = conv(points[left].color)
#define FILL_LEFT_NOT_SAME_LOCALS realtype deltac = conv(points[left].color) - cl
#define FILL_LEFT_NOT_SAME_COEF icl = frapport(deltac, deltay)
#define FILL_RIGHT_SAME_COEF cr = conv(points[right].color)
#define FILL_RIGHT_NOT_SAME_LOCALS realtype deltac = conv(points[right].color) - cr
#define FILL_RIGHT_NOT_SAME_COEF icr = frapport(deltac, deltay)
#include "Fill.cpp"
    }

    void GouraudFaceDrawer::Horiz(realtype xb, realtype xe, realtype zb, realtype ze, int y, int cb, int ce) const
    {
#define HORIZ_LOCALS realtype c, ic
#define HORIZ_SWAP SwapR(cb, ce)
#define HORIZ_COEF_INIT ic = frapport( (ce-cb), (xe-xb) ); c = cb
#define HORIZ_CLIP c += ic*(xmin-rxb)
#define HORIZ_PUT_PIXEL *Cur = Color(round(c))
#define HORIZ_NEXT_PIXEL c += ic
#include "Horiz.cpp"
    }

    /*****************************************************************************\
    * AMappedFaceDrawer
    \*****************************************************************************/

    AMappedFaceDrawer::AMappedFaceDrawer(const Image & aImage, int aPointsCount,
      const Image & aMappedImage)
      : inherited(aImage, aPointsCount)
    {
      fMappedImage = &aMappedImage;
      alloc(mpoints = new MapPoint[fCount]);
      alloc(dupmpoints = new MapPoint[fCount]);
    }

#define MARGIN 1
    AMappedFaceDrawer::AMappedFaceDrawer(const Image & aImage,
      const Image * aMappedImage)
      : inherited(aImage, 4)
    {
      fMappedImage = aMappedImage;
      alloc(mpoints = new MapPoint[fCount]);
      alloc(dupmpoints = new MapPoint[fCount]);
      if (fMappedImage != NULL)
      {
        SetMapPoint(0, MARGIN, MARGIN);
        SetMapPoint(1, fMappedImage->Width()-MARGIN-1, MARGIN);
        SetMapPoint(2, fMappedImage->Width()-MARGIN-1, fMappedImage->Height()-MARGIN-1);
        SetMapPoint(3, 1, fMappedImage->Height()-MARGIN-1);
      }
    }

    AMappedFaceDrawer::~AMappedFaceDrawer()
    {
      delete [] mpoints;
      delete [] dupmpoints;
    }


    void AMappedFaceDrawer::SetMapImage(const Image * mapImage)
    {
      if (fMappedImage == NULL)
      {
        SetMapPoint(0, MARGIN, MARGIN);
        SetMapPoint(1, mapImage->Width()-MARGIN-1, MARGIN);
        SetMapPoint(2, mapImage->Width()-MARGIN-1, mapImage->Height()-MARGIN-1);
        SetMapPoint(3, 1, mapImage->Height()-MARGIN-1);
      }
      fMappedImage = mapImage;
    }

#define NEXT(i) ((i)==aParent->Count()-1?0:(i)+1)
#define PREV(i) ((i)==0?aParent->Count()-1:(i)-1)
#define PARENTMPOINT ((AMappedFaceDrawer *)aParent)->mpoints

    void AMappedFaceDrawer::SplitExtrem(AFaceDrawer * aParent, int FirstPositive,
      int FirstNegative,  int PositiveCount, int,
      real FirstCoord, real SecondCoord)
    {
      int ir=FirstPositive;
      for (int q=0; q<PositiveCount; q++, ir=NEXT(ir))
        mpoints[q] = PARENTMPOINT[ir];

      mpoints[PositiveCount].x = (int)(
        (PARENTMPOINT[PREV(FirstNegative)].x-PARENTMPOINT[FirstNegative].x)*
        SecondCoord+PARENTMPOINT[FirstNegative].x);
      mpoints[PositiveCount].y = (int)(
        (PARENTMPOINT[PREV(FirstNegative)].y-PARENTMPOINT[FirstNegative].y)*
        SecondCoord+PARENTMPOINT[FirstNegative].y);

      mpoints[PositiveCount+1].x = (int)(
        (PARENTMPOINT[PREV(FirstPositive)].x-PARENTMPOINT[FirstPositive].x)*
        FirstCoord+PARENTMPOINT[FirstPositive].x);
      mpoints[PositiveCount+1].y = (int)(
        (PARENTMPOINT[PREV(FirstPositive)].y-PARENTMPOINT[FirstPositive].y)*
        FirstCoord+PARENTMPOINT[FirstPositive].y);
    }

    void AMappedFaceDrawer::SetMapPoint(int aPointIndice, int aMapX, int aMapY)
    {
      mpoints[aPointIndice].x = aMapX;
      mpoints[aPointIndice].y = aMapY;
    }

    /*****************************************************************************\
    * LinearMappedFaceDrawer
    \*****************************************************************************/

    LinearMappedFaceDrawer::LinearMappedFaceDrawer(const Image & aImage,
      int aPointsCount, const Image & aMappedImage)
      : inherited(aImage, aPointsCount, aMappedImage)
    {
    }

    LinearMappedFaceDrawer::LinearMappedFaceDrawer(const Image & aImage,
      const Image * aMappedImage)
      : inherited(aImage, aMappedImage)
    {
    }

    void LinearMappedFaceDrawer::Draw(int npoints, PointZ * points) const
    {
      const Image * ima = fMappedImage;
      MapPoint * mappoints = dupmpoints;
      for (int i=0; i<fCount; i++)
        dupmpoints[i] = mpoints[i];

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
#define FILL_CALL_HORIZ Horiz(xl, xr, zl, zr, ry, mxl, myl, mxr, myr, ima)
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

    void LinearMappedFaceDrawer::Horiz(realtype xb, realtype xe,
      realtype zb, realtype ze,
      int y, realtype mxb, realtype myb,
      realtype mxe, realtype mye, const Image * ima) const

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

#define HORIZ_LOCALS_CLIPPED Color * Ima = ima->GetBuffer()

#define HORIZ_PUT_PIXEL \
  assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())>=0); \
  assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())<ima->Len()); \
  *Cur = *(Ima+ round(mx)+ bl*round(my))

#define HORIZ_NEXT_PIXEL  \
  mx += imx; \
  my += imy

#include "Horiz.cpp"
    }

    AFaceDrawer * LinearMappedFaceDrawer::Clone(int NewPointsCount) const
    {
      return new  LinearMappedFaceDrawer(*fImage, NewPointsCount, *fMappedImage);
    }

    /*****************************************************************************\
    * QuadraticMappedFaceDrawer
    \*****************************************************************************/

    QuadraticMappedFaceDrawer::QuadraticMappedFaceDrawer(const Image & aImage,
      int aPointsCount, const Image & aMappedImage)
      : inherited(aImage, aPointsCount, aMappedImage)
    {
    }

    QuadraticMappedFaceDrawer::QuadraticMappedFaceDrawer(const Image & aImage,
      const Image * aMappedImage)
      : inherited(aImage, aMappedImage)
    {
    }

    void facedrawer_QuadMapCoefs(realtype xb,realtype xe,realtype l,
      real & fa, real & fb, real & fc)
    {
      real fxb  = fround(xb);
      real fxe  = fround(xe);
      real fl   = fround(l);
      real fd;
      if ( fl > 1 )
      {
        fd = fl*(fxe-fxb)*(fxe-fxb);
        fa = fl-1;
        fb = fxb*(1-2*fl) + fxe;
        fc = fxb*(fxb*fl-fxe);
      }
      else
      {
        fd = (fxe-fxb)*(fxe-fxb);
        fa = fl-1;
        fb = -fxb*fl+fxe*(2-fl);
        fc = fxb*(fxb+fxe*(fl-2));
      }
      if (fabs(fd) < zero) fd = 1;
      fa = fa/fd;
      fb = fb/fd;
      fc = fc/fd;
    }

    void facedrawer_QuadMapInit(real fa, real fb, real fc, realtype xd,
      realtype mxb, realtype mxe,
      realtype myb, realtype mye,
      realtype &mx, realtype &my,
      realtype &tx, realtype &ty,
      realtype &ax, realtype &ay)
    {
      real fxd  = fround(xd);
      real fmxb = fround(mxb);
      real fmyb = fround(myb);
      real fmxe = fround(mxe);
      real fmye = fround(mye);
      real fk = (fxd*(fa*fxd+fb)+fc);
      mx= fconv( fk*(fmxe-fmxb)+ fmxb );
      my= fconv( fk*(fmye-fmyb)+ fmyb );
      tx= fconv( ((fa+fa)*fxd+fa+fb)*(fmxe-fmxb) );
      ax= fconv( (fa+fa)*(fmxe-fmxb) );
      ty= fconv( ((fa+fa)*fxd+fa+fb)*(fmye-fmyb) );
      ay= fconv( (fa+fa)*(fmye-fmyb) );
    }

    void facedrawer_QuadMapCoefsInit(realtype xb, realtype xe, realtype l,
      realtype xd,
      realtype mxb, realtype mxe,
      realtype myb, realtype mye,
      realtype &mx, realtype &my,
      realtype &tx, realtype &ty,
      realtype &ax, realtype &ay)
    {
      real fa, fb, fc;
      facedrawer_QuadMapCoefs(xb,xe,l, fa, fb, fc);
      facedrawer_QuadMapInit(fa, fb, fc, xd, mxb, mxe, myb, mye, mx, my, tx, ty, ax, ay);
    }

    void QuadraticMappedFaceDrawer::Draw(int npoints, PointZ * points) const
    {
      const Image * ima = fMappedImage;
      MapPoint * mappoints = dupmpoints;
      for (int i=0; i<fCount; i++)
        dupmpoints[i] = mpoints[i];
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
  realtype ll=0, lr=0; \
  realtype mxlb=mxl, mxle=mxl, mylb=myl, myle=myl; \
  realtype mxrb=mxr, mxre=mxr, myrb=myr, myre=myr; \
  realtype zlb=zl, zle=zl, zrb, zre=zr; \
  int rylb=ry, ryle=ry, ryrb, ryre=ry; \
  realtype txl=0, axl=0, tyl=0, ayl=0; \
  realtype txr=0, axr=0, tyr=0, ayr=0
#define FILL_CALL_HORIZ Horiz(xl, xr, zl, zr, ry, mxl, myl, mxr, myr, ima)
#define FILL_NEXT_LINE \
  mxl += txl;  mxr += txr; \
  txl += axl;  txr += axr; \
  myl += tyl;  myr += tyr; \
  tyl += ayl;  tyr += ayr
#define FILL_LEFT_CHANGING \
  zlb = zle; \
  mxlb=mxle; \
  mylb=myle; \
  rylb=ryle
#define FILL_LEFT_CHANGED \
  zle =fconv(points[left].z); \
  mxle=conv(mappoints[left].x); \
  myle=conv(mappoints[left].y); \
  ryle=points[left].y
#define FILL_LEFT_SAME_COEF \
  mxl = conv(mappoints[left].x); \
  myl = conv(mappoints[left].y); \
  ll = frapport(zle,zlb)
#define FILL_LEFT_NOT_SAME_COEF \
  ll = frapport(zle,zlb); \
  facedrawer_QuadMapCoefsInit(conv(rylb), conv(ryle), ll, conv(rylb), \
  mxlb, mxle, mylb, myle, \
  mxl, myl, txl, tyl, axl, ayl)
#define FILL_RIGHT_CHANGING \
  zrb =zre; \
  mxrb=mxre; \
  myrb=myre; \
  ryrb=ryre
#define FILL_RIGHT_CHANGED \
  zre =fconv(points[right].z); \
  mxre=conv(mappoints[right].x); \
  myre=conv(mappoints[right].y); \
  ryre=points[right].y
#define FILL_RIGHT_SAME_COEF \
  mxr = conv(mappoints[right].x); \
  myr = conv(mappoints[right].y); \
  lr = frapport(zre,zrb)
#define FILL_RIGHT_NOT_SAME_COEF \
  lr = frapport(zre,zrb); \
  facedrawer_QuadMapCoefsInit(conv(ryrb), conv(ryre), lr, conv(ryrb), \
  mxrb, mxre, myrb, myre, \
  mxr, myr, txr, tyr, axr, ayr)
#include "Fill.cpp"
    }

    void QuadraticMappedFaceDrawer::Horiz(realtype xb, realtype xe,
      realtype zb, realtype ze,
      int y, realtype mxb, realtype myb,
      realtype mxe, realtype mye, const Image * ima) const

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
#define HORIZ_LOCALS   realtype l, mx, my, tx, ty, ax, ay
#define HORIZ_SWAP  SwapR(mxb, mxe); SwapR(myb, mye)
#define HORIZ_COEF_INIT \
  assert(zb!=0); \
  assert(ze!=0); \
  assert( (zb>0&&ze>0)||(zb<0&&ze<0) ); \
  l = frapport(ze,zb)
#define HORIZ_LOCALS_CLIPPED \
  Color * Ima = ima->GetBuffer(); \
  facedrawer_QuadMapCoefsInit(xb, xe, l, conv(xd+1), mxb, mxe, myb, mye, \
  mx, my, tx, ty, ax, ay)
#define HORIZ_PUT_PIXEL \
  assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())>=0); \
  assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())<ima->Len()); \
  *Cur = *(Ima+ round(mx)+ bl*round(my))
#define HORIZ_NEXT_PIXEL  \
  mx += tx; \
  tx += ax; \
  my += ty; \
  ty += ay
#include "Horiz.cpp"

    }

    AFaceDrawer * QuadraticMappedFaceDrawer::Clone(int NewPointsCount) const
    {
      return new  QuadraticMappedFaceDrawer(*fImage, NewPointsCount, *fMappedImage);
    }

    /*****************************************************************************\
    * HypperbolicMappedFaceDrawer
    \*****************************************************************************/

    HypperbolicMappedFaceDrawer::HypperbolicMappedFaceDrawer(const Image & aImage,
      int aPointsCount, const Image & aMappedImage)
      : inherited(aImage, aPointsCount, aMappedImage)
    {
    }

    HypperbolicMappedFaceDrawer::HypperbolicMappedFaceDrawer(const Image & aImage,
      const Image * aMappedImage)
      : inherited(aImage, aMappedImage)
    {
    }

    void HypperbolicMappedFaceDrawer::Draw(int npoints, PointZ * points) const
    {
#define HYPINC(u,s,x,xb,xe,y,yb,ye,zb,ze)\
  u+=s*(zb*(ye-y)+ze*(y-yb));\
  while (u>0)\
      {\
      y+=s;\
      u+=zb*(xb-x)+ze*(x-xe);\
      }

      const Image * ima = fMappedImage;
      MapPoint * mappoints = dupmpoints;
      for (int i=0; i<fCount; i++)
      {
        dupmpoints[i] = mpoints[i];
        points[i].z = points[i].z*1024; // precision problem
      }
#define FILL_POINTS_ROT \
  MapPoint mtmp = mappoints[0]; \
  for (int mi=0; mi<npoints-1; mi++) \
  mappoints[mi] = mappoints[mi+1]; \
  mappoints[npoints-1] = mtmp
#define FILL_LOCALS \
  int rmxl=mappoints[0].x; \
  int rmxr=mappoints[0].x; \
  int rmyl=mappoints[0].y; \
  int rmyr=mappoints[0].y; \
  int rzl=round(zl); \
  int rzr=round(zr); \
  int rmxlb=rmxl, rmxle=rmxl; \
  int rmylb=rmyl, rmyle=rmyl; \
  int rmxrb=rmxr, rmxre=rmxr; \
  int rmyrb=rmyr, rmyre=rmyr; \
  int rylb=ry,    ryle=ry; \
  int ryrb=ry,    ryre=ry; \
  int rzlb=rzl,   rzle=rzl; \
  int rzrb=rzr,   rzre=rzr; \
  int umxl=0, umxr=0, umyl=0, umyr=0, uzl=0, uzr=0; \
  int smxl=0, smxr=0, smyl=0, smyr=0, szl=0, szr=0
#define FILL_CALL_HORIZ \
  Horiz(xl, xr, conv(rzl), conv(rzr), ry, \
  conv(rmxl), conv(rmyl), conv(rmxr), conv(rmyr), ima)
#define FILL_NEXT_LINE \
  HYPINC(umxl,smxl,ry,rylb,ryle,rmxl,rmxlb,rmxle,rzlb,rzle); \
  HYPINC(umyl,smyl,ry,rylb,ryle,rmyl,rmylb,rmyle,rzlb,rzle); \
  HYPINC(umxr,smxr,ry,ryrb,ryre,rmxr,rmxrb,rmxre,rzrb,rzre); \
  HYPINC(umyr,smyr,ry,ryrb,ryre,rmyr,rmyrb,rmyre,rzrb,rzre); \
  HYPINC(uzl, szl, ry,rylb,ryle, rzl,rzlb, rzle, rzlb,rzle); \
  HYPINC(uzr, szr, ry,ryrb,ryre, rzr,rzrb, rzre, rzrb,rzre)
#define FILL_LEFT_CHANGING \
  rzlb =rzle; \
  rmxlb=rmxle; \
  rmylb=rmyle; \
  rylb =ryle
#define FILL_LEFT_CHANGED \
  rzle =int(points[left].z); \
  rmxle=mappoints[left].x; \
  rmyle=mappoints[left].y; \
  ryle =points[left].y; \
  smxl=sign(rmxle-rmxlb); \
  smyl=sign(rmyle-rmylb); \
  szl =sign(rzle-rzlb); \
  umxl=0; umyl=0; uzl =0
#define FILL_LEFT_SAME_COEF \
  rmxl = mappoints[left].x; \
  rmyl = mappoints[left].y
#define FILL_RIGHT_CHANGING \
  rzrb =rzre; \
  rmxrb=rmxre; \
  rmyrb=rmyre; \
  ryrb =ryre
#define FILL_RIGHT_CHANGED \
  rzre =int(points[right].z); \
  rmxre=mappoints[right].x; \
  rmyre=mappoints[right].y; \
  ryre =points[right].y; \
  smxr=sign(rmxre-rmxrb); \
  smyr=sign(rmyre-rmyrb); \
  szr =sign(rzre-rzrb); \
  umxr=0; umyr=0; uzr =0
#define FILL_RIGHT_SAME_COEF \
  rmxr = mappoints[right].x; \
  rmyr = mappoints[right].y
#include "Fill.cpp"
    }

    void HypperbolicMappedFaceDrawer::Horiz(realtype xb, realtype xe,
      realtype zb, realtype ze,
      int y, realtype mxb, realtype myb,
      realtype mxe, realtype mye, const Image * ima) const

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
#define HORIZ_SWAP  SwapR(mxb, mxe); SwapR(myb, mye)

#define HORIZ_LOCALS_CLIPPED \
  Color * Ima = ima->GetBuffer(); \
  int ux = 0, uy = 0; \
  int smx=sign(mxe-mxb), smy=sign(mye-myb); \
  int rmxe=round(mxe)-smx, rmye=round(mye)-smy; \
  int rmxb=round(mxb), rmyb=round(myb); \
  int rze=round(ze), rzb=round(zb); \
  int rden=rzb*(rxb-(xd+1))+rze*((xd+1)-rxe); \
  int rmx=(rzb*rmxe*(rxb-(xd+1))+rze*rmxb*((xd+1)-rxe))/rden; \
  int rmy=(rzb*rmye*(rxb-(xd+1))+rze*rmyb*((xd+1)-rxe))/rden; \
  int p=rzb*(rxb-(xd+1))+rze*((xd+1)-rxe); \
  int ip=rze-rzb; \
  int qx=smx*(rzb*(rmxe-rmx)+rze*(rmx-rmxb)); \
  int iqx=(rze-rzb); \
  int qy=smy*(rzb*(rmye-rmy)+rze*(rmy-rmyb)); \
  int iqy=(rze-rzb)
#define HORIZ_PUT_PIXEL \
  *Cur = *(Ima+ rmx+ bl*rmy)

      //assert(int(Ima+ rmx+ bl*rmy-ima->GetBuffer())>=0);

      //assert(int(Ima+ rmx+ bl*rmy-ima->GetBuffer())<ima->Len());
#define HORIZ_NEXT_PIXEL  \
  ux += qx; \
  while (ux>0) \
      { \
      rmx+=smx; \
      qx +=iqx; \
      ux += p; \
      } \
      uy += qy; \
      while (uy>0) \
      { \
      rmy+=smy; \
      qy +=iqy; \
      uy += p; \
      } \
      p += ip
#include "Horiz.cpp"
    }

    AFaceDrawer * HypperbolicMappedFaceDrawer::Clone(int NewPointsCount) const
    {
      return new  HypperbolicMappedFaceDrawer(*fImage, NewPointsCount, *fMappedImage);
    }

    /*****************************************************************************\
    * AClutMappedFaceDrawer
    \*****************************************************************************/

    AClutMappedFaceDrawer::AClutMappedFaceDrawer(const Image &aImage, int aPointsCount,
      const Image & aMappedImage, const MultiClut & aMultiClut)
      : inherited(aImage, aPointsCount, aMappedImage)
    {
      fMultiClut = &aMultiClut;
    }

    AClutMappedFaceDrawer::AClutMappedFaceDrawer(const Image &aImage,
      const Image * aMappedImage, const MultiClut & aMultiClut)
      : inherited(aImage, aMappedImage)
    {
      fMultiClut = &aMultiClut;
    }

    /*****************************************************************************\
    * AShadedMappedFaceDrawer
    \*****************************************************************************/

    AShadedMappedFaceDrawer::AShadedMappedFaceDrawer(const Image &aImage,
      int aPointsCount, const Image &aMappedImage, const MultiShadeClut & aMultiClut)
      : inherited(aImage, aPointsCount, aMappedImage, aMultiClut)
    {
    }

    AShadedMappedFaceDrawer::AShadedMappedFaceDrawer(const Image &aImage,
      const Image *aMappedImage, const MultiShadeClut & aMultiClut)
      : inherited(aImage, aMappedImage, aMultiClut)
    {
    }

    /*****************************************************************************\
    * ShadedLinearMappedFaceDrawer
    \*****************************************************************************/

    ShadedLinearMappedFaceDrawer::ShadedLinearMappedFaceDrawer(const Image & aImage,
      int aPointsCount, const Image &aMappedImage, const MultiShadeClut & aMultiClut)
      : inherited(aImage, aPointsCount, aMappedImage, aMultiClut)
    {
    }

    ShadedLinearMappedFaceDrawer::ShadedLinearMappedFaceDrawer(const Image & aImage,
      const Image * aMappedImage, const MultiShadeClut & aMultiClut)
      : inherited(aImage, aMappedImage, aMultiClut)
    {
    }

    void ShadedLinearMappedFaceDrawer::Draw(int npoints, PointZ * points) const
    {
      const Image * ima = fMappedImage;
      MapPoint * mappoints = dupmpoints;
      Color shade = points[0].color;
      for (int i=0; i<fCount; i++)
        dupmpoints[i] = mpoints[i];

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

    void ShadedLinearMappedFaceDrawer::Horiz(realtype xb, realtype xe,
      realtype zb, realtype ze,
      int y, realtype mxb, realtype myb,
      realtype mxe, realtype mye, const Image * ima, Color shade) const

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
#define HORIZ_LOCALS_CLIPPED Color * Ima = ima->GetBuffer()
#define HORIZ_PUT_PIXEL \
  assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())>=0); \
  assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())<ima->Len()); \
  *Cur = (*fMultiClut)(shade, *(Ima+ round(mx)+ bl*round(my)));
#define HORIZ_NEXT_PIXEL  \
  mx += imx; \
  my += imy
#include "Horiz.cpp"
    }

    AFaceDrawer * ShadedLinearMappedFaceDrawer::Clone(int NewPointsCount) const
    {
      return new  ShadedLinearMappedFaceDrawer(*fImage, NewPointsCount, *fMappedImage, *(MultiShadeClut *)fMultiClut);
    }

    /*****************************************************************************\
    * TransluentFlatFaceDrawer
    \*****************************************************************************/

    TransluentFaceDrawer::TransluentFaceDrawer(const Image & aImage, int aCount,
      const TransluentClut & aTransluentClut)
      : inherited(aImage, aCount)
    {
      fTransluentClut = &aTransluentClut;
    }

    AFaceDrawer * TransluentFaceDrawer::Clone(int aCount) const
    {
      return new TransluentFaceDrawer(*fImage, aCount, *fTransluentClut);
    }

    void TransluentFaceDrawer::Draw(int npoints, PointZ * points) const
    {
      Color color = points[0].color;
#define FILL_CALL_HORIZ Horiz(xl, xr, zl, zr, ry,color)
#include "Fill.cpp"
    }

    void TransluentFaceDrawer::Horiz(realtype xb, realtype xe, realtype zb, realtype ze, int y, Color) const
    {
#define HORIZ_PUT_PIXEL *Cur = (*fTransluentClut)[*Cur]
#include "Horiz.cpp"
    }

  }

}
