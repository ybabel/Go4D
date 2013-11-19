#ifndef _COMMON_H_
#define _COMMON_H_

/**
*AUTHOR    : Babel Yoann
*TITLE     : Common.h
*DATE      : 11/12/95
*MODIF     : 17/01/01
*OBJECT    : common feature like error handling, real definition, and
*            some other useful macro
*            added some feature to run under PocketPC
*/

#include <stdlib.h>
#include <stdio.h>

#include "Base.h"
#include "Streams.h"
#include "Fixed.h"
#include "FString.h"
#include "Variant.h"


namespace Go4D
{

#ifdef VSNET
#include <memory.h>
#endif

#define zero 0.01
#define cutprecision 0.01

#define sqr(a) ((a)*(a))

#define TRUE 1
#define FALSE 0

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)<(b)?(b):(a))
#define min3(a,b,c) min( min(a,b),c )
#define max3(a,b,c) max( max(a,b),c )
#define min4(a,b,c,d) min( min(a,b), min(c,d) )
#define max4(a,b,c,d) max( max(a,b), max(c,d) )

#define sign(x) ((x)>=0?1:-1)
#define abs(x) ((x)>=0?(x):-(x))

  // various random generators ...
  // frandomf(f): returns a random float-number between 0.0 and f
#define frandomf(f) (float)(((f+1.0)*rand())/(RAND_MAX+1.0))
#define irandomf(f) (int)frandom(f)
#define random(x) (rand()%(x))
#define frand (float(random(10000))/10000.0)
#define FLT_MAX         3.402823466e+38F        /* max value */
#define FLT_MIN         1.175494351e-38F        /* min positive value */

  inline void Swap(int &a, int &b)
  {
    int tmp;
    tmp = a;
    a = b;
    b = tmp;
  }

  inline void Swap(real &a, real &b)
  {
    real tmp;
    tmp = a;
    a = b;
    b = tmp;
  }
  int Trunc(real);


  //#ifndef UNDER_CE
  //#define alloc(ex) if ( (ex)==NULL ) cout << "memory allocation " << #ex << \
  //" failed, line : " << __LINE__ << " file : " << __FILE__ << "\n"
  //#else
#define alloc(ex) if ( (ex)==NULL) error1("allocation failed", "#ex")
  //#endif

}

#endif
