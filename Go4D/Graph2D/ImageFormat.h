#ifndef _IMAGE_FORMAT_H_
#define _IMAGE_FORMAT_H_


// ****************************************************************************
//
// WINIMAGE.H : Generic classes for raster images (MSWindows specialization)
//
//  Content: Class declarations of:
//  - class C_Image             : Storage class for single images
//  - class C_ImageSet          : Storage class for sets of images
//  - class C_AnimationWindow   : Window Class to display animations
//
//  (Includes declarations of routines to Load and Save BMP files and to load
// GIF files into these classes).
//
//  --------------------------------------------------------------------------
//
// Copyright © 2000, Juan Soulie <jsoulie@cplusplus.com>
//
// Permission to use, copy, modify, distribute and sell this software or any
// part thereof and/or its documentation for any purpose is granted without fee
// provided that the above copyright notice and this permission notice appear
// in all copies.
//
// This software is provided "as is" without express or implied warranty of
// any kind. The author shall have no liability with respect to the
// infringement of copyrights or patents that any modification to the content
// of this file or this file itself may incur.
//
// ****************************************************************************

namespace Go4D
{


  struct COLOR {unsigned char b,g,r,x;};	// Windows GDI expects 4bytes per color
#define ALIGN sizeof(int)				// Windows GDI expects all int-aligned

  /**
  * C_Image
  *    Storage class for single images
  */
  class C_Image {
  public:
    // standard members:
    int Width, Height;			// Dimensions in pixels
    int BPP;					// Bits Per Pixel
    char * Raster;				// Bits of Raster Data (Byte Aligned)
    COLOR * Palette;			// Color Map
    int BytesPerRow;			// Width (in bytes) including alignment!
    int Transparent;			// Index of Transparent color (-1 for none)
    // Extra members for animations:
    int xPos, yPos;				// Relative Position
    int Delay;					// Delay after image in 1/1000 seconds.
    int Transparency;			// Animation Transparency.

    // constructor and destructor:
    C_Image() { Raster=0; Palette=0; }
    ~C_Image() { delete[]Raster; }

    // operator= (object copy)
    C_Image& operator= (C_Image& rhs);

    // Image initializer (allocates space for raster and palette):
    void Init (int iWidth, int iHeight, int iBPP);

    inline char& Pixel (const int& x, const int& y)
    {return Raster[y*BytesPerRow+x];}

    // File Formats:
    int LoadBMP (char* szFile);
    int SaveBMP (char* szFile);
  };

  /**
  * C_ImageSet
  *    Storage class for sets of images
  */
  class C_ImageSet {
  public:
    int FrameWidth, FrameHeight;	// Dimensions of ImageSet in pixels.
    int nLoops;						// Number of Loops (0 = infinite)

    C_Image ** img;				// Images' Vector.
    int nImages;					// Number of images (vector size)

    void AddImage (C_Image*);		// Append new image to vector (push_back)

    // constructor and destructor:
    C_ImageSet() {img=0; nImages=0; nLoops=0;}
    ~C_ImageSet() {for (int n=0;n<nImages;n++) delete img[n]; delete[] img;}

    // File Formats:
    int LoadGIF (char* szFile);
    int SaveGIF (char* szFile) {return 0;};	// NOT IMPLEMENTED
  };

}

#endif
