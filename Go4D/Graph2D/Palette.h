#ifndef _PALETTE_H_
#define _PALETTE_H_

/**
*AUTHOR : Babel yoann
*TITLE  : Palette.h, Palette managment
*CREATED: 27/06/96
*DATE   : 27/06/96
*OBJECT : Palette and color management. RGBColor operation, Palette,
*         PaletteBuilder
*USAGE  : For example if you want to build a palette for many images, and their
*         shaded versions.
*         - Read the images from disk.
*         - create as many CountPalette as yuo have images, initialize them
*           with the image palette and use AddImage for each association.
*         - Create a PaletteBuilder large enough to receive all these palette
*           and their shaded versions.
*         - Add all the palettes (CountPalette) you will use
*         - Get the Best palette.
*         - Now you can get rid of the PaletteBuilder and all the CountPalettes
*/

#include "Tools/Common.h"
#include "Graph2D/Image.h"

namespace Go4D
{

  /**
  * RGBColor :
  * This class must support all colors operations, like determining the distance
  * (in a 3D space) between 2 colors, combinig 2 colors (with an OR or an AND).
  * The value of the colors are ALWAYS : 0<=value<=255
  * The class is not optimized, and there is no need for. For color managment
  * differents operators have been redefined.
  *
  * NOTE : for example if you want to perform the color that is resulting from
  *   the combination of 2 transparent colors, if you only use | operator you
  *   will probably get a more luminous color, so you can readapt the luminosity
  *   use the Abs methods and * operator.
  *   If you want to darken a color by 25% then multiply it by 0.75
  */
  class RGBColor
  {
    friend class Palette;
  private :
    byte r, g, b;

  public :
    ///
    RGBColor();
    ///
    RGBColor(byte r, byte g, byte b);
    ///
    RGBColor(const RGBColor &);
    ///
    RGBColor & operator = (const RGBColor &);
    ///
    bool operator == (const RGBColor &) const;
    ///
    bool operator != (const RGBColor &) const;
    ///
    RGBColor operator | (const RGBColor &) const;
    ///  operator + is the same operator than |, but this one set the luminosity of the result color to the min of the 2 operands
    RGBColor operator + (const RGBColor &) const;
    ///  operator - perform the difference between the 2 vectors (if there is a negative value it is automatically set positive)
    RGBColor operator - (const RGBColor &) const;
    ///  c=c1*c2/256
    RGBColor operator ^ (const RGBColor &) const;
    /** operator * (by a real) mutiply the vector by one real. The real must be
    * positive, if the result is grater than 255 in one coordinate, it is
    * automatically truncated. You can also use *= operator.
    */
    RGBColor operator * (real) const;
    ///
    RGBColor & operator *= (const RGBColor &);
    ///
    RGBColor & operator *= (real);
    ///  method Abs return the norm of the vector (a real)
    real Abs() const;
    ///  mehtod Norm return the norm2 (abs of the sum) (a int)
    int Norm() const;
    ///
    byte & R() { return r; }
    ///
    byte & G() { return g; }
    ///
    byte & B() { return b; }
#ifdef DEBUGOUT
    ///
    friend ostream & operator << (ostream &, const RGBColor &);
    ///
    ostream & DebugOut(ostream &) const;
#endif
  };

  /**
  * PALETTE
  * A palette is an array (256 entries) of RGBColor. It's used for displaying images
  * on to screen. Moreover, most of the time, images stored on disk (bmp, gif,..)
  * also have palettes.
  * This class contains the same operators than RGBColor class, minus Abs, wich has no
  * signification here. The operators have 2 version : the first categories work
  * with 2 palette, the RGBColor operator is applied for each entries of the palette;
  * the other version applies on one Palette and one RGBColor, all the entries of the
  * palette are combined with the parameter.
  * There are also some predifined palettes to simplify their usage.
  */
  class Palette
  {
  protected :
    RGBColor * fEntries;
    int fCount;

  public :
    ///  Palette : the default constructor set all the Count entries to (0,0,0)
    Palette(int Count=256);
    ///  Palette : the default constructor set all the Count entries to (0,0,0)
    Palette(const Palette &);
    ///
    virtual ~Palette();
    ///
    Palette & operator = (const Palette &);
    ///
    int operator == (const Palette &) const;
    ///
    int operator != (const Palette &) const;
    ///
    Palette operator | (const Palette &) const;
    ///
    Palette operator + (const Palette &) const;
    ///
    Palette operator - (const Palette &) const;
    ///
    Palette operator * (const Palette &) const;
    ///
    Palette operator | (const RGBColor &) const;
    ///
    Palette operator + (const RGBColor &) const;
    ///
    Palette operator - (const RGBColor &) const;
    ///
    Palette operator ^ (const RGBColor &) const;
    ///
    Palette operator * (real) const;
    ///
    Palette & operator *= (real);
    ///
    Palette & operator *= (const RGBColor &);
    ///
    RGBColor & operator [] (int i) const;
    ///  Count : return the number of entries
    int Count() const;
    ///
    RGBColor & BestRGB(const RGBColor &) const;
    ///  Best : return the best RGBColor in the palette approximating the parameter. By default use Norm distance.
    int Best(const RGBColor &) const;
#ifdef DEBUGOUT
    ///
    friend ostream & operator << (ostream &, const Palette &);
    ///
    virtual ostream & DebugOut(ostream &) const;
#endif
    Palette & Shade16();
    ///  RedShade : predfine palette : 256 shades of red
    Palette & RedShade();
    ///  WhiteShade : predifine palette : 256 shade of white
    Palette & WhiteShade();
    ///  Voxel : predifine palette, special for voxel demo (cf voxel.cpp)
    Palette & Voxel();
    ///  Fire : predifine palette, special for fire demo
    Palette & Fire();
  };

  /**
  * COUNTPALETTE
  * This special palette add an statistic information to the normal palette.
  * For each palette entries, an other entry is added to count how many times
  * the color is used in the image. This class must be used exactly as a normal
  * palette (most of the times it's constructed by a FileImage). When everything
  * is OK just call AddImage(&Image) for each images that is associated to the
  * palette.
  */
  class CountPalette : public Palette
  {
    typedef Palette inherited;
    friend class PaletteBuilder;
  protected :
    int * fEntriesCount;
    void InitCounts();

  public :
    ///
    CountPalette(int Count=256);
    ///
    CountPalette(const CountPalette &);
    ///
    CountPalette(const Palette &);
    ///
    virtual ~CountPalette();
    ///
    CountPalette & operator = (const CountPalette &);
    ///
    CountPalette operator * (real ) const;
    ///
    CountPalette operator | (const CountPalette &) const;
    ///
    CountPalette operator + (const CountPalette &) const;
    ///
    CountPalette operator - (const CountPalette &) const;
    ///
    CountPalette operator * (const CountPalette &) const;
    ///
    CountPalette operator | (const RGBColor &) const;
    ///
    CountPalette operator + (const RGBColor &) const;
    ///
    CountPalette operator - (const RGBColor &) const;
    ///
    CountPalette operator ^ (const RGBColor &) const;
    ///
    CountPalette & operator *= (real);
    ///
    CountPalette & operator *= (const RGBColor &);
    ///  AddImage : for each pixel of the image increment it's associated count. The palette is considered as the image's one.
    CountPalette & AddImage(const Image &);
    ///
    CountPalette Transluent(const RGBColor &) const;
#ifdef DEBUGOUT
    ///
    friend ostream & operator << (ostream &, const CountPalette &);
    ///
    virtual ostream & DebugOut(ostream &) const;
#endif
  };

#define NOTPRESENT -1

  /**
  * PALETTEBUILDER :
  * the purpose of this class is to build a palette correspoding to diferrents
  * needs : display several images, and display several versions of the same
  * image (shaded images, and transpencies images). To use this class you must
  * add all the colors that you want to appear in the final palette and this
  * class will calculate the best palette that permit to view the image with
  * less deformations. In order that the algorithm work you must give all the
  * colors and how many there are used (how many times the color is used in the
  * image). That why you must use a CountPalette. This class is a CountPalette
  * and maintain it's own statistics on the use of the colors. The difference
  * between this class and CountPalette is that the size of the palette can
  * grow dinamically (you can add new entries after the creation of the instance.
  */
  class PaletteBuilder : public CountPalette
  {
    typedef CountPalette inherited;
  protected :
    int fThreshold;
    /* Present : determine if a color is already present in the palette. A color is
    * said present when the distance between an other color is less than a
    * threshold
    */
    int Present(const RGBColor & );
    // AddColor : when a color is not present in the current palette, add it.
    void AddColor(const RGBColor &, int EntryCount);

  public :
    ///
    PaletteBuilder(int Count, int Threshold=0);
    ///
    PaletteBuilder(const PaletteBuilder &, int Threshold=0);
    ///
    PaletteBuilder(const Palette &, int Threshold=0);
    ///  AddPalette : the palette must be taken in consideration for the calculation of the final palette.
    PaletteBuilder & AddPalette(const CountPalette & );
    /** Best : return the best NbPalEntries colors palette that can display all the
    * images. return always a NbPalEntries color palette, even if there is not
    * enough entries to fill it.
    */
    Palette Best(int BestCount=256) const;
#ifdef DEBUGOUT
    ///
    friend ostream & operator << (ostream &, const PaletteBuilder &);
    ///
    virtual ostream & DebugOut(ostream &) const;
#endif
  };

  /******************************************************************************\
  * Inline methods and properties
  \******************************************************************************/

  inline RGBColor & Palette::operator [] (int i) const
  {
    assert(i>=0);
    assert(i<fCount);
    return fEntries[i];
  }

  inline int Palette::Count() const
  {
    return fCount;
  }

}

#endif
