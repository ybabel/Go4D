#ifndef _GUI_OUTPUT_H_
#define _GUI_OUTPUT_H_

/**
*AUTHOR : Babel yoann
*TITLE  : GUIOutput.h, output nodes
*CREATED: 21/01/01
*DATE   : 21/02/01
*OBJECT : contains some nodes to display objects
*/

#include "Gui.h"

namespace Go4D
{


  namespace GUI
  {

    /**
    * This class is used to display a string. The size can be fixed
    * or it can be computed depending on the size of the string
    */
    class String : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      String(System * system, const char * name, int width, int height);
      ///
      String(System * system, const char * name, const char * value);
      ///
      ~String();
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
      ///
      void SetValue(const char * value);

    protected :
      void ComputeSize();
      bool fDynamicSize; // is the size to be recomputed each time the value change ?
      apstring fValue;
    };

    /**
    * a box is just a container with a margin, the sub elements are
    * displayed inside the frame
    */
    class Box : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      Box(System * system, const char * name, ANode * child, int width, int height, Color color, int margin=1);
      ///
      ~Box();
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
      ///
      void AnimPass(Image *, real x, real y, real width, real height, bool mustRefresh);
      ///
      void SetChild(ANode * child);
      ///
      ANode * GetChild() { return fChild; }
      ///
      ANode * SearchNode(const char * name) const;
      ///
      void SetMargin(int margin);
      ///
      void SetColor(Color );

    protected :
      ANode * fChild;
      int fMargin;
      int fTargetMargin;
      Color fColor;
      void InternalDrawIn(Image *, real x, real y, real width, real height, bool animPass, bool mustRefresh);
    };

    /**
    * this gui node can display a string list. The
    * selected element is enhanced by the selection color, and
    * also because the selected element is bigger than the other
    * a spherical deformation is used
    */
    class StringList : public ANode
    {
      typedef ANode inherited;
    public:
      ///
      StringList(System * system, const char * name, int width, int height,
        bool flat=true, real radius=0.0, real decal=1.0);
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
      ///
      void AnimPass(Image *, real x, real y, real width, real height, bool mustRefresh);
      ///
      void Clear();
      ///
      void AddString(const char *);
      ///
      int GetSelected() const { return fTargetIndex; }
      ///
      void SetSelected(int value);
      ///
      const char * GetSelectedText() const;
      ///
      void SetSelectedText(const char *);
      ///
      int MaxStrLen() const;
      ///
      int MaxWidth();
      ///
      void AutoSize();

    protected :
      Array<apstring *> fStrings;
      int fSelectedIndex;
      int fTargetIndex;
      real fRadius;
      real fDecal;
      bool fFlat;
      void InternalDrawIn(Image *, real x, real y, real width, real height, bool animPass, bool mustRefresh);
    };

    /**
    * It separate 2 panels, vertically or horizontally. The size
    * of the panels depends on wich of them is selected, the
    * deformation factor can be fixed
    */
    class Splitter : public ANode
    {
      typedef ANode inherited;
    public:
      ///
      Splitter(System * system, const char * name,
        int width, int height, bool vertical, int separation,
        int factor, bool showSeparation = true);
      ///
      ~Splitter();
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
      ///
      void AnimPass(Image *, real x, real y, real width, real height, bool mustRefresh);
      ///
      void SetAlpha(ANode * );
      ///
      void SetOmega(ANode * );
      ///
      ANode * SearchNode(const char * name) const;
      ///
      void SetSelected(bool);
      ///
      bool GetSelected() const { return fSelected; }

    protected :
      ANode * fAlpha;
      ANode * fOmega;
      bool fVertical;
      int fSeparation;
      int fFactor;
      int fTargetFactor;
      bool fSelected; //true=alpha, false=omega
      bool fShowSeparation;
      void InternalDrawIn(Image *, real x, real y, real width, real height, bool animPass, bool mustRefresh);
    };

    /**
    * The switcher allows to switch between many panels. Only ONE is displayed at once.
    * it's like a tabsheet.
    */
    class Switcher : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      Switcher(System * system, const char * name, int width, int height, bool vertical);
      ///
      ~Switcher();
      ///
      void AddSubNode(ANode *);
      ///
      int GetCurrentNode() const;
      ///
      void SetCurrentNode(int);
      ///
      int NodesCount() const;
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
      ///
      void AnimPass(Image *, real x, real y, real width, real height, bool mustRefresh);
      ///
      ANode * SearchNode(const char * name) const;

    protected :
      NodeArray fNodes;
      int fCurrentNode;
      int fSize; // in percents of the witdh
      int fTargetNode;
      bool fVertical;
      void InternalDrawIn(Image *, real x, real y, real width, real height, bool animPass, bool mustRefresh);
    };

    /**
    * This class just contains an horizontal list of strings or images. Images are
    * not implemented yet.
    */
    class ToolBox : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      ToolBox(System * system, const char * name, int width, int height, bool showSelected = true);
      ///
      ~ToolBox();
      ///
      void AddString(const char * );
      ///  always trigger an action
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
      ///
      int GetSelectedIndex() const { return fSelectedIndex; }
    protected :
      Array<const char *> fStrings;
      bool fShowSelected ;
      int fSelectedIndex;
    };

    ///  a node displayed in a tree
    class TreeNode
    {
      friend class Tree;
    public :
      ///
      TreeNode(TreeNode * parent=NULL);
      ///
      ~TreeNode();
      ///
      TreeNode * GetParent() const;
      ///
      bool IsParent(const TreeNode * ) const;
      ///
      int ChildCount() const;
      ///  count all the childs and sub childs recursively + itself
      int TotalCount() const;
      ///  weight is relativ to it's brothers, the sum of wieght of all brothers is 1
      ///  childs are less "important" than parents, by a factor of childFactor importance
      real WeightRelative() const;
      ///
      real WeightTotal() const;
      ///
      TreeNode * GetChild(int index);
      ///
      void AddChild(TreeNode *);
      ///
      void DeleteChild(TreeNode * );
      ///
      const char * Value();
      ///
      void SetValue(const char * value);
      ///
      void * CustomData;

    protected :
      apstring fValue;
      TreeNode * fParent;
      real fWeightTotal;
      real fWeight;
      Array<TreeNode *> fChilds;
      void SetComplete();
      real ComputeWeight(real childFactor, real relativeFactor);
    };

    /**
    * draw a tree of node. Evrey times we dive in depth the elements are
    * drawed smaller
    * TODO : ZOOMIN ZOOMOUT when entering or quitting a node, beatyfull,
    *   but hard to implement
    */
    class Tree : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      Tree(System * system, const char * name, int width, int height,
        bool showRoot=false, real childFactor=0.5, real relativeFactor=0.5, int maxDepth=16);
      ///
      ~Tree();
      ///
      void SetRootNode(TreeNode * root);
      ///
      void Clear();
      ///
      TreeNode * GetRootNode() const;
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
      ///
      void AnimPass(Image *, real x, real y, real width, real height, bool mustRefresh);
      ///
      void SetDisplayRoot(TreeNode * root);
      ///
      void SetSelectedAsDisplayRoot();
      ///
      TreeNode * GetSelected() const;
      ///
      void SetSelected(TreeNode * );
      ///
      void DeleteSelected();
      ///  call this when you have finished to build the tree
      void SetComplete();
      ///
      void SetChildFactor(real );
      ///
      void SetRelativeFactor(real);
    protected :
      ///
      void GoUp();
      /// go toward selected
      int GoDown();
      int SearchDown();
      void RecursiveDrawIn(Image *, real x, real y, real width, real height, TreeNode * node, real factor, bool showRoot, bool animPass, bool mustRefresh, int depth);
      bool RecursiveClick(real x, real y, TreeNode *, bool showRoot);
      bool fShowRoot;
      real fChildFactor;
      real fTargetChildFactor;
      real fRelativeFactor;
      real fTargetRelativeFactor;
      int fMaxDepth;
      TreeNode * fSelected;
      TreeNode * fTargetDisplayRoot;
      TreeNode * fRoot;
      TreeNode * fDisplayRoot;
    };

    /**
    * This class is used to display a string. The size can be fixed
    * or it can be computed depending on the size of the string
    */
    class ImageFrame : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      ImageFrame(System * system, const char * name, Image * image=NULL);
      ///
      ~ImageFrame();
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
      ///
      void SetImage(Image * value);

    protected :
      Image * fImage;
    };

    /**
    * Just draw all the characters of the font
    */
    class AsciiTable : public ANode
    {
      typedef ANode inherited;
    public :
      ///
      AsciiTable(System * system, const char * name, int width ,int height);
      ///
      ~AsciiTable();
      ///
      ANode * MouseEvent(MouseEventKind kind, int mousecode, real x, real y);
      ///
      void DrawPass(Image *, real x, real y, real width, real height);
    };

  }

}

#endif
