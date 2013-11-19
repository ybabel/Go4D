#ifndef MapperH
#define MapperH

/*
*AUTHOR : Babel yoann
*TITLE  : Mapper.h, various mapping algorithms
*CREATED: 19/05/99
*DATE   : 19/05/99
*OBJECT : In this unit are defined some mapping algorithms. The vary from
*         the quality of the rendering and the time it's consuming.
*         LinearMapper is the fastest, but the worst method.
*         HypperbolicMapper is the slower, but there are no deformations in
*         the mapped images.
*/

#include "TemplateFaceDrawer.h"

namespace Go4D
{


  /**
  *NULLMAPPER
  *  This class is the super-class for all classes that want to implement a
  *  mapping algo (like linear mapping, quadratic, and hypperbolic). Some methods
  *  that are common to all the mapping classes are predefined.
  */
  // just a base class for all mappers
  class NullMapper : public Filler
  {
    typedef Filler inherited;
  protected :
  public :
    /// 
    NullMapper() {}
    /// 
    void InitMapPoints(MapPoint * mpoints)
    {
    }
    /// 
    void Init(const Image * image, const FaceDrawerData * data)
    {
      inherited::Init(image, data);
    }
  };

  /**
  *MAPPER
  *  This class is the super-class for all classes that want to implement a
  *  mapping algo (like linear mapping, quadratic, and hypperbolic). Some methods
  *  that are common to all the mapping classes are predefined.
  */
  // just a base class for all mappers
  class Mapper : public Filler
  {
    typedef Filler inherited;
  protected :
    mutable MapPoint * mappoints;
    mutable const Image * ima;
    mutable realtype mxb, myb, mxe, mye;
    mutable int bl;
    mutable Color * Ima;

  public :
    /// 
    Mapper() {}
    /// 
    void InitMapPoints(MapPoint * mpoints)
    {
      mappoints = mpoints;
    }
    /// 
    void Init(const Image * image, const FaceDrawerData * data)
    {
      inherited::Init(image, data);
      if (data==NULL) return;
      ima = static_cast<const DrawerImageData *>(data)->fMappedImage;
    }
    /// 
    inline void FillPointsRot(int npoints) const
    {
      inherited::FillPointsRot(npoints);
      MapPoint mtmp = mappoints[0];
      for (int mi=0; mi<npoints-1; mi++)
        mappoints[mi] = mappoints[mi+1];
      mappoints[npoints-1] = mtmp;
    }
    /// 
    inline void HorizAssertions() const
    {
      inherited::HorizAssertions();
      assert(ima != NULL);
      assert(round(mxb)>=-1 && round(mxb)<ima->Width());
      assert(round(mxe)>=-1 && round(mxe)<ima->Width());
      assert(round(myb)>=-1 && round(myb)<ima->Height());
      assert(round(mye)>=-1 && round(mye)<ima->Height());
      assert(round(mxb)+ima->Width()*round(myb)<ima->Len());
      assert(round(mxe)+ima->Width()*round(mye)<ima->Len());
      bl=ima->Width();
    }
    /// 
    inline void HorizSwap() const
    {
      inherited::HorizSwap();
      SwapR(mxb, mxe); SwapR(myb, mye);
    }
    inline void HorizLocalsClipped(realtype xb, realtype xe, int xd, int xf, realtype zb, realtype ze, int rxb, int rxe) const
    {
      inherited::HorizLocalsClipped(xb, xe, xd, xf, zb, ze, rxb, rxe);
      Ima = ima->GetBuffer();
    }
  };

  /**
  *LINEARMAPPER
  *  Linear mapping algo. This algo is the faster one, but it don't gives the
  *  best result. You can use it when MaxZ/MinZ is near 1 ( 0.7<MaxZ/MinZ<1.5 )
  *  This values must be tested. MaxZ is the Z max of all the Z of the polygon's
  *  3D points. When the face is far from the observer used this algo (the
  *  deformation is not visible any more).
  *  The class's fields are static, wich is not anoying, and much faster.
  */
  //static is faster than mutable (static = global->easier access)
  class LinearMapper : public Mapper
  {
    typedef Mapper inherited;
  private :
    // fill locals
    mutable realtype mxl;
    mutable realtype mxr;
    mutable realtype myl;
    mutable realtype myr;
    mutable realtype imxl, imxr, imyl, imyr;
    mutable realtype deltamx;
    mutable realtype deltamy;
    // horiz locals
    mutable realtype mx, imx, my, imy;
  public :
    /// 
    LinearMapper() {}
    /// 
    inline void FillInitLocals(realtype zl, realtype zr, int ry) const
    {
      inherited::FillInitLocals(zl, zr, ry);
      mxl=conv(mappoints[0].x);
      mxr=conv(mappoints[0].x);
      myl=conv(mappoints[0].y);
      myr=conv(mappoints[0].y);
      imxl=0; imxr=0; imyl=0; imyr=0;
    }
    /// 
    inline void FillNextLine(int ry) const
    {
      inherited::FillNextLine(ry);
      mxl+=imxl; myl+=imyl;
      mxr+=imxr; myr+=imyr;
    }
    /// 
    inline void FillLeftSameCoef(int left) const
    {
      inherited::FillLeftSameCoef(left);
      mxl = conv(mappoints[left].x);
      myl = conv(mappoints[left].y);
    }
    /// 
    inline void FillLeftNotSameInitLocals(int left) const
    {
      inherited::FillLeftNotSameInitLocals(left);
      deltamx = conv(mappoints[left].x) - mxl;
      deltamy = conv(mappoints[left].y) - myl;
    }
    /// 
    inline void FillLeftNotSameCoef(realtype deltay) const
    {
      inherited::FillLeftNotSameCoef(deltay);
      imxl = frapport(deltamx, deltay);
      imyl = frapport(deltamy, deltay);
    }
    /// 
    inline void FillRightSameCoef(int right) const
    {
      inherited::FillRightSameCoef(right);
      mxr = conv(mappoints[right].x);
      myr = conv(mappoints[right].y);
    }
    /// 
    inline void FillRightNotSameInitLocals(int right) const
    {
      inherited::FillRightNotSameInitLocals(right);
      deltamx = conv(mappoints[right].x) - mxr;
      deltamy = conv(mappoints[right].y) - myr;
    }
    /// 
    inline void FillRightNotSameCoef(realtype deltay) const
    {
      inherited::FillRightNotSameCoef(deltay);
      imxr = frapport(deltamx, deltay);
      imyr = frapport(deltamy, deltay);
    }
    inline void FillInitHoriz(realtype xl, realtype xr, realtype zl, realtype zr, int ry) const
    {
      inherited::FillInitHoriz(xl, xr, zl, zr, ry);
      mxb = mxl; myb = myl; mxe = mxr; mye = myr;
    }
    /// 
    inline void HorizCoefInit(realtype xb, realtype xe, realtype zb, realtype ze) const
    {
      inherited::HorizCoefInit(xb, xe, zb, ze);
      imx = frapport( (mxe-mxb), (xe-xb) );
      mx = mxb;
      imy = frapport( (mye-myb), (xe-xb) );
      my = myb;
    }
    /// 
    inline void HorizClip(int xmin, int rxb) const
    {
      inherited::HorizClip(xmin, rxb);
      mx += imx*(xmin-rxb);
      my += imy*(xmin-rxb);
    }
    /// 
    inline int HorizGetOffs() const { return int(round(mx)+ bl*round(my)); }
    inline Color HorizGetPixel(int offs) const
    {
      assert(int(Ima+ offs-ima->GetBuffer())>=0);
      assert(int(Ima+ offs-ima->GetBuffer())<ima->Len());
      return *(Ima+ offs);
    }
    /// 
    inline void HorizNextPixel() const
    {
      inherited::HorizNextPixel();
      mx += imx;
      my += imy;
    }
  };

  void mapper_QuadMapCoefs(realtype xb,realtype xe,realtype l,
    real & fa, real & fb, real & fc);
  void mapper_QuadMapInit(real fa, real fb, real fc, realtype xd,
    realtype mxb, realtype mxe,
    realtype myb, realtype mye,
    realtype &mx, realtype &my,
    realtype &tx, realtype &ty,
    realtype &ax, realtype &ay);
  void mapper_QuadMapCoefsInit(realtype xb, realtype xe, realtype l,
    realtype xd,
    realtype mxb, realtype mxe,
    realtype myb, realtype mye,
    realtype &mx, realtype &my,
    realtype &tx, realtype &ty,
    realtype &ax, realtype &ay);

  /**
  *QUADRATICMAPPER
  *  This mapping algo is slower, but gives some better result (visualy talking).
  *  You can used it when faces are closer (but not when a part of the face is 
  *  behind the observer). 
  */
  class QuadraticMapper : public Mapper
  {
    typedef Mapper inherited;
  private :
    // fill locals
    mutable realtype mxl;
    mutable realtype mxr;
    mutable realtype myl;
    mutable realtype myr;
    mutable realtype ll, lr;
    mutable realtype mxlb, mxle, mylb, myle;
    mutable realtype mxrb, mxre, myrb, myre;
    mutable realtype zlb, zle, zrb, zre;
    mutable int rylb, ryle, ryrb, ryre;
    mutable realtype txl, axl, tyl, ayl;
    mutable realtype txr, axr, tyr, ayr;
    //horiz locals
    mutable realtype l, mx, my, tx, ty, ax, ay;
  public :
    /// 
    QuadraticMapper() { }
    /// 
    inline void FillInitLocals(realtype zl, realtype zr, int ry) const
    {
      inherited::FillInitLocals(zl, zr, ry);
      mxl=conv(mappoints[0].x);
      mxr=conv(mappoints[0].x);
      myl=conv(mappoints[0].y);
      myr=conv(mappoints[0].y);
      ll=0; lr=0;
      mxlb=mxl; mxle=mxl; mylb=myl; myle=myl;
      mxrb=mxr; mxre=mxr; myrb=myr; myre=myr;
      zlb=zl; zle=zl; zrb; zre=zr;
      rylb=ry; ryle=ry; ryrb; ryre=ry;
      txl=0; axl=0; tyl=0; ayl=0;
      txr=0; axr=0; tyr=0; ayr=0;
    }
    /// 
    inline void FillNextLine(int ry) const
    {
      inherited::FillNextLine(ry);
      mxl += txl;  mxr += txr;
      txl += axl;  txr += axr;
      myl += tyl;  myr += tyr;
      tyl += ayl;  tyr += ayr;
    }
    /// 
    inline void FillLeftChanging(int left) const
    {
      inherited::FillLeftChanging(left);
      zlb = zle;
      mxlb=mxle;
      mylb=myle;
      rylb=ryle;
    }
    /// 
    inline void FillLeftChanged(PointZ * points, int left) const
    {
      inherited::FillLeftChanged(points, left);
      zle =fconv(points[left].z);
      mxle=conv(mappoints[left].x);
      myle=conv(mappoints[left].y);
      ryle=points[left].y;
    }
    /// 
    inline void FillLeftSameCoef(int left) const
    {
      inherited::FillLeftSameCoef(left);
      mxl = conv(mappoints[left].x);
      myl = conv(mappoints[left].y);
      ll = frapport(zle,zlb);
    }
    /// 
    inline void FillLeftNotSameCoef(realtype deltay) const
    {
      inherited::FillLeftNotSameCoef(deltay);
      ll = frapport(zle,zlb);
      mapper_QuadMapCoefsInit(conv(rylb), conv(ryle), ll, conv(rylb),
        mxlb, mxle, mylb, myle,
        mxl, myl, txl, tyl, axl, ayl);
    }
    /// 
    inline void FillRightChanging(int right) const
    {
      inherited::FillRightChanging(right);
      zrb =zre;
      mxrb=mxre;
      myrb=myre;
      ryrb=ryre;
    }
    /// 
    inline void FillRightChanged(PointZ * points, int right) const
    {
      inherited::FillRightChanged(points, right);
      zre =fconv(points[right].z);
      mxre=conv(mappoints[right].x);
      myre=conv(mappoints[right].y);
      ryre=points[right].y;
    }
    /// 
    inline void FillRightSameCoef(int right) const
    {
      inherited::FillRightSameCoef(right);
      mxr = conv(mappoints[right].x);
      myr = conv(mappoints[right].y);
      lr = frapport(zre,zrb);
    }
    /// 
    inline void FillRightNotSameCoef(realtype deltay) const
    {
      inherited::FillRightNotSameCoef(deltay);
      lr = frapport(zre,zrb);
      mapper_QuadMapCoefsInit(conv(ryrb), conv(ryre), lr, conv(ryrb),
        mxrb, mxre, myrb, myre,
        mxr, myr, txr, tyr, axr, ayr);
    }
    inline void FillInitHoriz(realtype xl, realtype xr, realtype zl, realtype zr, int ry) const
    {
      inherited::FillInitHoriz(xl, xr, zl, zr, ry);
      mxb = mxl; myb = myl; mxe = mxr; mye = myr;
    }
    /// 
    inline void HorizCoefInit(realtype xb, realtype xe, realtype zb, realtype ze) const
    {
      inherited::HorizCoefInit(xb, xe, zb, ze);
      assert(zb!=0);
      assert(ze!=0);
      assert( (zb>0&&ze>0)||(zb<0&&ze<0) );
      l = frapport(ze,zb);
    }
    inline void HorizLocalsClipped(realtype xb, realtype xe, int xd, int xf, realtype zb, realtype ze, int rxb, int rxe) const
    {
      inherited::HorizLocalsClipped(xb, xe, xd, xf, zb, ze, rxb, rxe);
      mapper_QuadMapCoefsInit(xb, xe, l, conv(xd+1), mxb, mxe, myb, mye,
        mx, my, tx, ty, ax, ay);
    }
    /// 
    inline int HorizGetOffs() const { return int(round(mx)+ bl*round(my)); }
    inline Color HorizGetPixel(int offs) const
    {
      assert(int(Ima+ offs-ima->GetBuffer())>=0);
      assert(int(Ima+ offs-ima->GetBuffer())<ima->Len());
      return *(Ima+ offs);
    }
    /// 
    inline void HorizNextPixel() const
    {
      inherited::HorizNextPixel();
      mx += tx;
      tx += ax;
      my += ty;
      ty += ay;
    }
  };

#define HMPRES 256
  /**
  *HYPPERBOLICMAPPER
  *  Hypperbolic mapping algo. This algo introduce no deformations. The face is
  *  mapped correctly. The only deformation can be due to precision problems. In
  *  that case you must modify the HMPRES define. You can replace it by a
  *  variable if needed. This algo should be used when face is cutted (a part is
  *  behind the observer, the other is in front of it for example). The
  *  performance is varying. When there is a pixelization effect this algo is 
  *  faster than the Quadratic mapping algo. But is the pixel of the face are
  *  draw only one evry N pixel (when the face is far) this algo is slower. Note 
  *  that the HorizNextPixel is not expanded inline (in BC5), due to while 
  *  instruction, wich is slower. 
  */
  class HypperbolicMapper : public Mapper
  {
    typedef Mapper inherited;
  private :
    // fill locals
    mutable int rmxl;
    mutable int rmxr;
    mutable int rmyl;
    mutable int rmyr;
    mutable int rzl;
    mutable int rzr;
    mutable int rmxlb, rmxle;
    mutable int rmylb, rmyle;
    mutable int rmxrb, rmxre;
    mutable int rmyrb, rmyre;
    mutable int rylb,  ryle;
    mutable int ryrb,  ryre;
    mutable int rzlb,  rzle;
    mutable int rzrb,  rzre;
    mutable int umxl, umxr, umyl, umyr, uzl, uzr;
    mutable int smxl, smxr, smyl, smyr, szl, szr;
    //horiz locals
    mutable int ux, uy;
    mutable int smx, smy;
    mutable int rmxe, rmye;
    mutable int rmxb, rmyb;
    mutable int rze, rzb;
    mutable int rden;
    mutable int rmx;
    mutable int rmy;
    mutable int p;
    mutable int ip;
    mutable int qx;
    mutable int iqx;
    mutable int qy;
    mutable int iqy;

  public :
    /// 
    HypperbolicMapper() { }
    /// 
    inline void FillAssertions(int npoints, PointZ * points) const
    {
      for (int i=0; i<npoints; i++)
        points[i].z = points[i].z*HMPRES; // precision problem
    }
    /// 
    inline void FillInitLocals(realtype zl, realtype zr, int ry) const
    {
      inherited::FillInitLocals(zl, zr, ry);
      rmxl=mappoints[0].x;
      rmxr=mappoints[0].x;
      rmyl=mappoints[0].y;
      rmyr=mappoints[0].y;
      rzl=round(zl);
      rzr=round(zr);
      rmxlb=rmxl; rmxle=rmxl;
      rmylb=rmyl; rmyle=rmyl;
      rmxrb=rmxr; rmxre=rmxr;
      rmyrb=rmyr; rmyre=rmyr;
      rylb=ry;    ryle=ry;
      ryrb=ry;    ryre=ry;
      rzlb=rzl;   rzle=rzl;
      rzrb=rzr;   rzre=rzr;
      umxl=0; umxr=0; umyl=0; umyr=0; uzl=0; uzr=0;
      smxl=0; smxr=0; smyl=0; smyr=0; szl=0; szr=0;
    }
    inline void FillInitHoriz(realtype xl, realtype xr, realtype zl, realtype zr, int ry) const
    {
      inherited::FillInitHoriz(xl, xr, conv(rzl), conv(rzr), ry);
      mxb = conv(rmxl); myb = conv(rmyl); mxe = conv(rmxr); mye = conv(rmyr);
    }
    /// 
    inline void FillNextLine(int ry) const
    {
      inherited::FillNextLine(ry);
#define HYPINC(u,s,x,xb,xe,y,yb,ye,zb,ze)\
  u+=s*(zb*(ye-y)+ze*(y-yb));\
  while (u>0)\
      {\
      y+=s;\
      u+=zb*(xb-x)+ze*(x-xe);\
      }
      HYPINC(umxl,smxl,ry,rylb,ryle,rmxl,rmxlb,rmxle,rzlb,rzle);
      HYPINC(umyl,smyl,ry,rylb,ryle,rmyl,rmylb,rmyle,rzlb,rzle);
      HYPINC(umxr,smxr,ry,ryrb,ryre,rmxr,rmxrb,rmxre,rzrb,rzre);
      HYPINC(umyr,smyr,ry,ryrb,ryre,rmyr,rmyrb,rmyre,rzrb,rzre);
      HYPINC(uzl, szl, ry,rylb,ryle, rzl,rzlb, rzle, rzlb,rzle);
      HYPINC(uzr, szr, ry,ryrb,ryre, rzr,rzrb, rzre, rzrb,rzre);
    }
    /// 
    inline void FillLeftChanging(int left) const
    {
      inherited::FillLeftChanging(left);
      rzlb =rzle;
      rmxlb=rmxle;
      rmylb=rmyle;
      rylb =ryle;
    }
    /// 
    inline void FillLeftChanged(PointZ * points, int left) const
    {
      inherited::FillLeftChanged(points, left);
      rzle =int(points[left].z);
      rmxle=mappoints[left].x;
      rmyle=mappoints[left].y;
      ryle =points[left].y;
      smxl=sign(rmxle-rmxlb);
      smyl=sign(rmyle-rmylb);
      szl =sign(rzle-rzlb);
      umxl=0; umyl=0; uzl=0;
    }
    /// 
    inline void FillLeftSameCoef(int left) const
    {
      inherited::FillLeftSameCoef(left);
      rmxl = mappoints[left].x;
      rmyl = mappoints[left].y;
    }
    /// 
    inline void FillRightChanging(int right) const
    {
      inherited::FillRightChanging(right);
      rzrb =rzre;
      rmxrb=rmxre;
      rmyrb=rmyre;
      ryrb =ryre;
    }
    /// 
    inline void FillRightChanged(PointZ * points, int right) const
    {
      inherited::FillRightChanged(points, right);
      rzre =int(points[right].z);
      rmxre=mappoints[right].x;
      rmyre=mappoints[right].y;
      ryre =points[right].y;
      smxr=sign(rmxre-rmxrb);
      smyr=sign(rmyre-rmyrb);
      szr =sign(rzre-rzrb);
      umxr=0; umyr=0; uzr=0;
    }
    /// 
    inline void FillRightSameCoef(int right) const
    {
      inherited::FillRightSameCoef(right);
      rmxr = mappoints[right].x;
      rmyr = mappoints[right].y;
    }
    inline void HorizLocalsClipped(realtype xb, realtype xe, int xd, int xf, realtype zb, realtype ze, int rxb, int rxe) const
    {
      inherited::HorizLocalsClipped(xb, xe, xd, xf, zb, ze, rxb, rxe);
      ux=0; uy=0;
      smx=sign(mxe-mxb); smy=sign(mye-myb);
      rmxe=round(mxe)-smx; rmye=round(mye)-smy;
      rmxb=round(mxb); rmyb=round(myb);
      rze=round(ze); rzb=round(zb);
      xd+=1;
      rden=rzb*(rxb-xd)+rze*(xd-rxe);
      rmx=(rzb*rmxe*(rxb-xd)+rze*rmxb*(xd-rxe))/rden;
      rmy=(rzb*rmye*(rxb-xd)+rze*rmyb*(xd-rxe))/rden;
      p=rzb*(rxb-xd)+rze*(xd-rxe);
      ip=rze-rzb;
      qx=smx*(rzb*(rmxe-rmx)+rze*(rmx-rmxb));
      iqx=(rze-rzb);
      qy=smy*(rzb*(rmye-rmy)+rze*(rmy-rmyb));
      iqy=(rze-rzb);
    }
    /// 
    inline int HorizGetOffs() const { return rmx+ bl*rmy; }
    inline Color HorizGetPixel(int offs) const
    {
      //assert(int(Ima+ offs-ima->GetBuffer())>=0);
      //assert(int(Ima+ offs-ima->GetBuffer())<ima->Len());
      return *(Ima+ offs);
    }
    /// 
    inline void HorizNextPixel() const
    {
      inherited::HorizNextPixel();
      // not expanded inline -> slower
      ux += qx;
      while (ux>0)
      {
        rmx+=smx;
        qx +=iqx;
        ux += p;
      }
      uy += qy;
      while (uy>0)
      {
        rmy+=smy;
        qy +=iqy;
        uy += p;
      }
      p += ip;
    }
  };

}

#endif
