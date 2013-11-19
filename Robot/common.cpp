#include "common.h"

int StrLen(char * s)
{
  int result;
  for (result=0; s[result]; result++);
  return result;
}

void Int2Str(int x, char * s)
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
  for (int j=flag; j<i/2; j++)
    {
      char tmp = s[j-flag];
      s[j-flag] = s[i-j+flag];
      s[i-j+flag] = tmp;
    }
}
