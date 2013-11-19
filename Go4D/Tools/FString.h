#ifndef _STRING_H_
#define _STRING_H_

/**
*AUTHOR    : Babel Yoann
*TITLE     : String.h
*DATE      : 11/12/95
*OBJECT    : String handling (simplifiers functions)
*/

#include "Base.h"

namespace Go4D
{



  #define S_SIZE 256
  typedef char fstring[S_SIZE]; // fixed lenght string

  #define NewString(S)  ((S==NULL) ? NULL:(strcpy(new char[(strlen(S)+1)], S)))


  int StrLen(const char * s);
  bool SameStr(const char * left, const char * right);
  bool FinishStr(const char * str, const char * substr);
  void UpperStr(char * );
  void Int2Str(int x, char * s, int complete=0);
  void Str2Int(int& x, const char * s);
  int Str2Int(const char *);
  void Real2Str(real x, char * s);
  void Str2Real(real& x, const char * s);
  void CopyStr(char * dest, const char * src);
  void ConcatStr(char * dest, const char * src);
  void AddStr(char * dst, const char * src, bool start=false);
  void ClearStr(fstring );
  void SubStr(char * dest, const char * src, int beg, int len);

}

#endif
