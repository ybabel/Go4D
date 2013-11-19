/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : image.cc, image.h implementation
 *CREATED: 22/10/95
 *DATE   : 08/03/96
 *BUGS   : - in Bmp() if image is not loaded, don't stop the program
 *         - Stereogram::operator= don't work ? (no repetition visile)
 *NOTE   : improvements inSDrawer :
 *         - make a segment-manager (allocating and freeing is not fast
 *           enough)
 *         - use a binary tree for each line instead of a list
 *         improvements in Image : in constructor add AutoAlloc parameter
\*****************************************************************************/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef LIBGPP
#include <iostream.h>
#include <fstream.h>
#endif

#include "image.h"


/*****************************************************************************\
 *   Font & Font6x10
\*****************************************************************************/


Fonts::Fonts(int x, int y, char * filename)
{
#ifdef LIBGPP
  dimx = x;
  dimy = y;
  alloc(chars = new byte [NBCHARS*dimy*dimx]);

  ifstream f(filename);
  byte * buf;
  alloc(buf = new byte[NBCHARS*dimy]); //1 byte for the 6 plots of line, 10 lines -> 10 byte

  f.read(buf, sizeof(byte)*NBCHARS*dimy);

  //convert in the appropriate format
  for (int i=0; i<NBCHARS; i++) // all the chars
    for (int j=0; j<dimy; j++) // all the lines of a char
      for (int k=0; k<dimx; k++) // all the pixel of a line
	chars[i*dimy*dimx+j*dimx+k] = byte(((buf[i*dimy+j]>>(7-k))&1)*ON);
  delete [] buf;
#endif
}

Fonts::~Fonts()
{
  delete [] chars;
}


/*****************************************************************************\
 *   Image
\*****************************************************************************/


Image::Image(int pWidth, int pHeight, int autoalloc)
{
  assert(pWidth>0 && pHeight>0);
  // set image dimension
  fWidth = pWidth;
  fHeight = pHeight;
  fLen = fWidth * fHeight;
  //allocated memory for buffer
  if (autoalloc)
    {alloc(fBuffer = new byte[fLen]);} //!!! alloc is a macro with an if inside
  else
    fBuffer = NULL;
  fBufChange = FALSE;
}

Image::~Image()
{
  // if necessary free the memory
  if (!fBufChange)
  {
    assert(fBuffer != NULL);
    // bug : the destructor is called 2 times
    delete [] fBuffer;
  }
}

void Image::NewBuffer(byte * pNewBuf)
{
  // detroy the old buffer, set fBuffer to the one passed in parameter
  if (fBuffer!=NULL)
    delete [] fBuffer;
  fBuffer = pNewBuf;
  fBufChange = TRUE;
}

void Image::Resize(int newsizex, int newsizey)
{
  assert(!fBufChange);
  assert(fBuffer != NULL);
  delete [] fBuffer;
  assert(newsizex>0 && newsizey>0);
  // set image dimension
  fWidth = newsizex;
  fHeight = newsizey;
  fLen = fWidth * fHeight;
  //allocated memory for buffer
  alloc(fBuffer = new byte[fLen]);
  fBufChange = FALSE;
}

/*****************************************************************************\
 *  Targa
\*****************************************************************************/


int Targa::In(byte r, byte g, byte b, int mxscan)
{
  // look if the color is already in the palette
  int i;
  for (i=0 ; i<=mxscan ; i++ )
    if ((fPal[i][0]==r) && (fPal[i][1]==g) && (fPal[i][2]==b))
      return(i);
  return(-1);
}

// remember that ImagePal don't allocate the buffer
Targa::Targa(char *name):ImagePal()
{
  FILE *hand ;
  int i,j,k,nbcc = -1;
  byte r,g,b;
  byte *tmp;

  // 0 - initialize the palette
  for (int l=0; l<256; l++)
    {
      fPal[l][0]=0 ; fPal[l][1]=0 ; fPal[l][2]=0;
    }

  // 1 - read the header
  byte header[18];
  if ((hand=fopen(name,"r"))==NULL)
    error1("Can't open",name);
  fread(header,sizeof(byte),18,hand); /* lit le header */
  if (header[2]!=2)
    error("Not a TARGA uncompressed RGB !");
  if (header[16]!=24)
    error("Not a TARGA 24bit !");

  // 2 - read image dimensions, alloc mem for image, free the old buffer
  fWidth=header[13]*256+header[12]; /* lo-hi */
  fHeight=header[15]*256+header[14]; /* lo-hi */
  fLen = fWidth*fHeight;
  alloc(fBuffer = new byte[fLen]);

  // 3 - read the image, if a color is not present in the palette
  // add if (if there is enough palce)
  tmp = fBuffer;
  for (j=0 ; j<fHeight ; j++)
    {
      for (i=0 ; i<fWidth ;i++)
      	{
      	  fscanf(hand,"%c%c%c",&b,&g,&r);
      	  k=In(r,g,b,nbcc);
      	  if (k==-1)
      	    {
      	      if (nbcc<=254)
            		{
            		  k=++nbcc;
            		  fPal[k][0]=r ; fPal[k][1]=g ; fPal[k][2]=b;
            		}
              else k=0 ;
      	    }
      	  *(tmp++)=(char)k;
      	}
    }
  fclose(hand);
}


/*****************************************************************************\
 *  Bmp
\*****************************************************************************/


#define BmpPal 54
#define BmpIma 54+1024


// remember that ImagePal don't allocate any buffer
Bmp::Bmp(char * file):ImagePal()
{
#ifdef LIBGPP
	ifstream f(file);
	byte buf[BmpIma]; //contains header + palette
	// 0 - load the file in a buffer
	f.read(buf, sizeof(char)*BmpIma);

	// 1 - get the image size
	fWidth = byte(buf[0x12])+ byte(buf[0x13])*256;
	fHeight = byte(buf[0x16])+ byte(buf[0x17])*256;
	fLen = fWidth*fHeight;
	alloc(fBuffer = new byte[fLen]);

	// 2 - get the palette
	for (int i=0; i<256; i++)
		{
      fPal[i][0] = buf[BmpPal+2+i*4];
      fPal[i][1] = buf[BmpPal+1+i*4];
			fPal[i][2] = buf[BmpPal+0+i*4];
    }

  // 3 - get the image itself
  byte * tmp;
  alloc(tmp = new byte[fLen]);
  f.read(tmp, sizeof(char)*fLen);
  for (int l=0; l<fLen; l++) fBuffer[l] = tmp[fLen-l];
  delete [] tmp;
#endif
}

