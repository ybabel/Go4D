/*****************************************************************************\
 *AUTHOR : Babel yoann Antras , babel@iie.cnam.fr
 *TITLE  : device.h, screen managment
 *CREATED: 22/10/95
 *DATE   : 15/11/95
 *OBJECT : screen handling, displaying window, palette handling
 *ACTUAL :
 *DEFS   : no defs, window version
 *NOTE   : the structure must be reviewed : 1 device <=> 1 image
 *         a device is not multi-image, so DisplayImage(Image *)
 *         will become Display(void), ect.
 *         look for a way of resizing the window, and to change the palette
 *         dynamically.
\*****************************************************************************/

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdlib.h>
#include <string.h>
#include "image.h"

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
#define kMLEFT 0x0012
#define kMMIDDLE 0x0013
#define kMRIGHT 0x0014
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

class Application
{
  protected :
  Image * image;
  Device * pdevice;

  public :
  Application(Device &);
  virtual void Draw() = 0;
  virtual void Cycle() = 0;
  virtual void OnKeyPressed(int keycode) = 0;
  virtual void OnMousePressed(int mousecode, int x, int y) = 0;
};

/* windows specific adds */

#define  STRICT
#include <windows.h>
#pragma hdrstop
#include "wingdll.h"

#define WinX 300
#define WinY 300


class Main
{
  public:
  static HINSTANCE hInstance;
  static HINSTANCE hPrevInstance;
  static int nCmdShow;
  static int MessageLoop(::Application & );
};

LRESULT CALLBACK _export WndProc( HWND hWnd, UINT iMessage,
											WPARAM wParam, LPARAM lParam );

class Window
{
  protected:
	HWND hWnd;

  public:
	HWND GetHandle( void ) { return hWnd; }
	BOOL Show( int nCmdShow ) { return ShowWindow( hWnd, nCmdShow ); }
	void Update( void ) { UpdateWindow( hWnd ); }
  virtual LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam ) = 0;
};

typedef struct Header
{
  BITMAPINFOHEADER  header;
  RGBQUAD           aColors[256];
} header;

class MainWindow : public Window
{
  protected:
  static char szClassName[64];
  WinGdll WinG;
  HDC Buffer;
  Header header;
  LPVOID pBits;
  HBITMAP hbm;
  HDC windc;
  HPALETTE pal;
  struct {
     WORD Version;
     WORD NumberOfEntries;
     PALETTEENTRY aEntries[256];
  } Palette;
  void NewPalette();
  void SetPalette();

  public:
  MainWindow( void );
  static void Register( void );
  virtual ~MainWindow( void );
  virtual LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
};

/* end window section */

class Device : public MainWindow
{
  private :
  LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
  ::Application * papplication;

  public :
  Device(void);
  ~Device(void);
  void SetApplication(::Application *);
  void AutoRepeatOff();
  void AcceptImage(Image *);
  void DisplayImage();
  void DefaultPalette();
  void RedPalette();
  void BlackWhitePalette();
  void VoxelPalette();
  void ImaPalette(const ImagePal & );
  void setcolor(byte col, byte r, byte g, byte b);
  int fWidth;
  int fHeight;
  int fNbColors;
  int fDepth;

  private :
  word Code(int);
};

#endif








