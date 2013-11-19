#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "device.h"
#include "objlist.h"
#include "res.h"

class M6RobotApp : public ::Application
{
	private :
	ZDrawer drawer;
	Drawer iiebmp;
	Font6x10 font;
	Messager messager;
	PositionList path;
	M6Demo m6demo;
	real teta, phi, psi, z;
	Camera camera;
	ConstSet cstset;
	int r;
	Plot nc;
	::string mess;
	int linemode;
	int c;
	int drawing_mode, disphelp;

	public :
	M6RobotApp(Device &);
	void Draw();
	void Cycle();
	void OnKeyPressed(int keycode);
	void OnMousePressed(int mousecode, int x, int y);
	void SetConst();
	void LoadConst();
  	void Generate();
	void DispHelp();

	real convtime, inittime, deltatime, deltaangle, adjusttime;
	real angle0,tabv[32],tabtype[10];
	real xinit, yinit;
};

#endif