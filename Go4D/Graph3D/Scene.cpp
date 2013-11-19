/*****************************************************************************\
*AUTHOR     : Babel Yoann
*CREATED    : 8/11/95,
*DATE       : 29/02/96
*TITLE      : scene.cc , scene.h implementation
*BUG        :
\*****************************************************************************/


#include "Scene.h"

namespace Go4D
{


  /*****************************************************************************\
  *   PlotSet
  \*****************************************************************************/


  PlotElem::PlotElem(const APlot & pplot)
  {
    plot = pplot;
    next = NULL;
  }

  PlotSet::PlotSet(int prealloc)
  {
    root = NULL;
    queue = NULL;
    nplots = 0;
    assert(prealloc>=0);
    for (int i=0; i<prealloc; i++) AddPlot( APlot() );
  }

  PlotSet::~PlotSet()
  {
    Purge();
  }

  APlot & PlotSet::operator [] (int i) const
  {
    assert(nplots>=0);
    assert(i>=0&&i<nplots);
    PlotElem *tmp;
    for (tmp=root; i!=0; i--) tmp=tmp->next;
    return tmp->plot;
  }

  void PlotSet::operator = (const PlotSet & ps)
  {
    if (nplots==0)
    {
      for (int i=0; i<ps.nplots; i++) AddPlot( ps[i] );
    }
    else
    { // don't reallocate all the memory
      assert(ps.nplots==nplots);
      PlotElem *tmp=root, *pstmp=ps.root;
      for (int i=0; i<nplots; i++, tmp=tmp->next, pstmp=pstmp->next)
        tmp->plot = pstmp->plot;
    }
  }

  void PlotSet::AddPlot(const APlot & plot)
  {
    PlotElem * tmp;
    alloc(tmp = new PlotElem(plot));
    if (root==NULL)
      root = tmp;
    else
    {
      assert(queue->next == NULL);
      queue->next = tmp;
    }
    queue = tmp;
    nplots++;
  }

  void PlotSet::Rotate(const Rotation & r)
  {
    PlotElem *tmp=root;
    for (int i=0; i<nplots; i++, tmp=tmp->next)
      static_cast<Vector3>(tmp->plot) = static_cast<Vector3>(r*tmp->plot);
  }

  void PlotSet::Translate(const APlot & v)
  {
    PlotElem *tmp=root;
    for (int i=0; i<nplots; i++, tmp=tmp->next)
      static_cast<Vector3>(tmp->plot) = static_cast<Vector3>(tmp->plot)+static_cast<Vector3>(v);
  }

  void PlotSet::Purge(void)
  {
    PlotElem * tmp=root;
    while (tmp != NULL)
    {
      PlotElem * tmp1=tmp->next;
      delete tmp;
      tmp = tmp1;
    }
    root = NULL;
    queue = NULL;
    nplots = 0;
  }

#ifndef UNDER_CE
  ostream & operator << (ostream & os, const PlotSet & ps)
  {
    PlotElem *tmp=ps.root;
    for (int i=0; i<ps.nplots; i++, tmp=tmp->next)
      os << tmp->plot << kCR;
    return os;
  }
#endif

  /*****************************************************************************\
  *   Scene
  \*****************************************************************************/

  Scene::Scene(int n)
  {
    NbFaces = n;
    alloc(faces = new PFace [NbFaces]);
    for (int i=0; i<NbFaces; i++)
      faces[i] = NULL;
    CurNbFaces = 0;
  }

  Scene::~Scene()
  {
    // faces are not aggregated
    //for (int i=0; i<NbFaces; i++)
    //  delete faces[i];
    delete [] faces;
  }

  void Scene::AddFace(PFace fp)
  {
    assert(CurNbFaces < NbFaces);
    faces[CurNbFaces++] = fp;
  }

  PFace & Scene::operator [] (int i) const
  {
    assert(i>=0&&i<NbFaces);
    return faces[i];
  }

  void Scene::PrepareDraw(const Camera &camera, const ALight &light)
  {
    int i;
    assert(CurNbFaces == NbFaces);
    Plane screen(Vector3(0,0,1),camera.CutPlaneHeight());
    for (i=0; i<NbFaces; i++)
      assert( faces[i] != NULL);
    for (i=0; i<NbFaces; i++)
      faces[i]->BeginDraw();
    for (i=0; i<NbFaces; i++)
      faces[i]->Update(camera);
    for (i=0; i<NbFaces; i++)
      faces[i]->SplitRight(screen);
    for (i=0; i<NbFaces; i++)
    {
      faces[i]->Lightning();
      faces[i]->AddLight(light);
      faces[i]->Lightned();
    }
    for (i=0; i<NbFaces; i++)
      faces[i]->Project(camera);
  }

  void Scene::Draw(const Camera & aCamera, const ALight & light)
  {
    PrepareDraw(aCamera, light);
    for (int i=0; i<NbFaces; i++)
      faces[i]->Draw();
  }

  void Scene::AttachObject(Object & aObject)
  {
    for (int i=0; i<aObject.FaceCount(); i++)
    {
      AddFace(aObject.GetFace(i));
    }
  }


  /*****************************************************************************\
  *  SortedObject
  \*****************************************************************************/

  void SortedObject::Sort()
  {
    for (int i=0; i<NbFaces; i++)
      if (faces[i]->Visible())
        AddFaceNode(faces[i],root);
  }

  void SortedObject::DelTree(FaceNode * &fn)
  {
    if (fn==NULL) return;
    DelTree(fn->right);
    DelTree(fn->left);
    delete fn;
    fn = NULL;
  }

  void SortedObject::DrawFace(FaceNode *fn)
  {
    if (fn==NULL) return;
    DrawFace(fn->left);
    fn->f->Draw();
    DrawFace(fn->right);
  }

  void SortedObject::Draw(const Camera & aCamera, const ALight & light)
  {
    PrepareDraw(aCamera, light);
    Sort();
    DrawFace(root);
    DelTree(root);
  }

  void SortedObject::AddFaceNode(PFace f, FaceNode * & fn)
  {
    assert(f!=NULL);
    if (fn==NULL)
    {
      alloc(fn = new FaceNode);
      fn->f = f;
      fn->right = NULL;  fn->left = NULL;
    }
    else
    {
      // drawind : Left Middle Right
      //           back(z>)    front(z<)
      if (f->GetAverageZ() < fn->f->GetAverageZ() ) AddFaceNode(f, fn->right);
      else AddFaceNode(f, fn->left);
    }
  }


  /*****************************************************************************\
  *  PreSortedObject
  \*****************************************************************************/


  void PreSortedObject::Draw(const Camera & aCamera, const ALight & light)
  {
    PrepareDraw(aCamera, light);
    if (!alreadysorted) { Sort(); alreadysorted = 1; }
    DrawFace(root);
  }


}
