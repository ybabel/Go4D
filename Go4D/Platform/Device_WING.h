#ifndef _DEVICEWING_H_
#define _DEVICEWING_H_

#include "Device.h"

#define  STRICT
#include <windows.h>
#pragma hdrstop
#include <stdlib.h>
#include <string.h>
#include "Wingdll.h"

namespace Go4D
{


  class Main
  {
  public:
    static HINSTANCE hInstance;
    static HINSTANCE hPrevInstance;
    static int nCmdShow;
    static int MessageLoop(Application & );
  };

#ifdef VSNET
  LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam );
#else
  LRESULT CALLBACK _export WndProc( HWND hWnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam );
#endif

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

  typedef struct Header
  {
    BITMAPINFOHEADER  header;
    RGBQUAD           aColors[256];
  } header;

  class MainWindow : public Window
  {
    typedef Window inherited;
  protected:
    static char szClassName[64];
    WinGdll WinG;
    HDC Buffer;
    Header header;
    LPVOID pBits;
    HBITMAP hbm;
    HDC windc;
    bool HasPalette;
    void NewPalette();
    void SetPalette();
    void CopyPalette(const Palette *pPal=NULL);

  public:
    MainWindow(int & Width, int & Height, const Palette *pPal=NULL);
    static void Register( void );
    virtual ~MainWindow( void );
    virtual LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
  };

  class WinGDevice : public MainWindow
  {
    typedef MainWindow inherited;
  private :
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    Application * papplication;

  public :
    WinGDevice(int Width, int Height, const Palette *p=NULL);
    ~WinGDevice(void);
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
  };

}

#endif