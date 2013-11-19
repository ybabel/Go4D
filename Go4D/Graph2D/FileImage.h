#ifndef _FILEIMA_H_
#define _FILEIMA_H_

/**
*AUTHOR : Babel yoann
*TITLE  : fileima.h
*CREATED: 22/10/95
*DATE   : 15/01/01
*OBJECT : images that can be loaded from file. Add PCX, plus BMP and animated GIFs
*         support
*/

#include "Tools/Common.h"
#include "Image.h"
#include "Palette.h"
#include "Tools/Streams.h"

namespace Go4D
{

  /**
  *  all the image that can be loaded from the disk, with their palette
  *  this class also contains some function to manage the palette of the image.
  */
  class ImageFile : public Image
  {
    typedef Image inherited;
  public :
    ///  Swap : change all the pixel of the image to their new value
    void Swap(Color oldcolor, Color newcolor);
    ///
    const Palette & GetPalette() const;
    ///  ImportPalette : change the pixel of the image in order that the image can be
    ///  diplayed with the palette. Simple algo (Floid&Steinberg should be better).
    ImageFile & ImportPalette(const Palette & );

  protected :
    ImageFile():inherited(1,1,FALSE){} // buffer later by targa or bmp
    Palette fPal;
  };

  /**
  *PCX : load PCX images, exit in case of error
  */
  class Pcx : public ImageFile
  {
    typedef ImageFile inherited;
  private :
    void ReadPalette(istream & );
    void ReadData(istream & , bool );
    void ReadToken(istream &, byte &, short &);

  public :
    ///
    Pcx(char * filename);
  };


  /**
  * Bmp : load BMP images
  */
  class Bmp : public ImageFile
  {
    typedef ImageFile inherited;
  private :
    int Load(char *);

  public :
    ///
    Bmp(char * filename);
  };

  /**
  * GIF : load GIF images
  */
  class Gif : public ImageFile
  {
    typedef ImageFile inherited;
  private :

  public :
    ///
    Gif(char * filename);
  };

  /*****************************************************************************\
  *Inline methods, and properties
  \*****************************************************************************/

  inline const Palette & ImageFile::GetPalette() const
  {
    return fPal;
  }

}

#endif
