#include "GUIInput.h"


namespace Go4D
{


  namespace GUI
  {


    //*******************************************************************

    SpinButton::SpinButton(System * system, const char * name, int width, int height, bool vertical)
      : inherited(system, name, width, height)
    {
      fVertical = vertical;
    }

    SpinButton::~SpinButton()
    {
    }

    ANode * SpinButton::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      assert( (x < fWidth && y < fHeight) && (x>=0 && y>=0));
      if (  (!fVertical&&(x<fWidth/2)) || (fVertical&&(y<fHeight/2))  )
      {
        Action action(this, "Dec");
        GetListener()->Execute(&action, fName);
      }
      else
      {
        Action action(this, "Inc");
        GetListener()->Execute(&action, fName);
      }
      return this;
    }

    void SpinButton::DrawPass(Image *aImage, real x, real y, real width, real height)
    {
      FastBox(aImage, x, y, x+width, y+height, GetLayout()->BackgroundColor() );
      if (!fVertical)
      {
        GetDrawer()->Fill4(
          Trunc(x), Trunc(y+height/2),
          Trunc(x+width/2), Trunc(y),
          Trunc(x+width/2), Trunc(y+height),
          Trunc(x), Trunc(y+height/2),
          GetLayout()->ForegroundColor() );
        GetDrawer()->Fill4(
          Trunc(x+width), Trunc(y+height/2),
          Trunc(x+width/2), Trunc(y+height),
          Trunc(x+width/2), Trunc(y),
          Trunc(x+width), Trunc(y+height/2),
          GetLayout()->ForegroundColor());
      }
      else
      {
        GetDrawer()->Fill4(
          Trunc(x+width/2), Trunc(y),
          Trunc(x+width), Trunc(y+height/2),
          Trunc(x), Trunc(y+height/2),
          Trunc(x+width/2), Trunc(y),
          GetLayout()->ForegroundColor() );
        GetDrawer()->Fill4(
          Trunc(x+width/2), Trunc(y+height),
          Trunc(x), Trunc(y+height/2),
          Trunc(x+width), Trunc(y+height/2),
          Trunc(x+width/2), Trunc(y+height),
          GetLayout()->ForegroundColor());
      }
    }

    //*******************************************************************

    ColorEditor::ColorEditor(System * system, const char * name, int width, int height, int rows)
      : inherited(system, name, width, height)
    {
      fRows = rows;
    }

    ColorEditor::~ColorEditor()
    {
    }

    Color ColorEditor::GetColor(int i, int j, int width, int height)
    {
      return (i*256)/(fRows*width) + ((j*fRows)/height)*(256/fRows);
    }

    Color ColorEditor::GetSelectedColor()
    {
      return fSelectedColor;
    }

    ANode * ColorEditor::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      assert( (x < fWidth && y < fHeight) && (x>=0 && y>=0));
      fSelectedColor = GetColor(Trunc(x), Trunc(y), Trunc(fWidth), Trunc(fHeight));
      ClickEvent(x, y);
      return this;
    }

    void ColorEditor::DrawPass(Image * aImage, real x, real y, real width, real height)
    {
      for (int j=0; j<height; j++)
        for (int i=0; i<width; i++)
          GetDrawer()->Pixel(PointZ(Trunc(x)+i, Trunc(y)+j, GetColor(i, j, Trunc(width), Trunc(height)) ));
    }

    //**************************************************************

    Gauge::Gauge(System * system, const char * name, int width, int height, real min, real max)
      : inherited(system, name, width, height)
    {
      fMin = min;
      fMax = max;
      fCurrent = (fMin+fMax)/2;
    }

    Gauge::~Gauge()
    {
    }

    ANode * Gauge::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      fCurrent = (x/fWidth)*(fMax-fMin) + fMin;
      ChangeEvent();
      return this;
    }

    void Gauge::DrawPass(Image *aImage, real x, real y, real width, real height)
    {
      FastBox(aImage, x, y, x+width, y+height, GetLayout()->BackgroundColor() );
      Line(aImage, x,y, x, y+height, GetLayout()->ForegroundColor() );
      Line(aImage, x+width, y, x+width, y+height, GetLayout()->ForegroundColor());
      Line(aImage, x, y+height/2, x+width, y+height/2, GetLayout()->ForegroundColor());
      Line(aImage, x+fCurrent*width/(fMax-fMin), y, x+fCurrent*width/(fMax-fMin), y+height, GetLayout()->ForegroundColor());
    }

    //***************************************************************


    TextInput::TextInput(System * system, const char * name, int width, int height, int nbLines)
      : inherited(system, name, width, height)
    {
      fLines = nbLines;
      fShift = false;
      fCaps = false;
    }

    TextInput::~TextInput()
    {
    }

    void TextInput::GetUp(char * result)
    {
      if (fShift||fCaps)
        CopyStr(result, "ABCDEFGHIJKLMNOPQR   ");
      else
        CopyStr(result, "abcdefghijklmnopqr   ");
      result[18] = 17;
      result[19] = 27;
    }

    void TextInput::GetDn(char * result)
    {
      if (fShift||fCaps)
        CopyStr(result, "STUVWXYZ0123456789  .");
      else
        CopyStr(result, "stuvwxyz0123456789  .");
      result[18] = 23;
      result[19] = 24;
    }

    ANode * TextInput::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      char sup[30];
      GetUp(sup);
      char sdn[30];
      GetDn(sdn);
      int i;
      int j = Trunc(y*2/fHeight);
      if (j==0)
      {
        i = Trunc(x*StrLen(sup)/fWidth);
        fLastKey = sup[i];
      }
      else
      {
       i = Trunc(x*StrLen(sdn)/fWidth);
       fLastKey = sdn[i];
      }
      if (fLastKey==23)
      {
        fCaps = !fCaps;
        RefreshEvent();
      }
      if (fLastKey==24)
      {
        fShift = !fShift;
        RefreshEvent();
      }
      else
        fShift = false;
      Action keystroke(this, "Key");
      GetListener()->Execute(&keystroke, fName);
      return this;
    }

    void TextInput::DrawPass(Image *aImage, real x, real y, real width, real height)
    {
      char sup[30];
      GetUp(sup);
      char sdn[30];
      GetDn(sdn);
      DrawString(aImage, sup, StrLen(sup)*GetLayout()->Font()->GetDimX(), x, y, x+width, y+height/2);
      DrawString(aImage, sdn, StrLen(sdn)*GetLayout()->Font()->GetDimX(), x, y+height/2, x+width, y+height);
    }

    bool TextInput::ApplyLastKey(apstring& s)
    {
      switch (fLastKey)
      {
      case 23 : ;
      case 24 : break; // shift & caps
      case 17 : return true; // return
      case 27 : s = s.substr(0, s.length()-1); break; // DEL
      default : s += fLastKey;
      }
      return false;
    }


    //***************************************************************

    OptionBox::OptionBox(System * system, const char * name, int width, int height, int lines)
      : inherited(system, name, width, height),
      fSelected(true)
    {
      fLines = lines;
    }

    OptionBox::~OptionBox()
    {
    }

    void OptionBox::AddString(const char * btn)
    {
      fStrings.Add(btn);
      fSelected.Add(new bool);
      *fSelected.Current() = false;
      fStrings.Complete();
      fSelected.Complete();
    }

    ANode * OptionBox::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      assert( (x < fWidth && y < fHeight) && (x>=0 && y>=0));
      if (kind != mekClick) return this;
      assert(fStrings.Count() >0);
      int count = fStrings.Count();
      int index = Trunc((x*count)/fWidth);
      *fSelected[index] = !(*fSelected[index]);
      Action click(this, "Click");
      GetListener()->Execute(&click, fStrings[index]);
      return this;
    }

    void OptionBox::DrawPass(Image *aImage, real x, real y, real width, real height)
    {
      assert(fStrings.Count() >0);
      int count = fStrings.Count();
      // TODO : handle multilines display
      int countByLine = count/fLines;
      real size = width / count;
      for (int i=0; i<count; i++)
      {
        GetWriter()->SetBackground( *fSelected[i]?GetLayout()->SelectedColor():GetLayout()->BackgroundColor() );
        DrawString(aImage, fStrings[i], Trunc(fWidth/count),
          x+i*size, y, x+(i+1)*size, y+height);
      }
      GetWriter()->SetBackground(GetLayout()->BackgroundColor());
    }

    bool OptionBox::GetIsSelected(const char * ch) const
    {
      int index = fStrings.IndexOf(ch);
      if (index != NOTFOUND)
        return GetIsSelected(index);
      else
        return false;
    }

    bool OptionBox::GetIsSelected(int index) const
    {
      return *fSelected[index];
    }

    void OptionBox::SetIsSelected(const char * ch, bool value)
    {
      int index = fStrings.IndexOf(ch);
      if (index != NOTFOUND)
        *fSelected[index] = value;
    }


  }

}
