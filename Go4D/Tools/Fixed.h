#ifndef _FIXED_H_
#define _FIXED_H_

/**
*AUTHOR    : Babel Yoann
*TITLE     : Fixed.h
*DATE      : 11/12/95
*OBJECT    : Fixed point arithmetic. You can use fixed point or floats
*            by defining "usefixed / usefloat"
*/

#include <math.h>



namespace Go4D
{


#ifdef usefixed
#define fprecision 16
#define ppres (int(1<<fprecision))
#define inc(a,b) (a+=(b))
#define incn(a,b,n) (a+=((b)*(n)))
#define rapport(num,div) (int(long(\
  (long(num)<<fprecision) / long((div)==0?1:(div))\
  )))
#define frapport(num,div) (int(float(\
  ( real(num) / real((fabs(div)<=1e-3)?1:(div)) )*real(ppres)\
  )))
#define round(a) int(((a) >> fprecision))
#define fround(a) (real(real(a)/ppres))
#define conv(a) ((a) << fprecision)
#define fconv(a) int(real( (a)*real(ppres) ))
#define realtype int
#define mult(a,b) ( (long)a*(long)b >> fprecision)
#define divi(a,b) (int(long( (long(a)<<fprecision)/long(b))))
#define Mini int(0x80000000)
#define Maxi int(0x7fffffff)
#endif
#ifdef usefloat
#include <float.h>
#define inc(a,b) a+=b
#define incn(a,b,n) (a+=(b)*(n))
#define rapport(num,div) (real(num)/real(div==0?1:div))
#define frapport(num,div) (real(num)/real(div==0?1:div))
#define round(a) int(a)
#define fround(a) (a)
#define conv(a) real(a)
#define fconv(a) (a)
#define realtype real
#define mult(a,b) ((a)*(b))
#define divi(a,b) ((a)/(b))
#define Mini FLT_MIN
#define Maxi FLT_MAX
#endif
  inline void SwapR(realtype &a, realtype &b)
  {
    realtype tmp;
    tmp = a;
    a = b;
    b = tmp;
  }

}

#endif
