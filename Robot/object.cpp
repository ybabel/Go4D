/*****************************************************************************\
 *AUTHOR     : Babel Yoann
 *DATE       : 13/04/96
 *TITLE      : object.cc , 3d objects
 *OBJECT     : 
 *CURRENTLY  :  
 *BUGS       : 
\*****************************************************************************/


#include <assert.h>
#include <fstream.h>
#include "object.h"


/*****************************************************************************\
 * Object
\*****************************************************************************/


Object::Object(int pnplots, int pnfaces)
{
  nplots = pnplots;
  nfaces = pnfaces;
  assert(nplots>0 && nfaces>0);
  alloc(plots = new Plot[nplots]);
  alloc(dupplots = new Plot[nplots]);
  alloc(faces = new PFace[nfaces]);
  for (int i=0; i<nfaces; i++) faces[i]=NULL;
  rotation.RotateEuler(0,0,0);
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

Object::Object(char * filename)
{
  nplots = 0;
  nfaces = 0;
  ifstream obj(filename);

  // Read vertices
  //--------------
  char name [80];
  obj >> name;
  obj >> nplots;
  assert(nplots > 0);
  alloc(plots = new Plot[nplots]);
  alloc(dupplots = new Plot[nplots]);
  for (int i=0; i<nplots; i++)
    {
      real x, y, z;
      obj >> x >>  y >>  z ;
      plots[i] = Plot(x,y,z);
    }

  // Read faces
  //-----------

  obj >> nfaces;
  assert(nfaces > 0);
  alloc(faces = new PFace[nfaces]);
  for (int j=0; j<nplots; j++)
    {
      int c, facenplots;
      obj >> c >> facenplots; 
      int * faceplots;
      alloc(faceplots = new int [facenplots]);
      for (int i=0; i<facenplots; i++)
      	obj >>  faceplots[i];
      faces[j] = new Face(1,14,facenplots, TRUE);
      for (int k=0; k<facenplots; k++)
      	(*faces[j])[k] = dupplots+faceplots[k]-1;
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

void Object::Attach(Scene & scene) const
{
  for (int i=0; i<nfaces; i++)
    scene.AddFace(faces[i]);
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

void Object::Draw(const Camera & camera, Drawer & drawer, 
		  const Vector3 & light) const
{
  Plane screen(Vector3(0,0,1),0);
  for (int i=0; i<nfaces; i++)
    {
      assert( faces[i] != NULL);
      faces[i]->Update(camera.GetFocal());
      faces[i]->SplitRight(screen);
      faces[i]->AddLight(light);
      faces[i]->Project(drawer, camera.GetFocal(), camera.GetZoom() );
      faces[i]->Draw(drawer);
    }
}

void Object::QuickDraw(const Camera & camera, Drawer & drawer, 
		       const Vector3 & light) const
{
  Plane screen(Vector3(0,0,1),0);
  for (int i=0; i<nfaces; i++)
    {
      assert( faces[i] != NULL);
      faces[i]->Update(camera.GetFocal());
      faces[i]->SplitRight(screen);
      faces[i]->AddLight(light);
      faces[i]->Project(drawer, camera.GetFocal(), camera.GetZoom());
      faces[i]->DrawShape(drawer);
    }
}

void Object::SetPosition(const Rotation & protation, 
			 const Vector3 & ptranslation)
{
  rotation = protation;
  translation = ptranslation;
}

void Object::SetSize(real)
{ }

void Object::SetColor(byte col)
{
  for (int i=0; i<nfaces; i++)
    faces[i]->SetBaseColor(col);
}

void Object::SetShading(byte shade)
{
  for (int i=0; i<nfaces; i++)
    faces[i]->SetShading(shade);
}

void Object::NewFace4(int num_face, int p1, int p2, int p3, int p4,
		      byte color, byte nbcolor, int av)
{
  assert(num_face>=0 && num_face<nfaces);
  faces[num_face] = new Face4(dupplots[p1],
    dupplots[p2],
    dupplots[p3],
		dupplots[p4], color, nbcolor, av);
}

void Object::NewMapFace4(int num_face, int p1, int p2, int p3, int p4,
  const Image * pim, int av)
{
  assert(num_face>=0 && num_face<nfaces);
  faces[num_face] = new MappedImage(dupplots[p1],
    dupplots[p2],
	  dupplots[p3],
		dupplots[p4], pim, av);
}

void Object::NewFace3(int num_face, int p1, int p2, int p3,
  byte color, byte nbcolor, int av)
{
  assert(num_face>=0 && num_face<nfaces);
  faces[num_face] = new Face3(dupplots[p1],
    dupplots[p2], dupplots[p3],
		color, nbcolor, av);
}


/*****************************************************************************\
 * Cube
\*****************************************************************************/


Cube::Cube() : Object(8,6)
{
  SetSize(1);
  NewFace4(0, 0, 1, 2, 3, byte(0),byte(256),FALSE);
  NewFace4(1, 0, 3, 7, 4, byte(0),byte(256),FALSE);
  NewFace4(2, 0, 4, 5, 1, byte(0),byte(256),FALSE);
  NewFace4(3, 4, 7, 6, 5, byte(0),byte(256),FALSE);
  NewFace4(4, 1, 5, 6, 2, byte(0),byte(256),FALSE);
  NewFace4(5, 2, 6, 7, 3, byte(0),byte(256),FALSE);
}

void Cube::SetSize(real dim)
{
  plots[0] = Plot(-dim,-dim, dim);
  plots[1] = Plot(-dim, dim, dim);
  plots[2] = Plot( dim, dim, dim);
  plots[3] = Plot( dim,-dim, dim);
  plots[4] = Plot(-dim,-dim,-dim);
  plots[5] = Plot(-dim, dim,-dim);
  plots[6] = Plot( dim, dim,-dim);
  plots[7] = Plot( dim,-dim,-dim);
}

/*****************************************************************************\
 * MappedCube
\*****************************************************************************/


MappedCube::MappedCube(const Image * pima) : Cube()
{
  for (int i=0; i<nfaces; i++) delete faces[i];
  NewMapFace4(0, 0, 1, 2, 3, pima,FALSE);
  NewMapFace4(1, 0, 3, 7, 4, pima,FALSE);
  NewMapFace4(2, 0, 4, 5, 1, pima,FALSE);
  NewMapFace4(3, 4, 7, 6, 5, pima,FALSE);
  NewMapFace4(4, 1, 5, 6, 2, pima,FALSE);
  NewMapFace4(5, 2, 6, 7, 3, pima,FALSE);  
}
