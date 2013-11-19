#include "FString.h"
#include <stdlib.h>
#include "Common.h"


namespace Go4D
{


  int StrLen(const char * s)
  {
    int result;
    for (result=0; s[result]; result++);
    return result;
  }

  void AddStr(char * dst, const char * src, bool start)
  {
    int len = StrLen(dst);
    int slen = StrLen(src);
    if (!start)
      for (int i=0; i<=slen; i++)
        dst[len+i] = src[i];
    else
    {
      int i;
      for (i=len; i>0; i--)
        dst[i+slen-1] = dst[i-1];
      dst[len+slen] = 0;
      for (i=0; i<slen; i++)
        dst[i] = src[i];
    }
  }

  void SubStr(char * dest, const char * src, int beg, int len)
  {
    for (int i=beg; i<beg+len; i++)
      dest[i-beg] = src[i];
    dest[len] = 0;
  }

  void Int2Str(int x, char * s, int complete)
  {
    int i =0;
    int flag = 0;
    if (x<0) { s[0]= '-'; i++; x = -x; flag=1; }
    do
    {
      s[i] = char(x%10+'0');
      x /= 10;
      i++;
    }
    while (x != 0);
    s[i]=0;
    i--;
    for (int j=flag; j<(i+1)/2; j++)
    {
      char tmp = s[j-flag];
      s[j-flag] = s[i-j+flag];
      s[i-j+flag] = tmp;
    }
    while (StrLen(s) < complete)
      AddStr(s, "0", true);
  }

  void Str2Int(int& x, const char * s)
  {
    x = 0;
    while (*s != 0)
    {
      x *= 10;
      x += byte(*s)-byte('0');
      s++;
    }
  }

  int Str2Int(const char * s)
  {
    int result;
    Str2Int(result, s);
    return result;
  }

  void CopyStr(char * dest, const char * src)
  {
    while ( byte(*src) != 0x0)
      *(dest++) = *(src++);
    // copy \0 last char
    *dest = *src;
  }

  void Real2Str(real x, char * s)
  {
#ifndef UNDER_CE
    gcvt(x, 5, s);
#else
    _gcvt(x, 5, s);
#endif
  }

  void Str2Real(real& x, const char * s)
  {
    x = atof(s);
  }

  void ClearStr(fstring s)
  {
    for (int i=0; i<S_SIZE; i++)
      s[i] = char(0);
  }

  bool SameStr(const char * left, const char * right)
  {
    while ( (*left!=0) && (*right!=0) )
    {
      if (*left!=*right) return false;
      left++; right++;
    }
    // dont compare just the begining, check that they have the same size
    if (*left!=*right) return false;
    return true;
  }

  bool FinishStr(const char * str, const char * substr)
  {
    const char * startsub = substr;
    substr += StrLen(substr);
    const char * start = str;
    str += StrLen(str);
    while ( (substr != startsub) && (str != start) )
    {
      if (*substr != *str) return false;
      substr--; str--;
    }
    return true;
  }

  void ConcatStr(char * dest, const char * src)
  {
    dest = dest+StrLen(dest);
    while ( byte(*src) != 0x0)
      *(dest++) = *(src++);
    // copy \0 last char
    *dest = *src;
  }

  void UpperStr(char * value)
  {
    while (*value != 0)
    {
      if ( (*value>='a') && (*value<='z') )
        *value += 'A'-'a';
      value++;
    }
  }


}
