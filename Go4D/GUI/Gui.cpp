/**
*AUTHOR : Babel yoann
*TITLE  : GUI.cpp, GUI.h implementation
*CREATED: 31/12/00
*DATE   : 14/01/00
*BUGS   : the click on the Tree is a bit fuzzy. When an click occurs
*         on the upper part of a string, it send the click on the
*         upper node !! maybe it's a precision problem, but I'm not
*         sure...
*/

#include "Gui.h"

namespace Go4D
{


  namespace GUI
  {

    //************************************************************

    Layout::Layout(AFont *font, bool keepProportion, bool animation)
    {
      fFont = font;
      fKeepProportion = keepProportion;
      fAnimation = animation;
      fRGBBackgroundColor = RGBColor(255,255,255); // white
      fRGBForegroundColor = RGBColor(0,0,0); // black
      fRGBSelectedColor = RGBColor(128,128,255); // light-blue
      fRGBFrameColor = RGBColor(128,128,128); // grey
    }

    void Layout::ImportPalette(const Palette & pal)
    {
      fBackgroundColor = Color(pal.Best(fRGBBackgroundColor));
      fForegroundColor = Color(pal.Best(fRGBForegroundColor));
      fSelectedColor = Color(pal.Best(fRGBSelectedColor));
      fFrameColor = Color(pal.Best(fRGBFrameColor));
    }

    //************************************************************

    System::System(Image * image, ANode *  root, Layout * layout, ActionListener * listener)
      : fFinalImage(image),
      fPrepareImages(true), // aggregate !
      fWriter(0,0, false, 0/*border*/, true/*draw background*/, layout->BackgroundColor()),
      fDrawer(0,0, false)
    {
      fPrepareImages.Complete();
      fRootNode = root;
      fLayout = layout;
      fListener = listener;
      fCurrentImageIndex = -1;
      fDragStartNode = NULL;
      fDragEndNode = NULL;
    }

    System::~System()
    {
      delete fRootNode;
    }

    void System::SetRoot(ANode * root)
    {
      fRootNode = root;
    }

    void System::SetActionListener(ActionListener * listener)
    {
      fListener = listener;
    }

    void System::Display()
    {
      fRootNode->DrawPass(fFinalImage, 0,0, fFinalImage->Width(), fFinalImage->Height());
      fRootNode->AnimPass(fFinalImage, 0,0, fFinalImage->Width(), fFinalImage->Height(), false);
    }

    void System::SendClick(int mousecode, int x, int y)
    {
      x = (x*fRootNode->Width())/fFinalImage->Width();
      y = (y*fRootNode->Height())/fFinalImage->Height();
      fRootNode->MouseEvent(mekClick, mousecode, x, y);
    }

    void System::SendStartDrag(int mousecode, int x, int y)
    {
      x = (x*fRootNode->Width())/fFinalImage->Width();
      y = (y*fRootNode->Height())/fFinalImage->Height();
      fDragStartNode = fRootNode->MouseEvent(mekDragStart, mousecode, x, y);
    }

    void System::SendDragOver(int mousecode, int x, int y)
    {
      assert(fDragStartNode != NULL);
      fDragStartNode->MouseEvent(mekDragOver, mousecode, x, y);
    }

    void System::SendEndDrag(int mousecode, int x, int y)
    {
      x = (x*fRootNode->Width())/fFinalImage->Width();
      y = (y*fRootNode->Height())/fFinalImage->Height();
      ANode * fDragEndNode = fRootNode->MouseEvent(mekDragEnd, mousecode, x, y);
      Action dragdrop(fDragEndNode, "DragDrop");
      fListener->Execute(&dragdrop, "System");
    }

    Image * System::PushImage()
    {
      if (fCurrentImageIndex >= fPrepareImages.Count()-1 )
      {
        fPrepareImages.Add(new Image(fFinalImage->Width(), fFinalImage->Height(), true));
        fPrepareImages.Complete();
      }
      return fPrepareImages[++fCurrentImageIndex];
    }

    Image * System::LastImage()
    {
      return fPrepareImages[fCurrentImageIndex];
    }

    void System::PopImage()
    {
      --fCurrentImageIndex;
      // dot not desallocate, keep images to optimize allocation
    }

    ANode * System::SearchNode(const char * name)  const
    {
      return fRootNode->SearchNode(name);
    }

    void System::RefreshNode(ANode * node)
    {
      /// TODO : make the refresh of only the requested node
      Display();
    }


    //************************************************************

    ANode::ANode(System * system, const char * name)
    {
      fSystem = system; fName = name;
    }

    ANode::ANode(System * system, const char * name, int width, int height)
    {
      fSystem = system;
      fName = name;
      fWidth = real(width);
      fHeight = real(height);
    }

    void ANode::DrawPass(Image* image, real x, real y, real width, real height)
    {
      GetDrawer()->Plug(*image);
      GetDrawer()->MapBox(*LastImage(), 0,0,Trunc(fWidth), Trunc(fHeight), Trunc(x), Trunc(y), Trunc(x+width), Trunc(y+height));
    }

    void ANode::DrawString(Image * aImage, const char * data, int proportionSize, real xmin, real ymin, real xmax, real ymax)
    {
      /*GetWriter()->Plug(* PushImage());
      GetDrawer()->Plug(*aImage);
      GetWriter()->WriteLine(
        PointZ(0,0, GetLayout()->ForegroundColor()),
        *GetLayout()->Font(),
        data);
      GetDrawer()->MapBox(*LastImage(), 0, 0,
        GetLayout()->KeepProportion()?proportionSize:StrLen(data)*GetLayout()->Font()->GetDimX(), GetLayout()->Font()->GetDimY(),
        Trunc(xmin), Trunc(ymin), Trunc(xmax), Trunc(ymax));
      PopImage();*/
      GetWriter()->Plug(*aImage);
      GetWriter()->MapString(data, *GetLayout()->Font(),
        GetLayout()->KeepProportion()?proportionSize:StrLen(data)*GetLayout()->Font()->GetDimX(),
        Trunc(xmin), Trunc(ymin), Trunc(xmax), Trunc(ymax), GetLayout()->ForegroundColor());
    }

    void ANode::FastBox(Image *aImage, real xmin, real ymin, real xmax, real ymax, Color color)
    {
      GetDrawer()->Plug(*aImage);
      GetDrawer()->FastBox(Trunc(xmin), Trunc(ymin), Trunc(xmax), Trunc(ymax), color);
    }

    void ANode::Line(Image *aImage, real xmin, real ymin, real xmax, real ymax, Color color)
    {
      GetDrawer()->Plug(*aImage);
      GetDrawer()->Line(Trunc(xmin), Trunc(ymin), Trunc(xmax), Trunc(ymax), color);
    }

    void ANode::AnimPass(Image* image, real x, real y, real width, real height, bool mustRefresh)
    {
      if (mustRefresh)
        DrawPass(image, x, y, width, height);
    }

    int ANode::RefreshEvent()
    {
      Action refresh(this, "Refresh");
      return GetListener()->Execute(&refresh, fName);
    }

    ANode * ANode::SearchNode(const char * name) const
    {
      ANode * result = SameStr(name, fName)?const_cast<ANode *>(this):NULL;
      return result;
    }

    int ANode::ClickEvent(real x, real y)
    {
      Action click(this, "Click");
      return GetListener()->Execute(&click, fName);
    }

    int ANode::ChangeEvent()
    {
      Action change(this, "Change");
      return GetListener()->Execute(&change, fName);
    }

  }

}
