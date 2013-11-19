/*****************************************************************************\
 *AUTHOR    : Babel Yoann
 *TITLE     : common.h
 *DATE      : 11/12/95
 *OBJECT    : common feature like error handling, real definition, and
 *            some other useful macro
\*****************************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef LIBGPP
#include <iostream.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <float.h>

typedef float real;
typedef unsigned char byte;
typedef unsigned int word;
typedef char string[256];

#define epsilonprecision 0.01
#define cutprecision 0.01

#ifdef LIBGPP
#define alloc(ex) if ( (ex)==NULL ) cout << "memory allocation " << #ex << \
" failed, line : " << __LINE__ << " file : " << __FILE__ << "\n"
#else
#define alloc(ex) (ex)
#endif

#define sqr(a) ((a)*(a))
//#define self (*this)
#define kCR "\n"
#ifndef NULL
#define NULL ((void *)0)
#endif
#define TRUE 1
#define FALSE 0
#define minimum(a,b) ((a)<(b)?(a):(b))
#define maximum(a,b) ((a)<(b)?(b):(a))
#define min3(a,b,c) minimum( minimum(a,b),c )
#define max3(a,b,c) maximum( maximum(a,b),c )
#define min4(a,b,c,d) minimum( minimum(a,b), minimum(c,d) )
#define max4(a,b,c,d) maximum( maximum(a,b), maximum(c,d) )
#define random(x) (rand()%(x))
#define frand (float(random(10000))/10000.0)
#define sign(x) ((x)>=0?1:-1)
#define abs(x) ((x)>=0?(x):-(x))




#ifdef usefixed
#define fixedprecision 16
#define ppres (int(1<<fixedprecision))
#define inc(a,b) (a+=(b))
#define incn(a,b,n) (a+=((b)*(n)))
#define rapport(num,div) (int(long(\
(long(num)<<fixedprecision) / long((div)==0?1:(div))\
)))
#define frapport(num,div) (int(float(\
( real(num) / real((fabs(div)<=1e-3)?1:(div)) )*real(ppres)\
)))
#define round(a) int(((a) >> fixedprecision))
#define roundf(a) (real(real(a)/ppres))
#define conv(a) ((a) << fixedprecision)
#define fconv(a) int(real( (a)*real(ppres) ))
#define realtype int
#define mult(a,b) ( (long)a*(long)b >> fixedprecision)
#define divi(a,b) (int(long( (long(a)<<fixedprecision)/long(b))))
#define Mini int(0x80000000)
#define Maxi int(0x7fffffff)
#endif

#ifdef usefloat
#define inc(a,b) a+=b
#define incn(a,b,n) (a+=(b)*(n))
#define rapport(num,div) (real(num)/real(div==0?1:div))
#define frapport(num,div) (real(num)/real(div==0?1:div))
#define round(a) int(a)
#define roundf(a) (a)
#define conv(a) real(a)
#define fconv(a) (a)
#define realtype real
#define mult(a,b) ((a)*(b))
#define divi(a,b) ((a)/(b))
//#define Mini -MAXFLOAT
//#define Maxi MAXFLOAT
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

inline void Swap(int &a, int &b)
{
  int tmp;
  tmp = a;
  a = b;
  b = tmp;
}

inline void error(char * mess)
{
#ifdef LIBGPP
  cout << "error : " << mess << kCR;
#else
  //printf("error : %s\n",mess);
#endif
  exit(1);
}

inline void error1(char * mess1, char * mess2)
{
#ifdef LIBGPP
  cout << "error : " << mess1 << " " << mess2 << kCR;
#else
  //printf("error : %s %s\n",mess1, mess2);
#endif
  exit(1);
}

int StrLen(char * s);
void Int2Str(int x, char * s);

#endif

