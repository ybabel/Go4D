#ifndef _BASE_H_
#define _BASE_H_


namespace Go4D
{


  typedef double real;
  typedef unsigned char byte;
  typedef unsigned int word;
#define self (*this)
#define kCR "\n"

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


  void error(const char * mess);
  void error1(const char * mess1, const char * mess2);
  void trace(const char * mess);

#ifdef UNDER_CE
#ifdef NDEBUG
#define assert(x)
#else
#define assert(x) if (!(x)) error1(#x, __FILE__)
#endif
#else
#include <assert.h>
#endif

#define DBGTrace(x) trace(x)

#ifdef UNDER_CE
#define M_PI 3.1415926535
#endif

}
#endif
