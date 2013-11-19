#ifndef _WRITEDRAWER_H_
#define _WRITEDRAWER_H_

/*
*AUTHOR : Babel yoann
*TITLE  : WriteDrawer.h, writing strings in an image
*CREATED: 19/05/99
*DATE   : 19/05/99
*OBJECT : The WriteDrawer class allows to draw various text in the image
*         (or ZImage, or whatever you want.
*/

#include "Drawer.h"
#include "Font.h"
#include "Tools/FString.h"
#include <memory.h>

namespace Go4D
{


  /**
  * WRITEDRAWER
  *   This class is used to draw text in an image.
  */
  template <class Pixeler> class WriteDrawer : public Drawer<Pixeler>
  {
    typedef Drawer<Pixeler> inherited;
    // YOB GCC.41
    #define fPixeler inherited::fPixeler
    #define xmax_ inherited::xmax
    #define xmin_ inherited::xmin
    #define ymax_ inherited::ymax
    #define ymin_ inherited::ymin

  public :
    ///
    WriteDrawer(const Image &, int border=0, bool drawBackGround=false, Color backgroundColor=0);
    ///
    WriteDrawer(int width, int height, bool AutoAlloc=true, int border=0, bool drawBackGround=false, Color backgroundColor=0);
    ///
    void SetBackground(Color backgroundColor) { fBackgroundColor=backgroundColor; }
    ///
    virtual ~WriteDrawer();
    ///
    void ClearLine(const PointZ &p, const AFont &f)
    {
      fPixeler.SetStart(p);
      for (int j=0; j<min(ymax_,p.y+f.GetDimY())-p.y; j++)
      {
        for (int i=0; i<xmax_-p.x; i++, fPixeler.Next())
          fPixeler.Pixel();
        fPixeler.NextL(p.x);
      }
    }
    ///  back ground color is used
    virtual void PutChar(const PointZ &p, const AFont & f, const char c)
    {
      Color color = p.color;
      int x,y,w,h;
      byte * chr = f[c];
      if (ClipChar(p,f,x,y,w,h,chr)) return;
      byte * schr = chr;

      if (fDrawBackground)
      {
        fPixeler.SetStart(PointZ(x, y, fBackgroundColor, p.z));
        for (int j=0; j<h; j++)
        {
          for (int i=0; i<w; i++, fPixeler.Next(), chr++)
            if (*chr == OFF)
              fPixeler.Pixel();
          fPixeler.NextL(inherited::fWidth-w);
          chr += f.GetDimX()-w;
        }
      }

      fPixeler.SetStart(PointZ(x, y, color, p.z));
      chr = schr;
      for (int j=0; j<h; j++)
      {
        for (int i=0; i<w; i++, fPixeler.Next(), chr++)
          if (*chr == ON)
            fPixeler.Pixel();
        fPixeler.NextL(inherited::fWidth-w);
        chr += f.GetDimX()-w;
      }
    }
    ///
    void CenteredWriteString(const PointZ& p, const AFont & f, const char * s)
    {
      PointZ tp = p;
      int dx = f.GetDimX();
      tp.x -= (StrLen(s)*dx)/2;
      tp.y -= f.GetDimY()/2;
      for (int i=0; s[i]!=0; i++, tp.x+=dx)
        PutChar(tp, f, s[i]);
    }
    ///
    void WriteString(const PointZ& p, const AFont & f, const char * s)
    {
      PointZ tp = p;
      int dx = f.GetDimX();
      for (int i=0; s[i]!=0; i++, tp.x+=dx)
        PutChar(tp, f, s[i]);
    }
    ///  write string, clear the end of the line
    void WriteLine(const PointZ& p, const AFont & f, const char * s)
    {
      WriteString(p,f,s);
      ClearLine(PointZ(p.x+StrLen(s)*f.GetDimX(), p.y,fBackgroundColor),f);
    }
    /// no clipping ! it must be done by the caller, assume that xmin... are correct
    /// drawbackground is always true here
    /// Pixeler is not used, it's a fast primitive drawing method, don't take
    /// in account enhancements
    void MapString(const char * s, const AFont & f,
      int size, int _xmin, int _ymin, int _xmax, int _ymax, Color color)
    {
      int w = f.GetDimX();
      int h = f.GetDimY();
      Color * pixel = inherited::fBuffer+_xmin+_ymin*inherited::fWidth;
      int dx = _xmax-_xmin;
      for (int y=_ymin; y<_ymax; y++)
      {
        int curchar = 0;
        byte c = s[curchar];
        int chrdy = (((y-_ymin)*h)/(_ymax-_ymin)) *w;
        byte * chr = f[c]+ chrdy;
        int cursizeC=0;
        int cursizeP=0;
        for (int x=_xmin; x<_xmax; x++, pixel++)
        {
          *pixel = *chr==ON?color:fBackgroundColor;
          cursizeC+=size;
          cursizeP+=size;
          if (cursizeC>=w*dx) // go next char
          {
            if (c != 0)
              curchar++;
            cursizeC-=w*dx;
            cursizeP=0;
            c = s[curchar];
            chr = f[c]+chrdy;
          }
          if (cursizeP >= dx) // go next pixel of the current char
          {
            chr++;
            cursizeP -= dx;
          }
        }
        pixel += (inherited::fWidth-_xmax)+_xmin;
      }
    }
    ///
    void WriteInt(const PointZ&p, const AFont & f, int i, int complete=0)
    {
      fstring str;
      Int2Str(i, str, complete);
      WriteString(p, f, str);
    }
    ///
    void WriteReal(const PointZ&p, const AFont & f, real r)
    {
      fstring str;
      Real2Str(r, str);
      WriteString(p, f, str);
    }


  protected :
    bool ClipChar(const PointZ &p, const AFont &f,
      int	&x,	int	&y,	int	&w,	int	&h,
      byte *&	chr)
    {
      w	=	f.GetDimX();
      h	=	f.GetDimY();
      x	=	p.x;
      y	=	p.y;

      if (x	>= xmax_) return	true;
      if (y	>= ymax_) return	true;
      if (x+w	<	xmin_)	return true;
      if (y+h	<	ymin_)	return true;

      if (x+w	>	xmax_)	w	=	xmax_-x;
      if (y+h	>	ymax_)	h	=	ymax_-y;
      if (x<xmin_)	{w-=(xmin_-x);	chr+=(xmin_-x); x=xmin_;}
      if (y<ymin_)	{h-=(ymin_-y);	chr+=(ymin_-y)*f.GetDimX(); y=ymin_;}
      return false;
    }

    bool fDrawBackground;
    Color fBackgroundColor;
  };

  typedef WriteDrawer<Pixeler> Writer;

  /**
  * MESSAGER
  *   This class is used to draw a pool of messages. A message is displayed
  *   only for a while (or when an other message is displayed)
  */
  template <class Pixeler> class Messager
  {
  public :
    ///
    Messager(AFont &, int time, int maxmessages, Color color=1);
    ///
    ~Messager();
    ///
    void PutMessage(const char * message)
    {
      strncpy(messages[last],message,255);
      // search the older message
      /**/
      int older=10000;
      int i;
      for (i=0; i<max_messages; i++)
        if (timer[i]<older)
        {
          last = i;
          older = timer[i];
        }
        timer[last] = time;
        /**/
        //last ++;
        //last %= max_messages;
    }
    ///
    void Draw(WriteDrawer<Pixeler> & drawer)
    {
      int y = 0;
      int h = font->GetDimY();

      for (int i=0; i<max_messages; i++)
        if (timer[i]!=0)
        {
          drawer.WriteString(PointZ(0,y,color,0), *font, messages[i]);
          y += h;
        }

        DecreaseTime();
    }


  private :
    void DecreaseTime()
    {
      for (int i=0; i<max_messages; i++)
        if (timer[i]>0) timer[i]--;
    }

    fstring * messages;
    int * timer;
    int last;
    int time;
    Color color;
    int max_messages;
    AFont * font;
  };

  //---------------------------------------------------------------------------
  template <class Pixeler> WriteDrawer<Pixeler>::WriteDrawer(const Image &aImage, int pBorder, bool drawBackGround, Color backgroundColor)
    : inherited(aImage, pBorder)
  {
    fDrawBackground = drawBackGround;
    fBackgroundColor = backgroundColor;
  }

  template <class Pixeler> WriteDrawer<Pixeler>::WriteDrawer(int pWidth, int pHeight, bool autoalloc, int pBorder, bool drawBackGround, Color backgroundColor)
    : inherited(pWidth, pHeight, autoalloc, pBorder)
  {
    fDrawBackground = drawBackGround;
    fBackgroundColor = backgroundColor;
  }

  template <class Pixeler> WriteDrawer<Pixeler>::~WriteDrawer()
  {
  }

  //*****************************************************************************

  template <class Pixeler> Messager<Pixeler>::Messager(AFont & pfont, int ptime, int pmaxmessages, Color pcolor)
  {
    font = & pfont;
    time = ptime;
    max_messages = pmaxmessages;
    alloc(messages = new fstring [max_messages]); // YOB GCC 4.1
    alloc(timer = new int [max_messages]);
    for (int i=0; i<max_messages; i++)
    {
      messages[i][0] = 0;
      timer[i] = 0;
    }
    color = pcolor;
    last = 0;
  }

  template <class Pixeler> Messager<Pixeler>::~Messager()
  {
    delete [] messages;
    delete [] timer;
  }


}

#endif
