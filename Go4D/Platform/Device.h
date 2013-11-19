/*****************************************************************************\
*AUTHOR : Babel yoann Antras , babel@iie.cnam.fr
*TITLE  : Device.h, screen managment
*CREATED: 22/10/95
*DATE   : 15/11/95
*OBJECT : screen handling, displaying window
\*****************************************************************************/

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "Graph2D/Image.h"
#include "Graph2D/Palette.h"

namespace Go4D
{

#define kMaxWin 16
#define kEND 0x00ff
#define kNONE 0x00fe
#define kUP 0x0001
#define kDN 0x0002
#define kRI 0x0003
#define kLE 0x0004
#define kFW 0x0005
#define kBW 0x0006
#define kHI 0x0007
#define kLO 0x0008
#define kSP 0x0009
#define kKU 0x000a
#define kKI 0x000b
#define kKQ 0x000c
#define kKW 0x000d
#define kKA 0x000e
#define kKZ 0x000f
#define kTB 0x0010
#define kLF 0x0011
#define kUPMask 0x8000
#define kDNMask 0x4000
#define kLEMask 0x2000
#define kRIMask 0x1000
#define kKeyMask 0x00ff
#define kMLEFT 0x0001
#define kMMIDDLE 0x0002
#define kMRIGHT 0x0004
#define kDEL 0x0015
#define kKT 0x0016
#define kKP 0x0017
#define kKN 0x0018
#define kKC 0x0019
#define kKV 0x001A
#define kPL 0x001B
#define kMN 0x001C
#define kK1 0x001D
#define kK2 0x001E
#define kK3 0x001F
#define kK4 0x0020
#define kK5 0x0021
#define kK6 0x0022
#define kK7 0x0023
#define kK8 0x0024
#define kK9 0x0025
#define kK0 0x0026
#define kKS 0x0027
#define kKR 0x0028
#define kCtrl 0x0100
#define kShift 0x0200

  class Device;

  /**
  * An application. The device will send some events to it's main
  * application. So to implement an application ,you must inherit
  * this interface
  */
  class Application
  {
  protected :
    Device * fDevice;

  public :
    Application(Device &);
    virtual void Draw() = 0;
    virtual void Cycle() = 0;
    virtual void OnKeyPressed(int keycode) = 0;
    virtual void OnMouseClick(int mousecode, int x, int y) = 0;
    virtual void OnMouseDblClick(int mousecode, int x, int y) = 0;
    virtual void OnDragStart(int mousecode, int x, int y) = 0;
    virtual void OnDragOver(int mousecode, int dx, int dy) = 0;
    virtual void OnDragEnd(int mousecode, int x, int y) = 0;
    int Launch();
  };

  /**
  * An abstraction layer that allow to be independent from the device
  * where the application run. Currently the device work for :
  * WIN32, POCKETPC, and XWINDOW
  * so an application can run indifferently on one or another platform
  */
  class Device
  {
  protected :
    const Palette * fPalette;

  public :
    virtual void SetApplication(Application *)=0;
    virtual void AutoRepeatOff()=0;
    // change the buffer of the image to the one of the Device
    virtual void AcceptImage(Image *)=0;
    virtual void DisplayImage()=0;
    virtual void ChangeCurrentPalette(const Palette *pPal=NULL)=0;
    virtual int Width() const =0;
    virtual int Height() const =0;
    virtual void StartChrono()=0;
    virtual int GetChrono()=0; // in milisecond, 1 day max
    const Palette * GetPalette() const { return fPalette; }
    virtual void Close()=0;
    virtual int MainLoop()=0;
  };

  Device * NewDevice(int Width, int Height, const Palette *p=NULL);

}

#endif
