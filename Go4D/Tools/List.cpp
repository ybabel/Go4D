#include "List.h"
#include <string.h>

namespace Go4D
{


  bool CompareStr(String a, String b)
  {
    return strcmp(a, b)==0;
  }

  /******************************************************************************\
  * StringArray
  \******************************************************************************/

  StringArray::StringArray()
    : inherited(false, false, CompareStr)
  {
  }

  /******************************************************************************\
  * Stringlist
  \******************************************************************************/

  StringList::StringList()
    : inherited(false, true, CompareStr)
  {
  }

}
