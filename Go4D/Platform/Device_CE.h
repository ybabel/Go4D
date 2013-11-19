#ifndef _DEVICE_CE_H_
#define _DEVICE_CE_H_

/**
*AUTHOR : Babel yoann Antras , babel@iie.cnam.fr
*TITLE  : device.h, screen managment
*CREATED: 17/12/00
*DATE   : 17/12/00
*OBJECT : screen handling, displaying window
*         ported to PocketPC
*/

#include "Device.h"

#include <windows.h>
#include <commctrl.h>
#include <aygshell.h>
#include <sipapi.h>
#include <stdlib.h>
#include <string.h>
#include "gx.h"

namespace Go4D
{


#define MENU_HEIGHT 26
#define MAX_LOADSTRING 100

  class Main
  {
  public:
    static HINSTANCE hInstance;
    static HINSTANCE hPrevInstance;
    static int nCmdShow;
    static int MessageLoop(Application & );

  };

  class Window : public Device
  {
  protected:
    HWND hWnd;

  public:
    HWND GetHandle( void ) { return hWnd; }
    BOOL Show( int nCmdShow ) { return ShowWindow( hWnd, nCmdShow ); }
    void Update( void ) { UpdateWindow( hWnd ); }
    virtual LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam ) = 0;
  };

  class MainWindow : public Window
  {
    typedef Window inherited;
  protected:
    static char szClassName[64];
    HDC hdc;
    PAINTSTRUCT ps;
    TCHAR szHello[MAX_LOADSTRING];
    short vkKey; 
    GXDisplayProperties g_gxdp;		// GX struct
    GXKeyList g_gxkl;				// GX struct

    HPALETTE hPalette;
    HPALETTE hOldPal;
    LPLOGPALETTE lpMem;  
    byte * gxBuffer;

    bool HasPalette;
    void NewPalette();
    void SetPalette();

    void CopyPalette(const Palette *pPal=NULL);

  public:
    MainWindow(int & Width, int & Height, const Palette *pPal=NULL);
    static void Register( void );
    virtual ~MainWindow( void );
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
  };

  /* end window section */

  class DeviceCE : public MainWindow
  {
    typedef MainWindow inherited;
  public : //only to be called by winproc
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    Application * papplication;

  public :
    DeviceCE(int Width, int Height, const Palette *p=NULL);
    ~DeviceCE();
    void SetApplication(Application *);
    void AutoRepeatOff();
    void AcceptImage(Image *);
    void DisplayImage();
    void ChangeCurrentPalette(const Palette *pPal=NULL);
    int Width() const { return fWidth; }
    int Height() const { return fHeight; }
    void StartChrono();
    int GetChrono(); // in milisecond, 1 day max
    void Close();

    static DeviceCE* MainDevice;

  private :
    SYSTEMTIME fChronoTime;
    word Code(int);
    int MouseCode(int);
    int fWidth;
    int fHeight;
    int fNbColors;
    int fDepth;
    bool fDraging;
    bool fHaveMoved;
    int fDragX;
    int fDragY;
    Image * fImage;
  };

}

#endif