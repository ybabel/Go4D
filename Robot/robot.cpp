/*****************************************************************************\
 * M6 demo for Robot"s club by LeBab
\*****************************************************************************/

#include <fstream.h>
#include <stdio.h>

#include "robot.h"

#define STEP M_PI/64.0

#define vi0 4
//*****************************************************************************

/*
class MyBmp : public Bmp
{
	public :
	MyBmp(char * filename);
};

MyBmp iiebmp("iie.bmp");

MyBmp::MyBmp(char * filename) :Bmp(filename)
{
	for (int i=0; i<fLen; i++)
		if(fBuffer[i]==0) fBuffer[i]=WHITE; else fBuffer[i]=BLUE;
}
*/

//*****************************************************************************


M6RobotApp::M6RobotApp(Device & device)
: Application(device),
  drawer(WinX, WinY, 0, FALSE),
  iiebmp(20,12),
  font("font6x10.fon"),
  messager(font, 30, 5, WRITE),
  path(),
  m6demo(NBSTARS, STAR, &iiebmp, path),
  teta(-M_PI/3),
  phi(0),
  psi(-M_PI/3-M_PI/2-M_PI/6),
  z(-FOCAL*2),
  camera(teta, phi, psi, 0,0, z, FOCAL, FOCAL),
  cstset("robot.cst")
{
	image = & drawer;
        pdevice->fNbColors = 256;
	pdevice->AcceptImage(image); // buffer not allocated

	r = 0;
	linemode = 0;
	disphelp = 0;            
	drawing_mode = FALSE;
	iiebmp.Clear(BLUE);
	iiebmp.PutString(PointZ(iiebmp.Width()/2, iiebmp.Height()/2, WHITE),
			 font, "IIE");
	c = -5;

	#ifdef PALETTE
 	pdevice->DefaultPalette();
	#endif

	camera.NewAngle(teta,phi,psi);
	camera.NewPosition(0,0,z);

	SetConst();
	LoadConst();
}

void M6RobotApp::DispHelp()
{
	int y=0;
	int l=font.GetDimX()+4;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"+---- KEY MAP ---- HELP-------------------------+"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| <Z> toggle help display mode                  |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| <TAB> toggle drawing mode                     |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| <ENTER> flush position list                   |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| S/R save/load curve in file 'robot.crb'       |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| Q generate BioNut program in file 'robot.prg' |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| W load constants                              |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| L/H zoom in / zoom out                        |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"|                                               |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"|-In drawing mode-------------------------------|"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| <CURSOR> curve left, right                    |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"|          choose curent segment                |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| 2/1 increase/decrease speed                   |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| F/B increase/decrease time                    |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| <DEL> delete last segment                     |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"|-In simulation mode----------------------------|"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| <CURSOR> change POV                           |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"| F/B rotate POV                                |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"|                                               |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"|     <A>    LAUNCH DEMO  !!!!!!!               |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"|                                               |"); y+=l;
	drawer.WriteString(PointZ(0,y,WRITE,0),font,"+-----------------------------------------------+");
}

void M6RobotApp::SetConst()
{
	cstset.Add("convtime", Ereal, &convtime);
	cstset.Add("inittime", Ereal, &inittime);
	cstset.Add("deltatime", Ereal, &deltatime);
	cstset.Add("deltaangle", Ereal, &deltaangle);
	cstset.Add("adjusttime", Ereal, &adjusttime);
	cstset.Add("angle0",Ereal,&angle0);
	cstset.Add("tabv",Ertab,tabv);
	cstset.Add("taba",Ertab,tabtype);
	cstset.Add("xinit",Ereal,&xinit);
	cstset.Add("yinit",Ereal,&yinit);
}

void M6RobotApp::LoadConst()
{
	cstset.Load();
	path.SetConst( convtime, inittime, deltatime, deltaangle, adjusttime,angle0,tabv,tabtype);
	if (path.Empty())
		path.AddPosition(xinit, yinit, vi0, angle0);
	m6demo.one.SetAngle(M_PI/2+angle0);
	m6demo.one.SetInitPos(xinit, yinit);
}

void M6RobotApp::Generate()
{
	ofstream output("robot.prg");
	output << path;
}

void M6RobotApp::OnKeyPressed(int keycode)
{
	switch (keycode)
		{
		case kKS :
			strncpy(mess,"saving position list !!", 255);
			messager.PutMessage(mess);
			path.Save();
			break;
		case kKR :
			strncpy(mess,"restore position list !!", 255);
			messager.PutMessage(mess);
			path.Load();
			break;
		case kKZ :
			disphelp = 1-disphelp;
			break;
		case kLF :
			if (drawing_mode)
			{
				strncpy(mess,"flushing position list !!", 255);
				messager.PutMessage(mess);
				path.Flush();
				path.AddPosition(xinit, yinit, vi0, angle0);
        pdevice->RedPalette();
			}
			break;
		case kDEL :
			if (drawing_mode)
			{
				strncpy(mess,"deleting last position !!",255);
				messager.PutMessage(mess);
				if (path.GetNbPositions()>=2)
				{
					path.DelLast();
					path.Previous();
				}
        pdevice->DefaultPalette();
			}
			break;
		case kSP :
			if (!drawing_mode)
			{
				strncpy(mess,"timer initialization",255);
				messager.PutMessage(mess);
				c = -5;
				nc.Set(0,0,0);
				m6demo.area.SetCenter(nc);
			}
			else
			{
				strncpy(mess,"adding one position",255);
				messager.PutMessage(mess);
				path.AddPosition(0,0,vi0,0); path.Next();
			}
			break;
		case kTB :
			drawing_mode = 1-drawing_mode;
			linemode = 1-linemode;
			if (linemode)
				strncpy(mess,"path drawing mode",255);
			else
				strncpy(mess,"path drawing canceled",255);
			c = 0;
			messager.PutMessage(mess);
			break;
		case kUP :
			if (!drawing_mode)
			{
				teta+=STEP;
				camera.NewAngle(teta,phi,psi);
				m6demo.NewArea(camera, CLS);
			}
			else
			{
				sprintf(mess,"current segment %d",path.Next());
				messager.PutMessage(mess);
			}
			break;
		case kDN :
			if (!drawing_mode)
			{
				teta-=STEP;
				camera.NewAngle(teta,phi,psi);
				m6demo.NewArea(camera, CLS);
			}
			else
			{
				sprintf(mess,"current segment %d",path.Previous());
				messager.PutMessage(mess);
			}
			break;
		case kLE :
			if (!drawing_mode)
			{
				phi += STEP;
				camera.NewAngle(teta,phi,psi);
				m6demo.NewArea(camera, CLS);
			}
			else
			{
				sprintf(mess,"segment curve %d",path.CurvLeft());
				messager.PutMessage(mess);
			}
			break;
		case kRI :
			if (!drawing_mode)
			{
				phi -= STEP;
				camera.NewAngle(teta,phi,psi);
				m6demo.NewArea(camera, CLS);
			}
			else
			{
				sprintf(mess,"segment curve %d",path.CurvRight());
				messager.PutMessage(mess);
			}
			break;
		case kFW :
			if (!drawing_mode)
			{
				psi += STEP;
				camera.NewAngle(teta,phi,psi);
				m6demo.NewArea(camera, CLS);
			}
			else
			{
				sprintf(mess,"segment time increased %f",path.IncrTime());
				messager.PutMessage(mess);
			}
			break;
		case kBW :
			if (!drawing_mode)
			{
				psi -= STEP;
				camera.NewAngle(teta,phi,psi);
				m6demo.NewArea(camera, CLS);
			}
			else
			{
				sprintf(mess,"segment time decreased %f",path.DecrTime());
				messager.PutMessage(mess);
			}
			break;
		case kK2 :
			if (drawing_mode)
			{
				sprintf(mess,"segment speed increased %d",path.SpeedUp());
				messager.PutMessage(mess);
			}
			break;
		case kK1 :
			if (drawing_mode)
			{
				sprintf(mess,"segment speed decreased %d",path.SlowDown());
				messager.PutMessage(mess);
			}
			break;
		case kKW :
			strncpy(mess,"loading constants",255);
			messager.PutMessage(mess);
			LoadConst();
			break;
		case kKQ :
			strncpy(mess,"generating program",255);
			messager.PutMessage(mess);
			Generate();
			break;
		case kLO :
			z += 0.5;
			camera.NewPosition(0,0,z);
			m6demo.NewArea(camera, CLS);
			break;
		case kHI :
			z -= 0.5;
			camera.NewPosition(0,0,z);
			m6demo.NewArea(camera, CLS);
			break;
		case kKA :
			m6demo.area_image.Clear(CLS);
			drawer.Clear(CLS);
			for (int i=0; i<INTRON; i++)
				{
					m6demo.Intro2(camera, (Drawer*)&drawer, teta, phi, psi, i, INTRON, CLS);
					pdevice->DisplayImage();
				}
			m6demo.area_image.Clear(CLS);
			drawer.Clear(CLS);
			m6demo.NewArea(camera, CLS);
			for (int j=0; j<INTRON; j++)
				{
					m6demo.Intro1(camera, drawer, teta, phi, psi, j, INTRON, CLS);
					pdevice->DisplayImage();
				}
			c=0;
			break;
		}
}

void M6RobotApp::OnMousePressed(int , int , int )
{
}

void M6RobotApp::Draw()
{
	if (c>=3) m6demo.SetVisibleBolt(FALSE);else m6demo.SetVisibleBolt(TRUE);
	if(c==-5||c==3) m6demo.NewArea(camera, CLS);

	camera.NewAngle(teta,phi,psi);
	camera.NewPosition(0,0,z);
	if (!drawing_mode)
	{
		m6demo.one.Cinematiq(c,m6demo.area,21);
		m6demo.two.Cinematiq(c,m6demo.area,21);
		m6demo.tree.Cinematiq(c-21, m6demo.area, 0);
	}
	m6demo.Draw(camera, drawer, !drawing_mode);
	messager.Draw(drawer);
	if (drawing_mode) path.Trace(drawer, camera, m6demo.area);
	if (disphelp) DispHelp();
	pdevice->DisplayImage();
}

void M6RobotApp::Cycle()
{
	Draw();
	c++;
}

