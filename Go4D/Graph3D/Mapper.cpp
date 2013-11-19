#include "Mapper.h"

namespace Go4D
{


  void mapper_QuadMapCoefs(realtype xb,realtype xe,realtype l,
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

  void mapper_QuadMapInit(real fa, real fb, real fc, realtype xd,
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

  void mapper_QuadMapCoefsInit(realtype xb, realtype xe, realtype l,
    realtype xd,
    realtype mxb, realtype mxe,
    realtype myb, realtype mye,
    realtype &mx, realtype &my,
    realtype &tx, realtype &ty,
    realtype &ax, realtype &ay)
  {
    real fa, fb, fc;
    mapper_QuadMapCoefs(xb,xe,l, fa, fb, fc);
    mapper_QuadMapInit(fa, fb, fc, xd, mxb, mxe, myb, mye, mx, my, tx, ty, ax, ay);
  }


}
