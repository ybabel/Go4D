/*****************************************************************************\
 * CHESS.CC, 3D interface, made by LeBab for LePrib
\*****************************************************************************/


#include "device.h"
#include "object.h"

#define FOCAL 3
#define DIMX 300
#define DIMY 300

class Chess : public Object
{
  public :
  Chess();
  int Visible(real focal);
};

class Pion : public Object
{
  public :
  Pion();
  void SetPositionOnBoard(int x, int y, const Chess &);
  void Draw(const Camera &, Drawer &) const;
  void SetVisible(int pvisible) {visible=pvisible;};
  real GetZ();
  void SetColor(byte color);

  private :
  int visible;
  byte color;
};


#define NPIONS 40
class Damier
{
  public :
  Damier();
  void Draw(const Camera &, Drawer &);
  void SetPosition(const Rotation &, const Vector3 &);

  private :
  void DrawPions(const Camera &, Drawer &);
  Chess chess;

  struct MarkPion
    {
      Pion pion;
      int marked;
    };
  MarkPion pions[NPIONS];
};


//*****************************************************************************


Chess::Chess() : Object(121+4,100+5)
{
  for (int i=0; i<11; i++)
    for (int j=0; j<11; j++)
      plots[i*11+j] = Plot(i/5.0-1,j/5.0-1,0);
  plots[121]  = Plot(-1,-1,0.5);  //plots[0]  = Plot(-1,-1,0);
  plots[122]  = Plot(1,-1,0.5);   //plots[110]= Plot(1,-1,0);
  plots[123]  = Plot(1,1,0.5);    //plots[120]= Plot(1,1,0);
  plots[124]  = Plot(-1,1,0.5);   //plots[10] = Plot(-1,1,0);

  for (int k=0; k<10; k++)
    for (int j=0; j<10; j++)
      faces[k*10+j] = new Face4(dupplots[k*11+j],
				dupplots[k*11+j+11],
				dupplots[k*11+j+12],
				dupplots[k*11+j+1],
				(k+j)%2,1,FALSE);
  faces[100] = new Face4(dupplots[121], dupplots[124],
			 dupplots[123], dupplots[122],
			 8,1,FALSE);
  faces[101] = new Face4(dupplots[110], dupplots[0],
			 dupplots[121], dupplots[122],
			 4,1,FALSE);
  faces[102] = new Face4(dupplots[120], dupplots[110],
			 dupplots[122], dupplots[123],
			 5,1,FALSE);
  faces[103] = new Face4(dupplots[10], dupplots[120],
			 dupplots[123], dupplots[124],
			 6,1,FALSE);
  faces[104] = new Face4(dupplots[0], dupplots[10],
			 dupplots[124], dupplots[121],
			 7,1,FALSE);
}

int Chess::Visible(real focal)
{
  return faces[0]->ConvexTest(focal);
}

//*****************************************************************************

#define PIONPRES 8
Pion::Pion():Object(PIONPRES*2+1, PIONPRES+2)
{
  color = 0; //black
  for (int i=0; i<PIONPRES; i++)
    {
      real angle = i*2*M_PI/PIONPRES;
      plots[i] = Plot(0.1*cos(angle), 
		      0.1*sin(angle),0);
      plots[PIONPRES+i] = Plot(0.1*cos(angle), 
			       0.1*sin(angle),-0.1);
    }
  plots[PIONPRES*2] = Plot(0,0,-0.05); // center of object
  for (int j=0; j<PIONPRES; j++)
    {
      int nextj = j+1==PIONPRES?0:j+1;
      faces[j] = new Face4(dupplots[j], 
			   dupplots[nextj],
			   dupplots[nextj+PIONPRES],
			   dupplots[j+PIONPRES],
			   2,1,FALSE);
    }
  faces[PIONPRES] = new Face(color,1,PIONPRES,FALSE);
  for (int k=0; k<PIONPRES; k++)
    (*faces[PIONPRES])[k]=&dupplots[PIONPRES-k-1];
  faces[PIONPRES+1] = new Face(color,1,PIONPRES,FALSE);
  for (int l=0; l<PIONPRES; l++)
    (*faces[PIONPRES+1])[l]=&dupplots[PIONPRES+l];
  visible = TRUE;
}

void Pion::SetPositionOnBoard(int x, int y, const Chess & chess)
{
  assert(x>=0 && x<10 && y>=0 && y<10);
  rotation = chess.GetRotation();
  translation = chess.GetTranslation() + Vector3(0.2*x-0.9, 0.2*y-0.9, 0);
}

void Pion::Draw(const Camera & camera, Drawer & drawer) const
{
  if (visible)
    {
      Object::Draw(camera, drawer, Vector3(1,1,1));
      //faces[PIONPRES+1]->DrawShape(drawer);
    }
}

real Pion::GetZ()
{
  return dupplots[PIONPRES*2][2];
}

void Pion::SetColor(byte color)
{
  faces[PIONPRES]->SetBaseColor(color);
  faces[PIONPRES+1]->SetBaseColor(color);
}

//*****************************************************************************

Damier::Damier()
{
  SetPosition(chess.GetRotation(), chess.GetTranslation());
}

void Damier::Draw(const Camera & camera, Drawer & drawer)
{
  chess.Update(camera);
  for (int i=0; i<NPIONS; i++)
    pions[i].pion.Update(camera);
  if (chess.Visible(camera.GetFocal()))
    {
      chess.Draw(camera, drawer, Vector3(1,1,1));
      DrawPions(camera, drawer);
    }
  else
    {
      DrawPions(camera, drawer);
      chess.Draw(camera, drawer, Vector3(1,1,1));
    }
}

void Damier::DrawPions(const Camera & camera, Drawer & drawer)
{
  //for (int k=0; k<NPIONS; k++) pions[k].pion.Draw(camera, drawer);

  for (int h=0; h<NPIONS; h++) pions[h].marked = FALSE;
  for (int i=0; i<NPIONS; i++)
    {
      real maxz = -MAXFLOAT;
      int imax = 0;
      for (int j=0; j<NPIONS; j++) 
	if ((pions[j].marked==FALSE) && (pions[j].pion.GetZ() > maxz))
	  { maxz = pions[j].pion.GetZ(); imax = j; }
      assert(pions[imax].marked == FALSE);
      pions[imax].marked = TRUE;
      pions[imax].pion.Draw(camera, drawer);
    }
}

void Damier::SetPosition(const Rotation &rotation, const Vector3 & translation)
{
  chess.SetPosition(rotation, translation);
  for (int j=0; j<NPIONS/2; j++) pions[j].pion.SetColor(1); //white

  for (int i=0; i<NPIONS/2; i++)
    pions[i].pion.SetPositionOnBoard(i%5*2+i/5%2, i/5, chess);

  for (int k=0; k<NPIONS/2; k++)
    pions[k+NPIONS/2].pion.SetPositionOnBoard(k%5*2+(k/5+1)%2, 9-k/5, chess);
}

//*****************************************************************************

#define STEP M_PI/64

int main(void)
{
  Device device;
  Drawer drawer(DIMX,DIMY);
  real teta=0, phi=0, psi=0;
  real z=-2.4*FOCAL;
  Damier damier;
  Camera camera(teta,phi,psi,0,0,z,FOCAL, FOCAL);
  byte r;
  
  device.AddImage(&drawer);
  while ( (r=device.Read(&drawer))!=kEND)
    {
      switch (r)
	{
	case kUP : teta+=STEP; break;
	case kDN : teta-=STEP; break;
	case kLE : phi +=STEP; break;
	case kRI : phi -=STEP; break;
	case kFW : psi +=STEP; break;
	case kBW : psi -=STEP; break;
	case kLO : z += 0.1; break;
	case kHI : z -=0.1; break;
	}
      drawer.Clear();
      camera.NewAngle(teta,phi,psi);
      camera.NewPosition(0,0,z);
      damier.Draw(camera, drawer);
      device.DisplayImage(&drawer);
    }
}

