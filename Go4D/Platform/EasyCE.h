// EasyCE.h : API
// By Jacco Bikker, a.k.a. "The Phantom"
// --------------------------------------------------

#ifndef EASYCE_H
#define EASYCE_H

#if _MSC_VER > 1000
#pragma once
#endif

#define WIN32_LEAN_AND_MEAN

// Colors
#define BLACK	0
#define DGREY	1
#define LGREY	2
#define WHITE	3

#if _WIN32_WCE==300
	#pragma message ( "Compiling for PocketPC (3.0, using GAPI)" )
	#define USE_GAPI
#elif _WIN32_WCE==211
	#pragma message ( "Compiling for PalmSize PC (2.11, using GAMEX)" )
	#define USE_GAMEX
#elif _WIN32_WCE==201
	#pragma message ( "Compiling for PalmSize PC (2.01, using GAMEX)" )
	#define USE_GAMEX
#endif

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// EasyCE API
void error( char* s );
void error_nr( char* s, int n );
void update( bool updscr = true );
bool loadtga( const unsigned short* file );
bool loadtga( const unsigned short* file, int w, int h, unsigned short* dest, bool pal );
void snapshot();
unsigned long* getbuffer( void );
unsigned long* getscrptr( void );
void clear( int color );
void print( char* a_String, int x1, int y1, int color );
void print( char* a_String, int color );
void print( char* a_String, int color, unsigned long* buff );
void line( float x1, float y1, float x2, float y2, int color );
void plot( int x1, int y1, int color );
void hline( int x1, int y1, int len, int color );
void vline( int x1, int y1, int len, int color );
void box( int x1, int y1, int x2, int y2, int color );
void bar( int x1, int y1, int x2, int y2, int color );
void bar( int x1, int y1, int x2, int y2, int color, unsigned long* buff );
void line_x( float x1, float y1, float x2, float y2, int color );
void hline_x( int x1, int y1, int len, int color );
void vline_x( int x1, int y1, int len, int color );
void box_x( int x1, int y1, int x2, int y2, int color );
void bar_x( int x1, int y1, int x2, int y2, int color );
void plot_x( int x1, int y1, int color );
int getpixel( int x1, int y1 );
bool clicked( void );
int clickxpos( void );
int clickypos( void );
int cursorxpos( void );
int cursorypos( void );
void cursorxpos( int x );
void cursorypos( int y );
int penxpos( void );
int penypos( void );
bool pendown( void );
void taskbar( bool show );
void resettimer( void );
int msecs( void );
void play( unsigned short* s );
void stopsound( void );
// End of EasyCE API

#ifdef USE_GAMEX
	// GameX API
	#pragma once
	const unsigned long kfDPGrey =		0x0001;
	const unsigned long kfDPGrey2Bit =	0x0002;
	const unsigned long kfDPGrey4Bit =	0x0004;
	const unsigned long kfDPColor =		0x0008;
	const unsigned long kfDPColor8Bit =	0x0010;
	const unsigned long kfDPColor16Bit = 0x0020;
	const unsigned long kfDPColor24Bit = 0x0040;
	const unsigned long kfDPColor32Bit = 0x0080;
	const unsigned long kfDPFormatNormal = 0x0100;
	const unsigned long	kfDPFormatRot270 = 0x0200;
	const unsigned long kfMPPSPC =	0x0001;
	const unsigned long kfMPPSPC1 =	0x0002;
	const unsigned long kfMPPSPC2 =	0x0004;
	const unsigned long kfMPHPC =	0x0008;
	const unsigned long kfMPHPC1 =	0x0010;
	const unsigned long kfMPHPC2 =	0x0020;
	const unsigned long kfMPHPC3 =	0x0040;
	const unsigned long kfMPPro =	0x0080;
	const unsigned long kfMPAutoPC = 0x0100;
	const unsigned long kfMPHasKeyboard = 0x0200;
	const unsigned long kfMPHasMouse = 0x0400;
	const unsigned long kfMPHasRumble = 0x0800;
	const unsigned long kfMPHasTouch = 0x1000;
	const int kiRotate0 = 0;
	const int kiRotate90 = 1;
	const int kiRotate180 = 2;
	const int kiRotate270 = 3;
	class GameX {
	public:
		HWND SetButtonNotificationWindow(HWND hWnd);
		GameX();
		~GameX();
		bool OpenGraphics();
		bool OpenSound();
		bool OpenButtons(HWND hwndButtonNotify);
		bool CloseGraphics();
		bool CloseSound();
		bool CloseButtons();
		bool IsColor();
		bool IsPSPC();
		bool IsHPC();
		bool IsHPCPro();
		bool HasMouse();
		bool HasKeyboard();
		bool HasRumble();
		bool HasTouch();
		int IsForeground();
		bool Suspend();
		bool Resume();
		bool BeginDraw();
		bool EndDraw();
		bool FindFrameBuffer();
		void * GetFBAddress();
		long GetFBModulo();
		long GetFBBpp();
		bool GetScreenRect(RECT * prc);
		unsigned long GetDisplayProperties();
		bool GetButton(int VK);
		unsigned short GetDefaultButtonID(long id, long rotate);
		bool ReleaseButton(int VK);
		bool BeginDetectButtons();
		bool EndDetectButtons();
		bool Rumble();
		bool Backlight(bool fOn);
	private:
		int m_iMP;
		void * m_pvFrameBuffer;
		long m_cbFBModulo;
		unsigned long m_ffMachineProperties;
		unsigned long m_ffDisplayProperties;
		int m_cBitsPP;
		long m_dwPrevMode;
		bool m_fActive;
		HWND m_hwndTaskbar;
	};
	// End of GameX API
#endif

#ifdef USE_GAPI
	// GAPI
	#ifdef GXDLL_EXPORTS
	#define GXDLL_API __declspec(dllexport)
	#else
	#define GXDLL_API __declspec(dllimport)
	#endif
	struct GXDisplayProperties 
	{
		DWORD cxWidth;
		DWORD cyHeight;
		long cbxPitch;
		long cbyPitch;
		long cBPP;
		DWORD ffFormat;
	};
	struct GXKeyList 
	{
		short vkUp;
		POINT ptUp;
		short vkDown;
		POINT ptDown;
		short vkLeft;
		POINT ptLeft;
		short vkRight;
		POINT ptRight;
		short vkA;
		POINT ptA;
		short vkB;
		POINT ptB;
		short vkC;
		POINT ptC;
		short vkStart;
		POINT ptStart;
	};
	struct GXScreenRect 
	{
		DWORD dwTop;
		DWORD dwLeft;
		DWORD dwWidth;
		DWORD dwHeight;
	};
	GXDLL_API int GXOpenDisplay(HWND hWnd, DWORD dwFlags);
	GXDLL_API int GXCloseDisplay();
	GXDLL_API void * GXBeginDraw();
	GXDLL_API int GXEndDraw();
	GXDLL_API int GXOpenInput();
	GXDLL_API int GXCloseInput();
	GXDLL_API GXDisplayProperties GXGetDisplayProperties();
	GXDLL_API GXKeyList GXGetDefaultKeys(int iOptions);
	GXDLL_API int GXSuspend();
	GXDLL_API int GXResume();
	GXDLL_API int GXSetViewport( DWORD dwTop, DWORD dwHeight, DWORD dwReserved1, DWORD dwReserved2 );
	GXDLL_API BOOL GXIsDisplayDRAMBuffer();
	#define GX_FULLSCREEN	0x01
	#define GX_NORMALKEYS	0x02
	#define GX_LANDSCAPEKEYS	0x03
	#ifndef kfLandscape
		#define kfLandscape	0x8			// Screen is rotated 270 degrees
		#define kfPalette	0x10		// Pixel values are indexes into a palette
		#define kfDirect	0x20		// Pixel values contain actual level information
		#define kfDirect555	0x40		// 5 bits each for red, green and blue values in a pixel.
		#define kfDirect565	0x80		// 5 red bits, 6 green bits and 5 blue bits per pixel
		#define kfDirect888	0x100		// 8 bits each for red, green and blue values in a pixel.
		#define kfDirect444	0x200		// 4 red, 4 green, 4 blue
		#define kfDirectInverted 0x400
	#endif
	// End of GAPI
#endif

// Unified windows
void itoa( int v, char* s );
double strtod( char* t, char** e );
int stricmp( char* s1, char* s2 );
// End of unified windows stuff

#endif

// --------------------------------------------------
// EOF