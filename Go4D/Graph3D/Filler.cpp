/**
*AUTHOR : Babel yoann
*TITLE  : Filler.cpp, Filler.h implementation
*CREATED: 06/08/96
*DATE   : 06/08/96
*BUGS   :
*/

#include "Filler.h"

namespace Go4D
{

  namespace Obsolete
  {

    int Mapper::bl;
    realtype Mapper::mxb, Mapper::myb, Mapper::mxe, Mapper::mye;
    Color * Mapper::Ima;

    realtype LinearMapper::mxl;
    realtype LinearMapper::mxr;
    realtype LinearMapper::myl;
    realtype LinearMapper::myr;
    realtype LinearMapper::imxl, LinearMapper::imxr, LinearMapper::imyl, LinearMapper::imyr;
    realtype LinearMapper::deltamx;
    realtype LinearMapper::deltamy;
    realtype LinearMapper::mx, LinearMapper::imx, LinearMapper::my, LinearMapper::imy;

    realtype QuadraticMapper::mxl;
    realtype QuadraticMapper::mxr;
    realtype QuadraticMapper::myl;
    realtype QuadraticMapper::myr;
    realtype QuadraticMapper::ll, QuadraticMapper::lr;
    realtype QuadraticMapper::mxlb, QuadraticMapper::mxle, QuadraticMapper::mylb, QuadraticMapper::myle;
    realtype QuadraticMapper::mxrb, QuadraticMapper::mxre, QuadraticMapper::myrb, QuadraticMapper::myre;
    realtype QuadraticMapper::zlb, QuadraticMapper::zle, QuadraticMapper::zrb, QuadraticMapper::zre;
    int QuadraticMapper::rylb, QuadraticMapper::ryle, QuadraticMapper::ryrb, QuadraticMapper::ryre;
    realtype QuadraticMapper::txl, QuadraticMapper::axl, QuadraticMapper::tyl, QuadraticMapper::ayl;
    realtype QuadraticMapper::txr, QuadraticMapper::axr, QuadraticMapper::tyr, QuadraticMapper::ayr;
    realtype QuadraticMapper::l, QuadraticMapper::mx, QuadraticMapper::my, QuadraticMapper::tx, QuadraticMapper::ty, QuadraticMapper::ax, QuadraticMapper::ay;

    int HypperbolicMapper::rmxl;
    int HypperbolicMapper::rmxr;
    int HypperbolicMapper::rmyl;
    int HypperbolicMapper::rmyr;
    int HypperbolicMapper::rzl;
    int HypperbolicMapper::rzr;
    int HypperbolicMapper::rmxlb, HypperbolicMapper::rmxle;
    int HypperbolicMapper::rmylb, HypperbolicMapper::rmyle;
    int HypperbolicMapper::rmxrb, HypperbolicMapper::rmxre;
    int HypperbolicMapper::rmyrb, HypperbolicMapper::rmyre;
    int HypperbolicMapper::rylb,  HypperbolicMapper::ryle;
    int HypperbolicMapper::ryrb,  HypperbolicMapper::ryre;
    int HypperbolicMapper::rzlb,  HypperbolicMapper::rzle;
    int HypperbolicMapper::rzrb,  HypperbolicMapper::rzre;
    int HypperbolicMapper::umxl, HypperbolicMapper::umxr, HypperbolicMapper::umyl, HypperbolicMapper::umyr, HypperbolicMapper::uzl, HypperbolicMapper::uzr;
    int HypperbolicMapper::smxl, HypperbolicMapper::smxr, HypperbolicMapper::smyl, HypperbolicMapper::smyr, HypperbolicMapper::szl, HypperbolicMapper::szr;
    int HypperbolicMapper::ux, HypperbolicMapper::uy;
    int HypperbolicMapper::smx, HypperbolicMapper::smy;
    int HypperbolicMapper::rmxe, HypperbolicMapper::rmye;
    int HypperbolicMapper::rmxb, HypperbolicMapper::rmyb;
    int HypperbolicMapper::rze, HypperbolicMapper::rzb;
    int HypperbolicMapper::rden;
    int HypperbolicMapper::rmx;
    int HypperbolicMapper::rmy;
    int HypperbolicMapper::p;
    int HypperbolicMapper::ip;
    int HypperbolicMapper::qx;
    int HypperbolicMapper::iqx;
    int HypperbolicMapper::qy;
    int HypperbolicMapper::iqy;

    void QuadMapCoefs(realtype xb,realtype xe,realtype l,
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

    void QuadMapInit(real fa, real fb, real fc, realtype xd,
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

    void QuadMapCoefsInit(realtype xb, realtype xe, realtype l,
      realtype xd,
      realtype mxb, realtype mxe,
      realtype myb, realtype mye,
      realtype &mx, realtype &my,
      realtype &tx, realtype &ty,
      realtype &ax, realtype &ay)
    {
      real fa, fb, fc;
      QuadMapCoefs(xb,xe,l, fa, fb, fc);
      QuadMapInit(fa, fb, fc, xd, mxb, mxe, myb, mye, mx, my, tx, ty, ax, ay);
    }

  }

}
