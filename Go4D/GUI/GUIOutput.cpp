#include "GUIOutput.h"

namespace Go4D
{


  namespace GUI
  {

#define Sign(x) ((x)==0?0:(abs(x)/(x)))
#define Anim(X, targetX, step) \
  if (X<targetX) \
    { \
    X += step; \
    if (X>targetX) X = targetX; \
    } \
  else if (X>targetX) \
    { \
    X -= step; \
    if (X<targetX) X = targetX; \
    }


    //************************************************************

    String::String(System * system, const char * name, int width, int height)
      : inherited(system, name, width, height)
    {
      fDynamicSize = false;
    }

    String::String(System * system, const char * name, const char * value)
      : inherited(system, name)
    {
      fDynamicSize = true;
      fValue = value;
    }

    String::~String()
    {
    }

    void String::SetValue(const char * value)
    {
      fValue = value;
    }

    ANode * String::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      assert( (x < fWidth && y < fHeight) && (x>=0 && y>=0));
      if (kind==mekClick)
        ClickEvent(x, y);
      return this;
    }

    void String::DrawPass(Image * aImage, real x, real y, real width, real height)
    {
      if (fDynamicSize)
        ComputeSize();
      DrawString(aImage, fValue.c_str(), Trunc(fWidth), x, y, x+width, y+height);
    }

    void String::ComputeSize()
    {
      fWidth = (GetLayout()->Font()->GetDimX())*(fValue.length());
      fHeight = GetLayout()->Font()->GetDimY();
    }

    //************************************************************

    Box::Box(System * system, const char * name, ANode * child, int width, int height, Color color, int margin)
      : inherited(system, name, width, height)
    {
      SetChild(child);
      fTargetMargin = margin;
      fMargin = fTargetMargin;
      fColor = color;
    }

    Box::~Box()
    {
      if (fChild != NULL)
        delete fChild;
    }

    ANode * Box::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      assert( (x < fWidth && y < fHeight) && (x>=0 && y>=0));
      if (fChild == NULL) return this;
      if ( (x>=fMargin) && (y>=fMargin) && (x<fWidth-fMargin) && (y<fHeight-fMargin))
      {
        x = ((x-fMargin)*fChild->Width())/(fWidth-2*fMargin);
        y = ((y-fMargin)*fChild->Height())/(fHeight-2*fMargin);
        return fChild->MouseEvent(kind, mousecode, x, y); // relative position
      }
      return this;
    }

    void Box::DrawPass(Image * aImage, real x, real y, real width, real height)
    {
      InternalDrawIn(aImage, x, y, width, height, false, true);
    }

    void Box::AnimPass(Image * aImage, real x, real y, real width, real height, bool mustRefresh)
    {
      InternalDrawIn(aImage, x, y, width, height, true, mustRefresh);
      while (fMargin != fTargetMargin)
      {
        fMargin += Sign(fTargetMargin-fMargin);
        RefreshEvent();
        InternalDrawIn(aImage, x, y, width, height, true, true);
      }
    }

    void Box::InternalDrawIn(Image * aImage, real x, real y, real width, real height, bool animPass, bool mustRefresh)
    {
      GetDrawer()->Plug(*aImage);
      if (mustRefresh)
        FastBox(aImage, x,y, x+width, y+height, fColor);
      if (fChild == NULL) return;
      if (animPass)
        fChild->AnimPass(aImage, fMargin, fMargin, fWidth-2*fMargin, fHeight-2*fMargin, mustRefresh);
      else
        fChild->DrawPass(aImage, fMargin, fMargin, fWidth-2*fMargin, fHeight-2*fMargin);
    }

    void Box::SetChild(ANode * child)
    {
      fChild = child;
    }

    void Box::SetMargin(int margin)
    {
      fTargetMargin = margin;
      if (!GetLayout()->Animation())
        fMargin = fTargetMargin;
    }

    void Box::SetColor(Color color)
    {
      fColor = color;
    }

    ANode * Box::SearchNode(const char * name) const
    {
      ANode * result = inherited::SearchNode(name);
      if (result != NULL)
        return result;
      else if (fChild != NULL)
        return fChild->SearchNode(name);
      else
        return NULL;
    }

    //************************************************************

    StringList::StringList(System * system, const char * name, int width, int height,
      bool flat, real radius, real decal)
      : inherited(system, name, width, height),
      fStrings(true) // do  aggregate strings
    {
      fSelectedIndex = 0;
      fTargetIndex = 0;
      fFlat = flat;
      fRadius = radius;
      fDecal = decal;
      fStrings.Complete();
    }

    int StringList::MaxStrLen() const
    {
      int result=0;
      for (int i=0; i<fStrings.Count(); i++)
      {
        int len=fStrings[i]->length();
        if (len>result)
          result=len;
      }
      return result;
    }

    int StringList::MaxWidth()
    {
      return (GetLayout()->Font()->GetDimX())*MaxStrLen();
    }

    void StringList::AutoSize()
    {
      fWidth = min(MaxWidth(), FinalImage()->Width());
    }

    real UnY(int i, int selected, int count, real height, real radius, real decal)
    {
      //int base = (i*height)/count;
      //return base;
      real sel = real(selected)+0.5;
      if (count == 0) return 0;
      if (i<sel)
      {
        real factorL = (sel*height)/count;
        // tmp from 0 to 1
        real tmp= real(i)/sel;
        //real factor = -sqrt(1.0-sqr(tmp));
        // factor from 1 to 0
        real factor = sqrt(sqr(decal+radius)-sqr(tmp))-radius;
        real t = sqrt(sqr(decal+radius)-1.0)-radius;
        factor = (factor-t)/(decal-t);
        tmp = -factor*factorL+factorL;

        return tmp;
      }
      else // right
      {
        real factorR = ((count-sel)*height)/count;
        real factorL = ((sel)      *height)/count;
        // tmp from 1 to 0
        real tmp= real(count-i)/real(count-sel);
        //real factor = sqrt(1.0-sqr(tmp));
        // factor from 0 to 1
        real factor = sqrt(sqr(decal+radius)-sqr(tmp))-radius;
        real t = sqrt(sqr(decal+radius)-1.0)-radius;
        factor = (factor-t)/(decal-t);
        tmp = factor*factorR+factorL;

        return tmp;
      }
    }

    ANode * StringList::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      if (kind!=mekClick) return this;
      assert( (x < fWidth && y < fHeight) && (x>=0 && y>=0));
      if (fStrings.Count() == 0) return this;
      int clickIndex;
      if (fFlat)
      {
        clickIndex = Trunc((y*fStrings.Count())/fHeight);
      }
      else
      {
        clickIndex = 0;
        for (int i=0; i<fStrings.Count(); i++)
        {
          real yInf = UnY(i , fSelectedIndex, fStrings.Count(), fHeight, fRadius, fDecal);
          real ySup = UnY(i+1, fSelectedIndex, fStrings.Count(), fHeight, fRadius, fDecal);
          if  ((y>=yInf) && (y<ySup))
            clickIndex = i;
        }
      }
      if (clickIndex == fSelectedIndex)
      {
        Action click(this, "Click");
        GetListener()->Execute(&click, fStrings[fSelectedIndex]->c_str());
      }
      else
        SetSelected(clickIndex);
      return this;
    }

    void StringList::InternalDrawIn(Image *aImage, real x, real y, real width, real height, bool, bool mustRefresh)
    {
      if (!mustRefresh) return;
      if (fStrings.Count()!=0)
      {
        real step = real(fHeight)/real(fStrings.Count());
        for (int i=0; i<fStrings.Count(); i++)
        {
          int sLen = fStrings[i]->length();
          GetWriter()->SetBackground(i==fSelectedIndex?GetLayout()->SelectedColor():GetLayout()->BackgroundColor());
          // mapped with no regard to the string size
          if (fFlat)
            DrawString(aImage, fStrings[i]->c_str(), Trunc(fWidth),
            x,y+step*real(i)*height/fHeight,
            x+width, y+step*real(i+1)*height/fHeight);
          else
            DrawString(aImage, fStrings[i]->c_str(), Trunc(fWidth),
            x,y+(UnY(i, fSelectedIndex, fStrings.Count(), fHeight, fRadius, fDecal)*height)/fHeight,
            x+width, y+(UnY(i+1, fSelectedIndex, fStrings.Count(), fHeight, fRadius, fDecal)*height)/fHeight);
        }
        GetWriter()->SetBackground(GetLayout()->BackgroundColor());
      }
      else
        FastBox(aImage, x, y, x+width, y+height, GetLayout()->FrameColor());
    }

    void StringList::DrawPass(Image *aImage, real x, real y, real width, real height)
    {
      InternalDrawIn(aImage, x, y, width, height, false, true);
    }

    void StringList::AnimPass(Image *aImage, real x, real y, real width, real height, bool mustRefresh)
    {
      InternalDrawIn(aImage, x, y, width, height, true, mustRefresh);
      while (fTargetIndex != fSelectedIndex)
      {
        fSelectedIndex += Sign(fTargetIndex-fSelectedIndex);
        RefreshEvent();
        InternalDrawIn(aImage, x, y, width, height, true, true);
      }
    }

    void StringList::Clear()
    {
      fStrings.Clear();
      fStrings.Complete();
    }

    void StringList::AddString(const char * str)
    {
      fStrings.Add(new apstring(str));
      fStrings.Complete();
    }

    void StringList::SetSelected(int value)
    {
      fTargetIndex = value;
      if (!GetLayout()->Animation())
        fSelectedIndex = fTargetIndex;
      ChangeEvent();
    }

    const char * StringList::GetSelectedText() const
    {
      if (fStrings.Count() > 0)
        return fStrings[fSelectedIndex]->c_str();
      else
        return "";
    }

    void StringList::SetSelectedText(const char * value)
    {
      if (fStrings.Count() > 0)
        *fStrings[fSelectedIndex] = value;
    }

    //************************************************************

    Splitter::Splitter(System * system, const char * name,
      int width, int height, bool vertical, int separation,
      int factor, bool showSeparation)
      : inherited(system, name, width, height)
    {
      fVertical = vertical;
      fSeparation = separation;
      fFactor = factor;
      fTargetFactor = factor;
      fSelected = true;
      fShowSeparation = showSeparation;
      fAlpha = NULL;
      fOmega = NULL;
    }

    Splitter::~Splitter()
    {
      if (fAlpha != NULL)
        delete fAlpha;
      if (fOmega != NULL)
        delete fOmega;
    }

    ANode * Splitter::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      assert( (x < fWidth && y < fHeight) && (x>=0 && y>=0));
      ANode * result = NULL;
      real sep;
      real size = fVertical?fWidth:fHeight;
      if (fSelected)
        sep = fSeparation+(fSeparation*fFactor)/100;
      else
        sep = fSeparation-(fSeparation*fFactor)/100;
      if (!fVertical)
        if (y<=sep)
          if (fSelected)
            result = fAlpha->MouseEvent(kind, mousecode, (x*fAlpha->Width())/fWidth, (y*fAlpha->Height())/(sep));
          else
          {
            if (GetLayout()->TransmitClick())
              result = fAlpha->MouseEvent(kind, mousecode, (x*fAlpha->Width())/fWidth, (y*fAlpha->Height())/(sep));
            SetSelected(!fSelected);
          }
        else
          if (!fSelected)
            result = fOmega->MouseEvent(kind, mousecode, (x*fOmega->Width())/fWidth, ((y-sep)*fOmega->Height())/(fHeight-sep));
          else
          {
            if (GetLayout()->TransmitClick())
              result = fOmega->MouseEvent(kind, mousecode, (x*fOmega->Width())/fWidth, ((y-sep)*fOmega->Height())/(fHeight-sep));
            SetSelected(!fSelected);
          }
      else
        if (x<=sep)
          if (fSelected)
            result = fAlpha->MouseEvent(kind, mousecode, (x*fAlpha->Width())/(sep), (y*fAlpha->Height())/fHeight);
          else
          {
            if (GetLayout()->TransmitClick())
              result = fAlpha->MouseEvent(kind, mousecode, (x*fAlpha->Width())/(sep), (y*fAlpha->Height())/fHeight);
            SetSelected(!fSelected);
          }
        else
          if (!fSelected)
            result = fOmega->MouseEvent(kind, mousecode, ((x-sep)*fOmega->Width())/(fWidth-sep), (y*fOmega->Height())/fHeight);
          else
          {
            if (GetLayout()->TransmitClick())
              result = fOmega->MouseEvent(kind, mousecode, ((x-sep)*fOmega->Width())/(fWidth-sep), (y*fOmega->Height())/fHeight);
            SetSelected(!fSelected);
          }
          assert(result != NULL);
          return result;
    }

    void Splitter::SetSelected(bool value)
    {
      fSelected = !fSelected;
      if (GetLayout()->Animation())
        fFactor = -fTargetFactor;
      ChangeEvent();
    }

    void Splitter::DrawPass(Image *aImage, real x, real y, real width, real height)
    {
      InternalDrawIn(aImage, x, y, width, height, false, true);
    }

    void Splitter::AnimPass(Image *aImage, real x, real y, real width, real height, bool mustRefresh)
    {
      InternalDrawIn(aImage, x, y, width, height, true, mustRefresh);
      while (fTargetFactor != fFactor)
      {
        fFactor += max(fTargetFactor/5,1);
        if (fFactor > fTargetFactor)
          fFactor = fTargetFactor;
        RefreshEvent();
        InternalDrawIn(aImage, x, y, width, height, true, true);
      }
    }

    void Splitter::InternalDrawIn(Image *aImage, real x, real y, real width, real height, bool animPass, bool mustRefresh)
    {
      assert(fAlpha != NULL);
      assert(fOmega != NULL);
      real sep;
      real size = fVertical?fWidth:fHeight;
      if (fSelected)
        sep = fSeparation+(fSeparation*fFactor)/100;
      else
        sep = fSeparation-(fSeparation*fFactor)/100;
      if (!fVertical)
      {
        sep = (sep*height)/fHeight;
        if (animPass)
        {
          fAlpha->AnimPass(aImage,x,y,width, sep, mustRefresh);
          fOmega->AnimPass(aImage,x,y+sep,width, height-sep, mustRefresh);
        }
        else
        {
          fAlpha->DrawPass(aImage,x,y,width, sep);
          fOmega->DrawPass(aImage,x,y+sep,width, height-sep);
        }
        if (fShowSeparation)
          Line(aImage, x,y+sep,x+width,y+sep, GetLayout()->FrameColor());
      }
      else
      {
        sep = (sep*width)/fWidth;
        if (animPass)
        {
          fAlpha->AnimPass(aImage,x,y,sep, height, mustRefresh);
          fOmega->AnimPass(aImage,x+sep,y,width-sep, height, mustRefresh);
        }
        else
        {
          fAlpha->DrawPass(aImage,x,y,sep, height);
          fOmega->DrawPass(aImage,x+sep,y,width-sep, height);
        }
        if (fShowSeparation)
          Line(aImage, x+sep,y, x+sep, y+height, GetLayout()->FrameColor());
      }
    }

    void Splitter::SetAlpha(ANode * node)
    {
      fAlpha = node;
    }

    void Splitter::SetOmega(ANode * node)
    {
      fOmega = node;
    }

    ANode * Splitter::SearchNode(const char * name) const
    {
      ANode * result = inherited::SearchNode(name);
      if (result != NULL)
        return result;
      result = fAlpha->SearchNode(name);
      if (result != NULL)
        return result;
      return fOmega->SearchNode(name);
    }

    //************************************************************

    Switcher::Switcher(System * system, const char * name, int width, int height, bool vertical)
      : inherited(system, name, width, height),
      fNodes(true /*aggregate*/)
    {
      fVertical = vertical;
      fTargetNode = 0;
      fCurrentNode = fTargetNode;
      fSize = 0;
    }

    Switcher::~Switcher()
    {
    }

    void Switcher::AddSubNode(ANode * aNode)
    {
      fNodes.Add(aNode);
      fNodes.Complete();
    }

    int Switcher::GetCurrentNode() const
    {
      return fTargetNode;
    }

    void Switcher::SetCurrentNode(int value)
    {
      fTargetNode = value;
      if (!GetLayout()->Animation())
        fCurrentNode = fTargetNode;
      ChangeEvent();
    }

    int Switcher::NodesCount() const
    {
      return fNodes.Count();
    }

    ANode * Switcher::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      assert(NodesCount() > 0);
      ANode * subNode = fNodes[fCurrentNode];
      return subNode->MouseEvent(kind, mousecode, (x*subNode->Width())/fWidth, (y*subNode->Height())/fHeight);
    }

    void Switcher::DrawPass(Image * aImage, real x, real y, real width, real height)
    {
      InternalDrawIn(aImage, x, y, width, height, false, true);
    }

    void Switcher::AnimPass(Image * aImage, real x, real y, real width, real height, bool mustRefresh)
    {
      InternalDrawIn(aImage, x, y, width, height, true, mustRefresh);
      while ( (fSize != 0) || (fTargetNode != fCurrentNode) )
      {
        if (fTargetNode != fCurrentNode)
          fSize += 1;
        else
          fSize -= 1;
        if (fSize > Trunc(fWidth/2))
        {
          fSize = Trunc(fWidth/2);
          fCurrentNode = fTargetNode;
        }
        if (fSize <= 0)
          fSize = 0;
        RefreshEvent();
        InternalDrawIn(aImage, x, y, width, height, true, true);
      }
    }

    void Switcher::InternalDrawIn(Image * aImage, real x, real y, real width, real height, bool animPass, bool mustRefresh)
    {
      assert(NodesCount() > 0);
      if ( (fSize != 0) && mustRefresh)
      {
        FastBox(aImage, x,y,x+width,y+height, GetLayout()->FrameColor());
      }
      if (animPass)
        fNodes[fCurrentNode]->AnimPass(aImage, x+fSize*width/fWidth, y, width-fSize*2*width/fWidth, height, mustRefresh);
      else
        fNodes[fCurrentNode]->DrawPass(aImage, x+fSize*width/fWidth, y, width-fSize*2*width/fWidth, height);
    }

    ANode * Switcher::SearchNode(const char * name) const
    {
      ANode * result = inherited::SearchNode(name);
      if (result != NULL)
        return result;
      for (int i = 0; i<fNodes.Count(); i++)
      {
        result = fNodes[i]->SearchNode(name);
        if (result != NULL)
          return result;
      }
      return NULL;
    }

    //************************************************************

    ToolBox::ToolBox(System * system, const char * name, int width, int height, bool showSelected)
      : inherited(system, name, width, height)
    {
      fShowSelected = showSelected;
      fSelectedIndex = -1;
    }

    ToolBox::~ToolBox()
    {
    }

    void ToolBox::AddString(const char * btn)
    {
      fStrings.Add(btn);
      fStrings.Complete();
    }

    ANode * ToolBox::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      if (kind != mekClick) return this;
      assert( (x < fWidth && y < fHeight) && (x>=0 && y>=0));
      assert(fStrings.Count() >0);
      int count = fStrings.Count();
      fSelectedIndex = Trunc((x*count)/fWidth);
      Action click(this, "Click");
      GetListener()->Execute(&click, fStrings[fSelectedIndex]);
      return this;
    }

    void ToolBox::DrawPass(Image * aImage, real x, real y, real width, real height)
    {
      assert(fStrings.Count() >0);
      int count = fStrings.Count();
      real size = width / count;
      for (int i=0; i<count; i++)
      {
        if (fShowSelected)
          GetWriter()->SetBackground( i==fSelectedIndex?GetLayout()->SelectedColor():GetLayout()->BackgroundColor() );
        DrawString(aImage, fStrings[i], Trunc(fWidth/count),
          x+real(i)*size, y, x+real(i+1)*size, y+height);
      }
      GetWriter()->SetBackground(GetLayout()->BackgroundColor());
    }

    //************************************************************

    TreeNode::TreeNode(TreeNode * parent)
      : fChilds(true)
    {
      fParent = parent;
      if (fParent != NULL)
        fParent->AddChild(this);
    }

    TreeNode::~TreeNode()
    {
    }

    TreeNode * TreeNode::GetParent() const
    {
      return fParent;
    }

    bool TreeNode::IsParent(const TreeNode * anode) const
    {
      if (anode == this)
        return true;
      else if (fParent == NULL)
        return false;
      else
        return fParent->IsParent(anode);
    }

    int TreeNode::ChildCount() const
    {
      return fChilds.Count();
    }

    int TreeNode::TotalCount() const
    {
      int result=1;
      for (int i=0; i<fChilds.Count(); i++)
        result += fChilds[i]->TotalCount();
      return result;
    }

    real TreeNode::ComputeWeight(real childFactor, real relativeFactor)
    {
      real result=0;
      for (int i=0; i<fChilds.Count(); i++)
        result += fChilds[i]->ComputeWeight(childFactor, relativeFactor)*childFactor;
      fWeight = 1.0+result*relativeFactor;
      fWeightTotal = fWeight+result;
      result = fWeightTotal;
      return result;
    }

    real TreeNode::WeightRelative() const
    {
      return fWeight;
    }

    real TreeNode::WeightTotal() const
    {
      return fWeightTotal;
    }

    TreeNode * TreeNode::GetChild(int index)
    {
      return fChilds[index];
    }

    void TreeNode::AddChild(TreeNode * child)
    {
      fChilds.Add(child);
    }

    void TreeNode::DeleteChild(TreeNode * child)
    {
      fChilds.Del(child);
    }

    const char * TreeNode::Value()
    {
      return fValue.c_str();
    }

    void TreeNode::SetValue(const char * value)
    {
      fValue = value;
    }

    void TreeNode::SetComplete()
    {
      fChilds.Complete();
      for (int i=0; i<fChilds.Count(); i++)
        fChilds[i]->SetComplete();
    }

    //************************************************************

    Tree::Tree(System * system, const char * name, int width, int height,
      bool showRoot, real childFactor, real relativeFactor, int maxDepth)
      : inherited(system, name, width, height)
    {
      fShowRoot = showRoot;
      fRoot = NULL;
      fDisplayRoot = NULL;
      fSelected = NULL;
      fTargetDisplayRoot = NULL;
      fTargetChildFactor = childFactor;
      fChildFactor = fTargetChildFactor;
      fTargetRelativeFactor = relativeFactor;
      fRelativeFactor = fTargetRelativeFactor;
      fMaxDepth = maxDepth;
    }

    Tree::~Tree()
    {
      Clear();
    }

    void Tree::SetRootNode(TreeNode * root)
    {
      fRoot = root;
      fSelected = root; // by default
      fTargetDisplayRoot = root;
      fDisplayRoot = root;
    }

    void Tree::Clear()
    {
      delete fRoot;
      fRoot = NULL;
      fDisplayRoot = NULL;
      fSelected = NULL;
      fTargetDisplayRoot = NULL;
    }

    TreeNode * Tree::GetRootNode() const
    {
      return fRoot;
    }

    void Tree::SetSelectedAsDisplayRoot()
    {
      if (fSelected == NULL) return;
      if ( (fSelected == fDisplayRoot) || (fSelected->ChildCount()==0) )
      {
        if (fSelected->GetParent()==NULL)
          SetDisplayRoot(fSelected);
        else
          SetDisplayRoot(fSelected->GetParent());
      }
      else
        SetDisplayRoot(fSelected);
    }

    ANode * Tree::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      if (kind==mekClick)
      {
        bool result = RecursiveClick(x, y, fDisplayRoot, fShowRoot);
        assert(result); // assert have find where click occurs
      }
      return this;
    }

    bool Tree::RecursiveClick(real x, real y, TreeNode * node, bool showRoot)
    {
      assert(x>=0 && y>=0 && x<fWidth && y<fHeight);
      real weight=node->WeightTotal();
      real offs=node->WeightRelative();
      if (showRoot)
      {
        if (y<=(offs*fHeight)/weight)
        {
          if (fSelected == node)
          {
            if (ClickEvent(x, y) < 0)
              SetSelectedAsDisplayRoot();
            return true;
          }
          else
          {
            SetSelected(node);
            return true;
          }
        }
      }
      else
      {
        weight-=offs;
        offs=0;
      }
      for (int i=0; i<node->ChildCount(); i++)
      {
        real next=node->GetChild(i)->WeightTotal()*fChildFactor;
        if ( (y>=(offs*fHeight)/weight) && (y<((offs+next)*fHeight)/weight) )
          if ( RecursiveClick(x,
            (y*weight-offs*fHeight)/next,
            node->GetChild(i), true) )
            return true;
        offs+=next;
        assert(offs <= weight);
      }
      return false;
    }

    void Tree::GoUp()
    {
      if (fDisplayRoot->GetParent() != NULL)
        fDisplayRoot = fDisplayRoot->GetParent();
    }

    int Tree::GoDown()
    {
      for (int i=0; i<fDisplayRoot->ChildCount(); i++)
        if (fSelected->IsParent(fDisplayRoot->GetChild(i)))
        {
          fDisplayRoot = fDisplayRoot->GetChild(i);
          return i;
        }
      return -1;
    }

    int Tree::SearchDown()
    {
      for (int i=0; i<fDisplayRoot->ChildCount(); i++)
        if (fSelected->IsParent(fDisplayRoot->GetChild(i)))
          return i;
      return -1;
    }

    void Tree::DrawPass(Image *aImage, real x, real y, real width, real height)
    {
      FastBox(aImage, x, y, x+width, y+height, GetLayout()->BackgroundColor());
      RecursiveDrawIn(aImage, x, y, width, height, fDisplayRoot, 1.0, fShowRoot, false, true, 1);
      GetWriter()->SetBackground(GetLayout()->BackgroundColor());
    }

    void Tree::AnimPass(Image *aImage, real x, real y, real width, real height, bool mustRefresh)
    {
      if (mustRefresh)
        FastBox(aImage, x, y, x+width, y+height, GetLayout()->BackgroundColor());
      RecursiveDrawIn(aImage, x, y, width, height, fDisplayRoot, 1.0, fShowRoot, true, mustRefresh, 1);
      while (
        (fTargetChildFactor != fChildFactor)
        || (fTargetRelativeFactor != fRelativeFactor)
        || (fTargetDisplayRoot != fDisplayRoot ) )
      {
        Anim(fChildFactor, fTargetChildFactor, 0.01);
        Anim(fRelativeFactor, fTargetRelativeFactor, 0.01);
        SetComplete();
        if (fTargetDisplayRoot != fDisplayRoot)
          if (fDisplayRoot->IsParent(fTargetDisplayRoot))
          {
            GoUp();
            FastBox(PushImage(), x, y, x+width, y+height, GetLayout()->BackgroundColor());
            RecursiveDrawIn(LastImage(), x, y, width, height, fDisplayRoot, 1.0, fShowRoot, true, true, 1);
            real weight=fDisplayRoot->WeightTotal();
            real offs=fDisplayRoot->WeightRelative();
            real next=0.0;
            if (!fShowRoot)
            {
              weight -= offs;
              offs = 0;
            }
            int zoomon = SearchDown();
            for (int i=0; i<=zoomon; i++)
            {
              offs+=next;
              next=fDisplayRoot->GetChild(i)->WeightTotal()*fChildFactor;
            }
            real zoom_xmin = x+5.0;
            real zoom_ymin = y+(height*offs)/weight;
            real zoom_xmax = zoom_xmin+width-5.0;
            real zoom_ymax = zoom_ymin+(height*next)/weight;

            #define NSTEP 50
            real step_xmin = (zoom_xmin-x)/NSTEP;
            real step_ymin = (zoom_ymin-y)/NSTEP;
            real step_xmax = (zoom_xmax-(x+width))/NSTEP;
            real step_ymax = (zoom_ymax-(y+height))/NSTEP;
            GetDrawer()->Plug(*aImage);
            for (int a=NSTEP; a>0; a--)
            {
              GetDrawer()->MapBox(*LastImage(),
                Trunc(x+a*step_xmin), Trunc(y+a*step_ymin), Trunc(x+width+a*step_xmax), Trunc(y+height+a*step_ymax),
                Trunc(x), Trunc(y), Trunc(x+width), Trunc(y+height));
              RefreshEvent();
            }
            PopImage();

          }
          else
          {
            FastBox(PushImage(), x, y, x+width, y+height, GetLayout()->BackgroundColor());
            RecursiveDrawIn(LastImage(), x, y, width, height, fDisplayRoot, 1.0, fShowRoot, true, true, 1);
            real weight=fDisplayRoot->WeightTotal();
            real offs=fDisplayRoot->WeightRelative();
            real next=0.0;
            if (!fShowRoot)
            {
              weight -= offs;
              offs = 0;
            }
            int zoomon = SearchDown();
            for (int i=0; i<=zoomon; i++)
            {
              offs+=next;
              next=fDisplayRoot->GetChild(i)->WeightTotal()*fChildFactor;
            }
            real zoom_xmin = x+5.0;
            real zoom_ymin = y+(height*offs)/weight;
            real zoom_xmax = zoom_xmin+width-5.0;
            real zoom_ymax = zoom_ymin+(height*next)/weight;

            #define NSTEP 50
            real step_xmin = (zoom_xmin-x)/NSTEP;
            real step_ymin = (zoom_ymin-y)/NSTEP;
            real step_xmax = (zoom_xmax-(x+width))/NSTEP;
            real step_ymax = (zoom_ymax-(y+height))/NSTEP;
            GetDrawer()->Plug(*aImage);
            for (int a=0; a<NSTEP; a++)
            {
              GetDrawer()->MapBox(*LastImage(),
                Trunc(x+a*step_xmin), Trunc(y+a*step_ymin), Trunc(x+width+a*step_xmax), Trunc(y+height+a*step_ymax),
                Trunc(x), Trunc(y), Trunc(x+width), Trunc(y+height));
              RefreshEvent();
            }
            PopImage();

            GoDown();
          }
        RefreshEvent();
        FastBox(aImage, x, y, x+width, y+height, GetLayout()->BackgroundColor());
        RecursiveDrawIn(aImage, x, y, width, height, fDisplayRoot, 1.0, fShowRoot, true, true, 1);
      }
      GetWriter()->SetBackground(GetLayout()->BackgroundColor());
    }

    void Tree::RecursiveDrawIn(Image * aImage, real x, real y, real width, real height, TreeNode * node, real factor, bool showRoot, bool animPass, bool mustRefresh, int depth)
    {
      if (!mustRefresh) return;
      if (depth>fMaxDepth) return;
      real weight=node->WeightTotal();
      real offs=node->WeightRelative();
      if (showRoot)
      {
        GetWriter()->SetBackground(node==fSelected?GetLayout()->SelectedColor():GetLayout()->BackgroundColor());
        //DrawString(aImage, node->Value(), Trunc(fWidth),
        DrawString(aImage, node->Value(), StrLen(node->Value())*GetLayout()->Font()->GetDimX(),
          x, y, x+width*factor, y+height*offs/weight );
      }
      else
      {
        weight -= offs;
        offs = 0;
      }
      for (int i=0; i<node->ChildCount(); i++)
      {
        real next=node->GetChild(i)->WeightTotal()*fChildFactor;
        RecursiveDrawIn(aImage, x+5.0, y+(height*offs)/weight,
          width-5.0, (height*next)/weight,
          node->GetChild(i), factor*sqrt(fChildFactor), true, animPass, mustRefresh, depth+1);
        offs+=next;
        // precision problems can occurs
        //assert(offs <= weight);
      }
    }

    void Tree::SetDisplayRoot(TreeNode * root)
    {
      fTargetDisplayRoot = root;
      if (!GetLayout()->Animation())
        fDisplayRoot = fTargetDisplayRoot;
    }

    TreeNode * Tree::GetSelected() const
    {
      return fSelected;
    }

    void Tree::SetSelected(TreeNode * node)
    {
      fSelected = node;
      ChangeEvent();
    }

    void Tree::DeleteSelected()
    {
      if (fSelected->GetParent() == NULL)
      {
        assert(fSelected = fRoot);
        delete fSelected;
        fRoot = NULL;
        SetDisplayRoot(NULL);
      }
      else
      {
        TreeNode * parent = fSelected->GetParent();
        if (fDisplayRoot == fSelected)
          SetDisplayRoot(parent);
        parent->DeleteChild(fSelected);
        SetSelected(parent);
      }
    }

    void Tree::SetChildFactor(real childFactor)
    {
      fTargetChildFactor = childFactor;
      if (!GetLayout()->Animation())
        fChildFactor = fTargetChildFactor;
    }

    void Tree::SetRelativeFactor(real relativeFactor)
    {
      fTargetRelativeFactor = relativeFactor;
      if (!GetLayout()->Animation())
        fRelativeFactor = fTargetRelativeFactor;
    }

    void Tree::SetComplete()
    {
      fRoot->SetComplete();
      fRoot->ComputeWeight(fChildFactor, fRelativeFactor);
    }

    ImageFrame::ImageFrame(System * system, const char * name, Image * image)
      : inherited(system, name)
    {
      fImage = image;
      fWidth = 0;
      fHeight = 0;
    }

    ImageFrame::~ImageFrame()
    {
    }

    ANode * ImageFrame::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      assert( (x < fWidth && y < fHeight) && (x>=0 && y>=0));
      ClickEvent(x, y);
      return this;
    }

    void ImageFrame::DrawPass(Image *aImage, real x, real y, real width, real height)
    {
      if (fImage == NULL)
        return;
      GetDrawer()->Plug(*aImage);
      GetDrawer()->MapBox(*fImage, 0,0, fImage->Width()-1, fImage->Height()-1,
        Trunc(x), Trunc(y), Trunc(width), Trunc(height));
    }

    void ImageFrame::SetImage(Image * value)
    {
      fImage = value;
      fWidth = fImage->Width();
      fHeight = fImage->Height();
    }


    //******************************************************************

    AsciiTable::AsciiTable(System * system, const char * name, int width ,int height)
      : inherited(system, name, width, height)
    {
    }

    AsciiTable::~AsciiTable()
    {
    }

    ANode * AsciiTable::MouseEvent(MouseEventKind kind, int mousecode, real x, real y)
    {
      return this;
    }

    void AsciiTable::DrawPass(Image *aImage, real x, real y, real width, real height)
    {
      for (int j=0; j<16; j++)
      {
        int i;
        char str[17];
        for (i=0; i<16; i++)
          str[i] = i+j*16;
        str[i]=0;
        DrawString(aImage, str, 16*GetLayout()->Font()->GetDimX(),
          x, y+j*height/16, x+width, y+(j+1)*height/16);
      }
    }


  } // GUI

} // Go4D
