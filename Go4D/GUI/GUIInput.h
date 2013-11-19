#ifndef _GUI_INPUT_H_
#define _GUI_INPUT_H_

/**
*AUTHOR : Babel yoann
*TITLE  : GUIInput.h, output nodes
*CREATED: 21/01/01
*DATE   : 21/02/01
*OBJECT : contains some nodes to make some input objects
*/

#include "Gui.h"

namespace Go4D
{


  namespace GUI
  {

    /**
    * The spin button is a double button where the upper button
    * raise the value, and the lower button decrease it
    */
    class SpinButton : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      SpinButton(System * system, const char * name, int width, int height, bool vertical=true);
      ///
      ~SpinButton();
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
    protected :
      bool fVertical;
    };

    /**
    * The color editor present all the 256 color of the palette
    * the user have just to choose one color visually
    * the colors can be displayed on many lines to ensure that each color
    * is represented visually
    */
    class ColorEditor : public ANode
    {
      typedef ANode inherited;
    public :
      ColorEditor(System * system, const char * name, int width, int height, int rows=16);
      ~ColorEditor();
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      void DrawPass(Image *, real x, real y, real width, real height);
      Color GetSelectedColor();
    protected :
      int fRows;
      int fSelectedColor;
      Color GetColor(int i, int j, int width, int height);
    };

    /**
    * A gauge allow to set an integer value inside an interval
    */
    class Gauge : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      Gauge(System * system, const char * name, int width, int height, real min, real max);
      ///
      ~Gauge();
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
    protected :
      real fMin;
      real fMax;
      real fCurrent;
    };

    /**
    * The input text panel is used to enter characters. It can handle
    * any set of char, but ussually it treat the 26 letters, the 10 numbers
    * + the delete and return keys +shift and caps which make 40 chars.
    * the panel can be displayed on many lines to ensure maximal usage confort
    */
    class TextInput : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      TextInput(System * system, const char * name, int width, int height, int nbLines=2);
      ///
      ~TextInput();
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
      ///
      char LastKey() const { return fLastKey; }
      /// return true for carriage return, false in other case
      bool ApplyLastKey(apstring& s);
    protected :
      int fLines;
      bool fCaps;
      bool fShift;
      void GetUp(char *);
      void GetDn(char *);
      char fLastKey;
    };

    /**
    * This class looks like the tool box, but each "tool" became an "option" that can
    * be selected or not.
    */
    class OptionBox : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      OptionBox(System * system, const char * name, int width, int height, int nbLines=1);
      ///
      ~OptionBox();
      ///
      void AddString(const char * );
      ///  always trigger an action
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
      ///
      bool GetIsSelected(const char *) const;
      ///
      bool GetIsSelected(int) const;
      ///
      void SetIsSelected(const char *, bool value);
    protected :
      StringArray fStrings;
      Array<bool *> fSelected;
      int fLines;
    };

  }

}

#endif
