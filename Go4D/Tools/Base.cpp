#include "Base.h"
#include <stdlib.h>
#include "Streams.h"

#ifdef VSNET
#include <windows.h>
#endif

namespace Go4D
{



#ifdef UNDER_CE
  TCHAR buf[512];
  LPCTSTR StrToUnicode(const char * mess)
  {
    MultiByteToWideChar(
      CP_ACP,
      MB_PRECOMPOSED,
      mess,
      strlen(mess)+1,
      buf,
      sizeof(buf));
    return buf;
  }

  TCHAR buf1[512];
  LPCTSTR StrToUnicode1(const char * mess)
  {
    MultiByteToWideChar(
      CP_ACP,
      MB_PRECOMPOSED,
      mess,
      strlen(mess)+1,
      buf1,
      sizeof(buf1));
    return buf1;
  }
#endif

  void error(const char * mess)
  {
    cerr << "error : " << mess << kCR << flush;
#ifdef UNDER_CE
//    MessageBox(0, StrToUnicode(mess), L"error", MB_OK);
#endif
#ifdef VSNET
    MessageBox(0, mess, "error", MB_OK);
#endif
    exit(1);
  }

  void error1(const char * mess1, const char * mess2)
  {
    cerr << "error : " << mess1 << " " << mess2 << kCR << flush;
#ifdef UNDER_CE
    //MessageBox(0, StrToUnicode(mess2), StrToUnicode1(mess1), MB_OK);
#endif
#ifdef VSNET
    MessageBox(0, mess2, mess1, MB_OK);
#endif
    exit(1);
  }

  void trace(const char * mess)
  {
    cout << "trace : " << mess << kCR << flush;
#ifdef UNDER_CE
    //MessageBox(NULL, StrToUnicode(mess), L"Trace", MB_OK);
#endif
#ifdef VSNET
    //MessageBox(NULL, mess, "Trace", MB_OK);
#endif
  }

}

