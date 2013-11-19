/*****************************************************************************\
*AUTHOR     : Babel Yoann
*CREATED    : 13/04/96
*DATE       : 29/06/96
*TITLE      : object.cpp, object.h implementation
*BUGS       :
\*****************************************************************************/

#include "Object.h"
#include "TemplateFaceDrawer.h"
#include "Plotter.h"
#include "Shader.h"
#include "Mapper.h"
#include "Tools/Streams.h"

namespace Go4D
{


  /*****************************************************************************\
  * Object
  \*****************************************************************************/


  Object::Object(Image * aImage, int pnplots, int pnfaces)
  {
    nplots = pnplots;
    nfaces = pnfaces;
    assert(nplots>0 && nfaces>0);
    alloc(plots = new APlot[nplots]);
    alloc(dupplots = new APlot[nplots]);
    alloc(faces = new PFace[nfaces]);
    for (int i=0; i<nfaces; i++) faces[i]=NULL;
    rotation.RotateEuler(0,0,0);
    fImage = aImage;
  }


  /*****************************************************************************\
  *  File format :
  *  Object Name
  *  Nbr plots
  *              |                  Plot 0
  *  Nbr plots * | X Y Z              ...
  *              |                  Plot N
  *  Nbr faces
  *              |                  Face 0
  *  Nbr faces * |  int?   nbr_plots   nbrplots * ( int = numplots+1 )
  *              [                  Face N
  \*****************************************************************************/

  Object::Object(Image * aImage, const char * filename, FileType fileType, Image * mapImage)
  {
    fImage = aImage;
    nplots = 0;
    nfaces = 0;
    switch (fileType)
    {
    case (ft3D) : Read3DFile(filename, mapImage); break;
    case (ftOFF) : ReadOFFFile(filename, mapImage); break;
    }
  }

  void Object::ReadOFFFile(const char * filename, Image * mapImage)
  {
    ifstream obj(filename);
    int dummy;

    // Read vertices
    //--------------
    char name [80];
    obj >> name;
    obj >> nplots;
    obj >> nfaces;
    obj >> dummy;

    assert(nplots > 0);
    alloc(plots = new APlot[nplots]);
    alloc(dupplots = new APlot[nplots]);

    int i,j;

    for (i=0; i<nplots; i++)
    {
      real x, y, z;
      obj >> x >>  y >>  z ;
      plots[i] = APlot(x,y,-z);
    }

    // Read faces
    //-----------

    assert(nfaces > 0);
    alloc(faces = new PFace[nfaces]);
    for (i=0; i<nfaces; i++) faces[i]=NULL;
    for (j=0; j<nfaces; j++)
    {
      //int c
      int facenplots;
      obj >> /*c >>*/ facenplots; // no color read in this format
      int * faceplots;
      alloc(faceplots = new int [facenplots]);
      for (i=0; i<facenplots; i++)
        obj >>  faceplots[i];
      faces[j] =
        //new FlatFace(new FlatFaceDrawer(*fImage, facenplots), 1,14,facenplots, true);
        //new GouraudFace(new GouraudFaceDrawer(*fImage, facenplots), 1,14,facenplots, true);
        new FlatFace(
        new MappedFaceDrawer<ImagePlotter, FlatShader, NullMapper>(*fImage, new DrawerImageData(mapImage)),
        1,14,facenplots, true);
      //new EnvMapFace(
      //  new MappedFaceDrawer<ImagePlotter, NullShader, LinearMapper>(*fImage, new DrawerImageData(mapImage)),
      //  1,14,facenplots, true);
      for (int k=0; k<facenplots; k++)
        (*faces[j])[k] = &(dupplots[faceplots[k]]);
      delete [] faceplots;
    }
    rotation.RotateEuler(0,0,0);
  }

  void Object::Read3DFile(const char * filename, Image * mapImage)
  {
    ifstream obj(filename);
    //assert(obj.is_open());
    //obj.seekg(0,ios::beg);

    // Read vertices
    //--------------
    char name [80];
    obj >> name;
    obj >> nplots;
    assert(nplots > 0);
    alloc(plots = new APlot[nplots]);
    alloc(dupplots = new APlot[nplots]);

    int i,j;

    for (i=0; i<nplots; i++)
    {
      real x, y, z;
      obj >> x >>  y >>  z ;
      plots[i] = APlot(x,y,z);
    }

    // Read faces
    //-----------

    obj >> nfaces;
    assert(nfaces > 0);
    alloc(faces = new PFace[nfaces]);
    for (i=0; i<nfaces; i++) faces[i]=NULL;
    for (j=0; j<nfaces; j++)
    {
      //int c;
      int facenplots;
      obj >> /*c >>*/ facenplots; // no color read in this format
      int * faceplots;
      alloc(faceplots = new int [facenplots]);
      for (i=0; i<facenplots; i++)
        obj >>  faceplots[i];
      faces[j] =
        //new FlatFace(new FlatFaceDrawer(*fImage, facenplots), 1,14,facenplots, true);
        //new GouraudFace(new GouraudFaceDrawer(*fImage, facenplots), 1,14,facenplots, true);
        //new FlatFace(
        //  new MappedFaceDrawer<ImagePlotter, FlatShader, NullMapper>(*fImage, DrawerImageData(mapImage)),
        //  1,14,facenplots, true);
        new EnvMapFace(
        new MappedFaceDrawer<ImagePlotter, NullShader, LinearMapper>(*fImage, new DrawerImageData(mapImage)),
        1,14,facenplots, true);
      for (int k=0; k<facenplots; k++)
        (*faces[j])[k] = &(dupplots[faceplots[k]]);
      delete [] faceplots;
    }
    rotation.RotateEuler(0,0,0);
  }


  Object::~Object()
  {
    for (int i=0; i<nfaces; i++)
    {
      assert(faces[i] != NULL);
      delete faces[i];
    }
    delete [] plots;
    delete [] dupplots;
    delete [] faces;
  }

  void Object::Update(const Camera & camera)
  {
    for (int i=0; i<nplots; i++)
    {
      Vector3 tmp = rotation * plots[i];
      tmp = tmp + translation;
      dupplots[i] = camera.ChangeToBase(tmp);
    }
  }

  void Object::Draw(const Camera & camera, const ALight & aLight, bool sorted) const
  {
    int i;
    Plane screen(Vector3(0,0,1),camera.CutPlaneHeight());
    for (i=0; i<nfaces; i++)
      assert( faces[i] != NULL);
    for (i=0; i<nfaces; i++)
      faces[i]->BeginDraw();
    for (i=0; i<nfaces; i++)
      faces[i]->Update(camera);
    for (i=0; i<nfaces; i++)
      faces[i]->SplitRight(screen);
    for (i=0; i<nfaces; i++)
    {
      faces[i]->Lightning();
      faces[i]->AddLight(aLight);
      faces[i]->Lightned();
    }
    for (i=0; i<nfaces; i++)
      faces[i]->Project(camera);

    if (sorted)
      SortedDraw();
    else
      BasicDraw();
  }

  void Object::PrepareDraw(const Camera & camera, const ALight & aLight) const
  {
    int i;
    Plane screen(Vector3(0,0,1),camera.CutPlaneHeight());
    for (i=0; i<nfaces; i++)
      assert( faces[i] != NULL);
    for (i=0; i<nfaces; i++)
      faces[i]->BeginDraw();
    for (i=0; i<nfaces; i++)
      faces[i]->Update(camera);
    for (i=0; i<nfaces; i++)
      faces[i]->SplitRight(screen);
    for (i=0; i<nfaces; i++)
    {
      faces[i]->Lightning();
      faces[i]->AddLight(aLight);
      faces[i]->Lightned();
    }
    for (i=0; i<nfaces; i++)
      faces[i]->Project(camera);
  }

  void Object::BasicDraw() const
  {
    for (int i=0; i<nfaces; i++)
      faces[i]->Draw();
  }

  void Object::SortedDraw() const
  {
    int i;
    bool * marked = new bool[nfaces];
    for (i=0; i<nfaces; i++) marked[i] = false;
    real MinZ = faces[0]->GetMinZ();
    for (i=1; i<nfaces; i++)
      if (faces[i]->GetMinZ()<MinZ) MinZ=faces[i]->GetMinZ();
    for (i=0; i<nfaces; i++)
    {
      real NMaxZ = MinZ;
      int j, r;
      for (j=0; j<nfaces; j++)
        if (!marked[j] && faces[j]->GetMaxZ()>=NMaxZ)
        {
          r = j;
          NMaxZ=faces[j]->GetMaxZ();
        }
        faces[r]->Draw();
        marked[r] = true;
    }
    delete [] marked;
  }

  void Object::SetPosition(const Rotation & protation,
    const Vector3 & ptranslation)
  {
    rotation = protation;
    translation = ptranslation;
  }

  void Object::SetSize(real)
  { }

  void Object::SetColor(Color col)
  {
    for (int i=0; i<nfaces; i++)
      faces[i]->SetBaseColor(col);
  }

  void Object::SetShading(Color shade)
  {
    for (int i=0; i<nfaces; i++)
      faces[i]->SetShading(shade);
  }


}
