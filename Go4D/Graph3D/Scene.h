#ifndef _SCENE_H_
#define _SCENE_H_

/**
*AUTHOR     : Babel Yoann
*DATE       : 13/04/96
*TITLE      : scene.h , 3d scene drawing
*OBJECT     :
*CURRENTLY  : this unit must be rewrite to use quaternions and Matrix4x4.
*BUGS       :
*NOTE       : improvements : plots projection must be set in camera
*/

#include "Face.h"
#include "Object.h"
#include "Light.h"
#include "FaceDrawer.h"

namespace Go4D
{


  /// a linked list of Plots (APlot)
  struct PlotElem
  {
    APlot plot;
    PlotElem * next;
    PlotElem(const APlot &);
  };

  /**
  *PLOTSET :
  *Rotate, Translate : apply the tranformation on all the points. becareful
  *  because the all the plots will be modified, if you don't have a backup
  *  copy of all your points you will loose their old definition
  */
  class PlotSet
  {
  public :
    PlotSet(int prealloc=0);
    ~PlotSet();
    void operator = (const PlotSet &);
    APlot& operator [] (int) const;
    void AddPlot(const APlot &);
    void Rotate(const Rotation &);
    void Translate(const APlot &);
    int GetNPlots() const {return nplots;}
#ifndef UNDER_CE
    friend ostream & operator << (ostream &, const PlotSet &);
#endif

  private :
    void Purge(void);
    PlotElem *root, *queue;
    int nplots;
  };

  /// a pointer on a face
  typedef Face * PFace;

  /// tree of faces
  struct FaceNode
  { // tree
    PFace f;
    FaceNode *right, *left;
  };

  /// double linked list of faces
  struct FaceElem
  { // double list
    PFace f;
    FaceElem *next, *prev;
  };

  /**
  *  This class is used to draw a set of faces
  *  this class is used to draw a set of faces (currently there is 3 differents
  *  algo to draw, 1. the faces are drawn in no particular oder, 2. the faces
  *  are sorted each time the method draw is called, 3. the faces are sorted
  *  once, and draw in that order.
  *TODO : face set and childs currently support only one light source.
  */
  class Scene
  { // owner of the faces
  public :
    Scene(int nbfaces);
    virtual ~Scene();
    /**
    *AddFace :
    *  FaceSet is list of n faces, when you create a FaceSet the necessary memory
    *  is allocated, you must then fill the list with the faces you want. You
    *  must use this function to fill the list. You must fill all the list (
    *  that means that you call AddFace n times), otherwise an assertion will fail
    */
    void AddFace(PFace );
    virtual void Draw(const Camera &, const ALight &light);
    void AttachObject(Object &);
    PFace & operator [] (int) const;

  protected :
    virtual void PrepareDraw(const Camera &, const ALight &light);
    FaceElem * listroot, * listqueue;
    PFace * faces;
    int NbFaces;
    int CurNbFaces;
  };

  /// a scene where the faces of the object are sorted (back face culling)
  class SortedObject : public Scene
  { // sort in a binary tree
    typedef Scene inherited;
  public :
    SortedObject(int nbfaces):inherited(nbfaces){root = NULL;}
    virtual ~SortedObject(){ DelTree(root); }
    virtual void Draw(const Camera &, const ALight &light);

  protected :
    FaceNode * root;
    void Sort();
    void DelTree(FaceNode * &);
    void DrawFace(FaceNode *);
    virtual void AddFaceNode(PFace , FaceNode * &);
  };

  /// optimized sorting of the face. The faces are sorted only when
  /// it's necessary
  class PreSortedObject : public SortedObject
  {
    typedef SortedObject inherited;
  public :
    PreSortedObject(int nbfaces):inherited(nbfaces)
    {alreadysorted=0;}
    virtual void Draw(const Camera &, const ALight &light);
    void ReSort() {alreadysorted = 0;}

  private :
    int alreadysorted;
  };

}

#endif
