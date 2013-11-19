#ifndef _GUI_H_
#define _GUI_H_

/**
*AUTHOR : Babel yoann
*TITLE  : GUI.h, 3D GUI System
*CREATED: 31/12/00
*DATE   : 31/12/00
*OBJECT : allow to create 3D guis.
*/

#include "Tools/Common.h"
#include "Tools/List.h"
#include "Tools/apstring.h"
#include "Graph2D/Font.h"
#include "Graph2D/Drawer2D.h"
#include "Graph2D/WriteDrawer.h"
#include "Graph2D/Palette.h"

namespace Go4D
{


  namespace GUI
  {

    class System; // forward
    class ANode; // forward

    /**
    * an action is triggered by the gui and contain
    * parameters that allow to implement the correct
    * behaviour
    */
    class Action
    {
    public :
      ///  sender is the component that raise the action, name is the id of the component
      Action(ANode * sender, const char * name) { fName = name; fSender = sender; }
      ///
      const char * Name() const { return fName; }
      ///
      ANode * Sender() const { return fSender; }

    protected :
      const char * fName;
      ANode * fSender;
    };

    /**
    * execute : called back laucnhed when a event is raised in the GUI
    * most of the time, when an area is clicked
    */
    class ActionListener
    {
    public :
      ///
      virtual int Execute(Action * action, const char * NodeName)=0;
    };

    /**
    * this class contains all the information that determine the
    * the style of the gui. The colors used to display the nodes
    * and other stuff like that
    */
    class Layout
    {
    public :
      ///
      Layout(AFont * font, Color fColor, Color bColor, Color sColor, bool keepProportion, bool animation)
      { fFont = font; fForegroundColor=fColor; fBackgroundColor=bColor; fSelectedColor=sColor; fKeepProportion=keepProportion; fAnimation = animation; }
      ///  default layout
      Layout(AFont *, bool keepProportion=true, bool animation=true);
      ///  Search the color in the palette for each layout color
      void ImportPalette(const Palette &);
      ///
      inline AFont * Font() { return fFont; }
      ///
      inline Color ForegroundColor() { return fForegroundColor; }
      ///
      inline Color BackgroundColor() { return fBackgroundColor; }
      ///
      inline Color SelectedColor() { return fSelectedColor; }
      ///
      inline Color FrameColor() { return fFrameColor; }
      ///
      inline bool KeepProportion() { return fKeepProportion; }
      ///
      inline bool Animation() { return fAnimation; }
      ///
      inline int AnimSteps() { return 10; } // by default
      ///
      inline bool TransmitClick() { return true; }
    protected :
      AFont * fFont;
      Color fForegroundColor;
      Color fBackgroundColor;
      Color fSelectedColor;
      Color fFrameColor;
      RGBColor fRGBForegroundColor;
      RGBColor fRGBBackgroundColor;
      RGBColor fRGBSelectedColor;
      RGBColor fRGBFrameColor;
      bool fKeepProportion;
      bool fAnimation;
    };

    /**
    * this is the main class of the GUI. It's the container of
    * all the nodes of the gui. It delegates the displaying to
    * each node and transfer mouse and keys events
    */
    class System
    {
      friend class ANode;
    public:
      ///
      System(Image *, ANode * root, Layout * layout, ActionListener *listener=NULL);
      ///
      ~System();
      ///  set the main node of the GUI
      void SetRoot(ANode * root);
      ///  display the GUI in the main image
      void Display();
      ///  transfer a click to the nodes. If a node is clicked it
      ///  can trigger the click action
      void SendClick(int mousecode, int x, int y);
      /// transfer a Drag&drop
      void SendStartDrag(int mousecode, int x, int y);
      /// transfer a Drag&drop
      void SendDragOver(int mousecode, int dx, int dy);
      /// transfer a Drag&drop
      void SendEndDrag(int mousecode, int x, int y);
      /// transfer a Drag&drop
      ///  search a node in the hierarchy
      ANode * SearchNode(const char * name) const;
      ///
      void RefreshNode(ANode * );
      ///
      void SetActionListener(ActionListener *);
      ///
      ANode * GetDragStartNode() const { return fDragStartNode; }
      ///
      ANode * GetDragEndNode() const { return fDragEndNode; }

    protected:
      Array<Image *> fPrepareImages; // this image is used to prepare drawing
      Image * fFinalImage;
      int fCurrentImageIndex;
      Image * PushImage();
      Image * LastImage();
      void PopImage();
      Layout * fLayout;
      ANode * fRootNode;
      ANode * fDragStartNode;
      ANode * fDragEndNode;
      ActionListener * fListener;

      // Stored here in order to limit memory usage
      Writer fWriter;
      Drawer2D fDrawer;
    };

    enum MouseEventKind {mekClick, mekDragStart, mekDragOver, mekDragEnd};

    /**
    * This abstract class define the basic services that
    * a displayable node of the gui must implements
    * the visual size of the node depends on where and how it is
    * displayed (due to the 3D interface, the distance of an
    * element is variable, and so the size varies also)
    */
    class ANode
    {
    public:
      ///
      ANode(System * system, const char * name);
      ///
      ANode(System * system, const char * name, int width, int height);
      ///
      inline int Width() const { return Trunc(fWidth); }
      ///
      inline int Height() const { return Trunc(fHeight); }
      ///
      inline void SetWidth(int value) { fWidth = real(value); }
      ///
      inline void SetHeight(int value) { fHeight = real(value); }
      ///
      inline const char * Name() const { return fName; }
      /// return the node on which the event is actually handled
      virtual ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y)=0;
      ///
      virtual void DrawPass(Image *, real x, real y, real width, real height);
      ///
      virtual void AnimPass(Image *, real x, real y, real width, real height, bool mustRefresh);
      ///
      virtual ANode * SearchNode(const char * name) const;

    protected:
      const char * fName;
      System * fSystem;
      real fWidth;
      real fHeight;

    protected :
      Image * FinalImage() { return fSystem->fFinalImage; }
      Image * PushImage() { return fSystem->PushImage(); }
      Image * LastImage() { return fSystem->LastImage(); }
      void PopImage() { fSystem->PopImage(); }
      Layout * GetLayout() { return fSystem->fLayout; }
      ANode * RootNode() { return fSystem->fRootNode; }
      ActionListener * GetListener() { return fSystem->fListener; }
      Drawer2D * GetDrawer() { return &(fSystem->fDrawer); }
      Writer * GetWriter() { return &(fSystem->fWriter); }
      /// call this to launch a "Click" event
      int ClickEvent(real x, real y);
      /// call this to signal an internal value of the node have change
      int ChangeEvent();
      /// call this to request a display refreshing, used for animated layouts
      int RefreshEvent();

    protected :
      void DrawString(Image * , const char *, int proportionSize, real xmin, real ymin, real xmax, real ymax);
      void FastBox(Image *, real xmin, real ymin, real xmax, real ymax, Color color);
      void Line(Image *, real xmin, real ymin, real xmax, real ymax, Color color);
    };

    ///
    typedef Array<ANode *> NodeArray;

  }

}
#endif
