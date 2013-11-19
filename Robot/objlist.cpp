/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : objlist.cpp, objlist.h implementation
 *CREATED: 05/05/96
 *DATE   : 05/05/96
 *OBJECT : list of 3D objects
 *BUG    : in Intro2, the drawer in fact is a ZDrawer, so when the cube is
 *         mapped, its in the ZBuffer mode, so the visual effect is not good.
 *         find the solution to typecast the drawer in a Drawer, in order that
  *        the mapping funtion execute in normal mode 
\*****************************************************************************/

#include "objlist.h"


//*****************************************************************************


Bolt::Bolt():Object(BOLTPRES*2+1, BOLTPRES+1)
{
  for (int i=0; i<BOLTPRES; i++)
    {
      real angle = i*2*M_PI/BOLTPRES;
      plots[i] = Plot(BOLTDIM*cos(angle),
            		      BOLTDIM*sin(angle),0);
      plots[BOLTPRES+i] = Plot(BOLTDIM*cos(angle),
                  			       BOLTDIM*sin(angle),BOLTH);
    }
  plots[BOLTPRES*2] = Plot(0,0,BOLTH/2); // center of object
  for (int j=0; j<BOLTPRES; j++)
    {
      int nextj = j+1==BOLTPRES?0:j+1;
      NewFace4(j, j, nextj, nextj+BOLTPRES, j+BOLTPRES, PALGREEN, NCOL, FALSE);
    }
  faces[BOLTPRES] = new Face(GREEN,NCOL,BOLTPRES,FALSE);
  for (int l=0; l<BOLTPRES; l++)
    (*faces[BOLTPRES])[l]=&dupplots[BOLTPRES+l];
  visible = TRUE;
}

void Bolt::SetPositionOnArea(int i, const Area & area)
{
  rotation = area.GetRotation();
  #define BANG 2*M_PI/NBOLT*i
  translation = area.GetTranslation() +
    Vector3(BOLTRADIUS*cos(BANG),BOLTRADIUS*sin(BANG), -0.01);
}

void Bolt::Draw(const Camera & camera, Drawer & drawer,
		const Vector3 & light) const
{
  if (visible)
    Object::Draw(camera, drawer, light);
}

//*****************************************************************************

Player::Player(int nplots, int nfaces) : Object(nplots, nfaces)
{ }

void Player::SetPositionOnArea(real x, real y, real rot, const Area & area)
{
  rotation.RotateZ(rot);
  rotation = rotation*area.GetRotation();
  translation = area.GetTranslation() + Vector3(x,y, -0.01);
}

void Player::Cube(int j)
{
  NewFace4(j*6+0, j*8+0, j*8+1, j*8+2, j*8+3, RED,NCOL,FALSE);
  NewFace4(j*6+1, j*8+0, j*8+3, j*8+7, j*8+4, RED,NCOL,FALSE);
  NewFace4(j*6+2, j*8+0, j*8+4, j*8+5, j*8+1, RED,NCOL,FALSE);
  NewFace4(j*6+3, j*8+4, j*8+7, j*8+6, j*8+5, RED,NCOL,FALSE);
  NewFace4(j*6+4, j*8+1, j*8+5, j*8+6, j*8+2, RED,NCOL,FALSE);
  NewFace4(j*6+5, j*8+2, j*8+6, j*8+7, j*8+3, RED,NCOL,FALSE);
}

void Player::MapCube(int j, const Image * pim)
{
  NewMapFace4(j*6+0, j*8+0, j*8+1, j*8+2, j*8+3, pim,FALSE);
  NewMapFace4(j*6+1, j*8+0, j*8+3, j*8+7, j*8+4, pim,FALSE);
  NewMapFace4(j*6+2, j*8+0, j*8+4, j*8+5, j*8+1, pim,FALSE);
  NewMapFace4(j*6+3, j*8+4, j*8+7, j*8+6, j*8+5, pim,FALSE);
  NewMapFace4(j*6+4, j*8+1, j*8+5, j*8+6, j*8+2, pim,FALSE);
  NewMapFace4(j*6+5, j*8+2, j*8+6, j*8+7, j*8+3, pim,FALSE);
}

//*****************************************************************************


#define C (cos(angle))
#define S (sin(angle))

IieRobot::IieRobot(const Image * pmap) : Player(8*4,6*4)
{
  iiemap = pmap;
  plots[0] = Plot(-PDIM,-PDIM,0    );
  plots[1] = Plot(-PDIM, PDIM,0    );
  plots[2] = Plot( PDIM, PDIM,0    );
  plots[3] = Plot( PDIM,-PDIM,0    );
  plots[4] = Plot(-PDIM,-PDIM,-PDIM*2);
  plots[5] = Plot(-PDIM, PDIM,-PDIM*2);
  plots[6] = Plot( PDIM, PDIM,-PDIM*2);
  plots[7] = Plot( PDIM,-PDIM,-PDIM*2);
  SetAngleAndLen(0,ML);
  MapCube(0, iiemap);
  Cube(1);
  Cube(2);
  Cube(3);
  destangle = M_PI/6+M_PI/9; // default value
  xinit=-0.6*cos(M_PI/6);
  yinit=-0.6*sin(M_PI/6);
}

void IieRobot::SetAngle(real newangle)
{
  destangle = newangle;
}

void IieRobot::SetInitPos(real pxinit, real pyinit)
{
  xinit = pxinit;
  yinit = pyinit;
}

void IieRobot::SetAngleAndLen(real angle, real len)
{
  plots[8+0] = Plot(-PDIM,    PDIM,      0 );
  plots[8+1] = Plot(-PDIM,    PDIM+len,  0 );
  plots[8+2] = Plot(-PDIM+MH, PDIM+len,  0 );
  plots[8+3] = Plot(-PDIM+MH, PDIM,      0 );
  plots[8+4] = Plot(-PDIM,    PDIM,     -MH);
  plots[8+5] = Plot(-PDIM,    PDIM+len, -MH);
  plots[8+6] = Plot(-PDIM+MH, PDIM+len, -MH);
  plots[8+7] = Plot(-PDIM+MH, PDIM,     -MH);

	Rotation rot; rot.RotateX(-angle);
	plots[16+0] = rot*Plot(-PDIM1,0,       0 );
	plots[16+1] = rot*Plot(-PDIM1,PDIM1*2, 0 );
	plots[16+2] = rot*Plot( PDIM1,PDIM1*2, 0 );
	plots[16+3] = rot*Plot( PDIM1,0,      0 );
	plots[16+4] = rot*Plot(-PDIM1,0,      -BH);
	plots[16+5] = rot*Plot(-PDIM1,PDIM1*2,-BH);
	plots[16+6] = rot*Plot( PDIM1,PDIM1*2,-BH);
	plots[16+7] = rot*Plot( PDIM1,0,      -BH);
	for (int i=16; i<24; i++)
		plots[i] = plots[i]+Vector3(0,PDIM+len,0);

	plots[24+0] = Plot(PDIM,    PDIM,      0 );
	plots[24+1] = Plot(PDIM-MH, PDIM,      0 );
	plots[24+2] = Plot(PDIM-MH, PDIM+len,  0 );
	plots[24+3] = Plot(PDIM,    PDIM+len,  0 );
	plots[24+4] = Plot(PDIM,    PDIM,     -MH);
	plots[24+5] = Plot(PDIM-MH, PDIM,     -MH);
	plots[24+6] = Plot(PDIM-MH, PDIM+len, -MH);
	plots[24+7] = Plot(PDIM,    PDIM+len, -MH);
}

void IieRobot::Cinematiq(int i, const Area & area, int imax)
{
	if (i<0) i=0;
	if (i>imax) i=imax;
	real rot=-M_PI/2+M_PI/6; // init angle
	real len=0.05, angle=M_PI/2;

	if (i==imax)
		{ angle=0; len=0.35; rot=destangle; }
	else
		{
			switch (i)
				{
				case 0 : break;
				case 1 : len=0.4; break;
				case 2 : len=0.4; angle=0; break;
				case 3 : len=0.05; angle=0; break;
					default : angle=0; rot=real(i-4)/real(imax-4)*(destangle-rot)+rot;
					break;
				}
		}

	SetPositionOnArea(xinit,yinit,rot, area);
	SetAngleAndLen(angle,len);
}

//*****************************************************************************

Robot::Robot() : Player(8*3,6*3)
{
  plots[0] = Plot(-PDIM,-PDIM,0    );
  plots[1] = Plot(-PDIM, PDIM,0    );
  plots[2] = Plot( PDIM, PDIM,0    );
  plots[3] = Plot( PDIM,-PDIM,0    );
  plots[4] = Plot(-PDIM,-PDIM,-PDIM);
  plots[5] = Plot(-PDIM, PDIM,-PDIM);
  plots[6] = Plot( PDIM, PDIM,-PDIM);
  plots[7] = Plot( PDIM,-PDIM,-PDIM);
	SetAngleAndLen(0,ML);
  Cube(0);
  Cube(1);
  Cube(2);
}

void Robot::SetAngleAndLen(real angle, real len)
{
  plots[8+0] = Plot(-MH, PDIM,      0);
  plots[8+1] = Plot(-MH, PDIM+len*C, -len*S);
  plots[8+2] = Plot( MH, PDIM+len*C, -len*S);
  plots[8+3] = Plot( MH, PDIM,      0);
  plots[8+4] = Plot(-MH, PDIM,      -MH);
  plots[8+5] = Plot(-MH, PDIM+len*C-MH*S, -(len*S+MH*C));
  plots[8+6] = Plot( MH, PDIM+len*C-MH*S, -(len*S+MH*C));
  plots[8+7] = Plot( MH, PDIM,      -MH);

  Rotation rot; rot.RotateX(-angle);
  plots[16+0] = rot*Plot(-BH,len,     0  );
  plots[16+1] = rot*Plot(-BH,len+BH*2,0  );
  plots[16+2] = rot*Plot( BH,len+BH*2,0  );
  plots[16+3] = rot*Plot( BH,len,     0  );
  plots[16+4] = rot*Plot(-BH,len,     -BH);
  plots[16+5] = rot*Plot(-BH,len+BH*2,-BH);
  plots[16+6] = rot*Plot( BH,len+BH*2,-BH);
  plots[16+7] = rot*Plot( BH,len,     -BH);
  for (int i=16; i<24; i++)
    plots[i] = plots[i]+Vector3(0,PDIM,0);
}

void Robot::Cinematiq(int i, const Area & area, int imax)
{
	if (i<0) i=0;
  if (i>imax) i=imax;
  real x=(0.5 -i/150.0)*cos(M_PI/6);
  real y=(0.5 -i/150.0)*sin(M_PI/6);
  real rot=M_PI/6+M_PI/2;
  real len=0.2, angle=M_PI/2;

  if (i>imax-4) angle=0;

  SetPositionOnArea(x,y,rot, area);
  SetAngleAndLen(angle,len);
}

//*****************************************************************************

SmallRobot::SmallRobot(PositionList & path) : Player(8,6)
{
  plots[0] = Plot(-SDIM,-SDIM,0    );
  plots[1] = Plot(-SDIM, SDIM,0    );
  plots[2] = Plot( SDIM, SDIM,0    );
  plots[3] = Plot( SDIM,-SDIM,0    );
  plots[4] = Plot(-SDIM,-SDIM,-2*SDIM);
  plots[5] = Plot(-SDIM, SDIM,-2*SDIM);
  plots[6] = Plot( SDIM, SDIM,-2*SDIM);
  plots[7] = Plot( SDIM,-SDIM,-2*SDIM);
  Cube(0);
  ppath = &path;
}

void SmallRobot::SetAngleAndLen(real, real)
{ }

void SmallRobot::Cinematiq(int i, const Area & area, int)
{
  if (i<0) i=0;
  if (i==0) ppath->BeginRead();
  real x=0, y=0, rot=0;
  if (ppath->ReadNext(x, y, rot))
    SetPositionOnArea(x,y,rot, area);
}

//*****************************************************************************

#define ANGLE(i) ((i)*2*M_PI/ZONE)

M6Demo::M6Demo(int nb_stars, byte color, const Image * pima, PositionList & path)
: area_image(DIMX, DIMY), star_field(nb_stars, color), one(pima),
  tree(path)
{
  for (int i=0; i<NBOLT; i++)
    bolts[i].SetPositionOnArea(i, area);
  Rotation rot;
  for (int k=0; k<ZONE; k++)
    {
      rot.RotateEuler(0,0,ANGLE(k)+M_PI/ZONE);
      cubes[k].SetSize(CUBEDIM);
      cubes[k].SetColor(byte((k%2)?BLACK:WHITE));
      cubes[k].SetShading(3);
      cubes[k].SetPosition(rot,
			   Vector3(1.2*cos(ANGLE(k)+M_PI/ZONE),
				   1.2*sin(ANGLE(k)+M_PI/ZONE),
				   WALL-CUBEDIM));
    }
  one.Cinematiq(0,area,1);
  two.Cinematiq(0,area,1);
  tree.Cinematiq(0,area,1);
}

void M6Demo::Draw(const Camera & camera, ZDrawer & drawer, int draw_second)
{
  one.Update(camera);
  two.Update(camera);
  tree.Update(camera);
  Vector3 light(1,1,-1);
  drawer.Copy(area_image);
	if (draw_second)
	{
		one.Draw(camera, drawer, light);
		tree.Draw(camera, drawer, light);
		two.Draw(camera, drawer, light);
	}
}

void M6Demo::NewArea(const Camera & camera, byte clscolor)
{
  area.Update(camera);
  for (int j=0; j<NBOLT; j++)
    bolts[j].Update(camera);
  for (int l=0; l<ZONE; l++)
    cubes[l].Update(camera);
  Vector3 light(1,1,-1);
  area_image.Clear(clscolor);
  area.Draw(camera, area_image, light);
  for (int i=0; i<NBOLT; i++)
    bolts[i].Draw(camera, area_image, light);
  for (int k=0; k<ZONE; k++)
    cubes[k].Draw(camera, area_image, light);
  star_field.Draw(camera, area_image);
}

void M6Demo::SetVisibleBolt(int v)
{
  for (int i=0; i<NBOLT; i++)
    bolts[i].SetVisible(v);
}

void M6Demo::Intro1(Camera & camera, ZDrawer & drawer,
		   real teta, real phi, real psi,
		   int i1, int imax, byte clscolor)
{
  real x=-0.6*cos(M_PI/6), y=-0.6*sin(M_PI/6), rot=M_PI/6-M_PI/2;
  NewArea(camera, clscolor);
  one.Cinematiq(0,area,1);
  two.Cinematiq(0,area,1);
  tree.Cinematiq(0,area,1);

  MappedCube mc(one.iiemap);

  Rotation rota1;
  Rotation rota2;
  Rotation rota;
  Vector3 t, t1, t2;
  real zoom = camera.GetZoom();
  t2.Set(x, y, -PDIM);
  t1.Set(0, 0, 1.1/zoom);
  t1 = camera.ReturnToOldBase(t1);

  int i=i1;
  if (i1<0) i=0;
  Vector3 light(1,1,-1);

  drawer.Copy(area_image);
  two.Update(camera);
  two.Draw(camera, drawer, light);

  t = t2*(real(i)/real(imax))+t1*(real(imax-i)/real(imax));

  rota1.RotateEuler(2*M_PI*real(i)/real(imax),
                    4*M_PI*real(i)/real(imax),
            		    (6*M_PI+rot)*real(i)/real(imax));
  rota2.RotateEuler(real(imax-i)/real(imax)*(teta),
            		    real(imax-i)/real(imax)*(phi),
            		    real(imax-i)/real(imax)*(psi));
  rota = rota1*(!rota2);

  mc.SetSize(1/zoom*real(imax-i)/imax+PDIM*real(i)/imax);
  mc.SetPosition(rota, t);
  mc.Update(camera);
  mc.Draw(camera, drawer, Vector3());
}

void M6Demo::Intro2(Camera & camera, Drawer * drawer,
	real teta, real phi, real psi,
	int i1, int imax, byte clscolor)
{
  // area_image is used here to blur the drawer
  real x=-0.6*cos(M_PI/6), y=-0.6*sin(M_PI/6), rot=M_PI/6-M_PI/2;
  Vector3 light(1,1,-1);
  int i=i1;
  MappedCube mc(one.iiemap);
  Rotation rota1;
  Rotation rota2;
  Rotation rota;
  Vector3 t, t1, t2, t3;
  real zoom = camera.GetZoom();

  if (i1<0) i=0;
  i = imax-i-1;
  t2.Set(x, y, -PDIM);
  t1.Set(0, 0, 1.1/zoom);
  t1 = camera.ReturnToOldBase(t1);
  t3.Set(cos(2*M_PI*i/imax), sin(2*M_PI*i/imax),0);

  t = (t2+t3)*(real(i)/real(imax))+t1*(real(imax-i)/real(imax));

  rota1.RotateEuler(2*M_PI*real(i)/real(imax),
            		    4*M_PI*real(i)/real(imax),
            		    (6*M_PI+rot)*real(i)/real(imax));
  rota2.RotateEuler(real(imax-i)/real(imax)*(teta),
            		    real(imax-i)/real(imax)*(phi),
            		    real(imax-i)/real(imax)*(psi));
  rota = rota1*(!rota2);

  mc.SetSize(1/zoom*real(imax-i)/imax+PDIM*real(i)/imax);
  mc.SetPosition(rota, t);
  mc.Update(camera);
  ((Drawer*)(&area_image))->Copy(*drawer);
  drawer->BlurAttract(*((Drawer*)(&area_image)), clscolor, 243);
  static_cast<ZDrawer *>(drawer)->ClearZ();
  mc.Draw(camera, *(Drawer *)drawer, Vector3()); // in fact drawer is a ZDrawer
}


//*****************************************************************************

Messager::Messager(Fonts & pfont, int ptime, int pmaxmessages, byte pcolor)
{
  font = & pfont;
  time = ptime;
	max_messages = pmaxmessages;
	alloc(messages = new ::string [max_messages]);
	alloc(timer = new int [max_messages]);
	for (int i=0; i<max_messages; i++)
		{
			messages[i][0] = 0;
			timer[i] = 0;
		}
	color = pcolor;
	last = 0;
}

Messager::~Messager()
{
	delete [] messages;
	delete [] timer;
}

void Messager::PutMessage(::string message)
{
	strncpy(messages[last],message,255);
	// search the older message
	/**/
	int older=10000;
	int i;
	for (i=0; i<max_messages; i++)
		if (timer[i]<older)
		{
			last = i;
			older = timer[i];
		}
	timer[last] = time;
	/**/
	//last ++;
	//last %= max_messages;
}

void Messager::Draw(Drawer & drawer)
{
	int y = 0;
	int h = font->GetDimY();

	for (int i=0; i<max_messages; i++)
			if (timer[i]!=0)
				{
					drawer.WriteString(PointZ(0,y,color,0), *font, messages[i]);
					y += h;
				}

	DecreaseTime();
}

void Messager::DecreaseTime()
{
	for (int i=0; i<max_messages; i++)
		if (timer[i]>0) timer[i]--;
}

//*****************************************************************************

StarField::StarField(int nbstars, byte pcolor, int psize, real radius)
{
  nb_stars = nbstars;
  alloc(stars = new Plot [nbstars]);
  Rotation rot;
  for (int i=0; i<nb_stars; i++)
    {
      stars[i].Set(radius,0,0);
      stars[i].SetColor(color);
      real teta=2*M_PI*frand, phi=2*M_PI*frand, psi=2*M_PI*frand;
      rot.RotateEuler(teta, phi, psi);
      stars[i] = rot*stars[i];
    }
  color = pcolor;
  size = psize;
}

StarField::~StarField()
{
  delete [] stars;
}

void StarField::Draw(const Camera & camera, Drawer & drawer)
{
  Rotation rot = camera.GetRotation();
  for (int i=0; i<nb_stars; i++)
    {
      Plot cur = rot*(stars[i]);
      cur.ProjectionPerspective(camera.GetFocal(), drawer.MiddleX(),
                         			  drawer.MiddleY(), camera.GetZoom(),
                        			  minimum(drawer.Width(), drawer.Height()));
      cur.SetColor(color);
      if (cur[2] > 0)
   	    drawer.Box(cur.GetPointZ().SetZ(256), size,size);
    }
}
