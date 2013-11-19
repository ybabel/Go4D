/*
  AUTHOR : Babel yoann Antras , babel@iie.cnam.fr
  TITLE  : device.h, screen managment
  CREATED: 22/10/95
  DATE   : 15/11/95
  OBJECT : screen handling, displaying window, palette handling
  ACTUAL : key handling added (Wait)
           defines createcolor share school
 */

#ifndef _DEVICEX_H_
#define _DEVICEX_H_

#include <sys/time.h>
#include <unistd.h>
//#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include "Device.h"


namespace Go4D
{


class DeviceX : public Device
{
private :
  Application *papplication;

public :
  DeviceX(int Width, int Height, const Palette *p=NULL);
  ~DeviceX(void);
  virtual void SetApplication(Application *);
  virtual void AutoRepeatOff();
  virtual void AcceptImage(Image *);
  virtual void DisplayImage();
  virtual void ChangeCurrentPalette(const Palette *pPal=NULL);
  virtual int  Width() const { return fWidth; }
  virtual int  Height() const { return fHeight; }
  virtual void StartChrono();
  virtual int  GetChrono();       // in millisecond, 1 day max !!Second
  virtual void Close();
  virtual int  MainLoop();

private :
  timeval fChronoTime;
  word Code(int);
  int MouseCode(int);

  bool     fDraging, fHaveMoved;
  int      fDragX, fDragY;
  int      fButton;
  bool     fCtrlPressed, fShiftPressed;

  int      fWidth, fHeight;
  int      fNbColors;
  int      fDepth;
  int      fScreen;
  int      fMainWin;

  Visual   *fVisual;
  GC       fgc;
  Display  *fDisplay;
  Window   fXWindow;
  XImage   *fXImage;
  Image    *fWindow;
  Colormap fColormap;
#ifdef share
  XShmSegmentInfo fShmInfo;
#endif
};


};

#endif
