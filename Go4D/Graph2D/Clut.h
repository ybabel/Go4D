#ifndef _CLUT_H_
#define _CLUT_H_

/*
*AUTHOR : Babel yoann
*TITLE  : Clut.h
*CREATED: 07/07/96
*DATE   : 07/07/96
*OBJECT : ColoLookUpTable
*/

#include "Palette.h"
#include "Image.h"

namespace Go4D
{

  /**
  * COLORLOOKUPTABLE :
  *   This class allow to display the same image in diferrent maner. Generaly
  *   speaking, instead of putting a pixel directly in an image you can get the
  *   value of the pixel in a table. For examplz Pixel(i,j,color) put the
  *   color in the pixel at (i,j). If you use a Clut then the same instruction
  *   put the value of table[color] instead. The interest of such a class is that
  *   you can build Clut to display with the same routine an image differently
  *   depending on the clut you use. You can for instance display an image or the
  *   same image shaded (if the clut is adapted). The Clut is useful for a 8 bit
  *   display, that cannot support much colors. An other, and very important
  *   usage is the gouraud-mapping, and translucid-mapping.
  *
  * Note : ClutDrawer is friend in order to optimze algo
  */
  class ColorLookUpTable
  {
    friend class ClutDrawer;
  protected :
    Color * fValues;
    int fCount;
#ifdef DEBUGOUT
    virtual ostream & DebugOut(ostream &) const;
#endif

  public :
    /// by default a 256 clut is created
    ColorLookUpTable(int Count=256);
    /// copy another clut
    ColorLookUpTable(const ColorLookUpTable &);
    /// destructor
    virtual ~ColorLookUpTable();
    /// return a color in the table
    Color & operator [] (int) const;
    /// return the number of elements of the table
    int Count() const { return fCount; }
#ifdef DEBUGOUT
    /// write the clut in the stream
    friend ostream & operator << (ostream &, const ColorLookUpTable &);
#endif
  };


  /**
  * ShadeClut :
  *   For a given Palette perform the clut that will permit to display the same
  *   image shaded by a given percentage.
  */
  class ShadeClut : public ColorLookUpTable
  {
    typedef ColorLookUpTable inherited;
  public :
    /// Build a table where each color is the shaded color of the color in the palette
    ShadeClut(const Palette &, real Percent);
  };

  /**
  * MultiClut :
  *   regroup many cluts
  */
  class MultiClut : public ColorLookUpTable
  {
    typedef ColorLookUpTable inherited;
  protected :
    const Palette * pPalette; // not owner, relation
    int fClutsCount;
    int fClutValuesCount;

  public :
    /// define the nimber of cluts in the table
    MultiClut(const Palette &, int ClutsCount);
    /// SetClut : abstract method that initialize a clut (only one in the list). you must use the method after construction for each clut of the list.
    virtual void SetClut(int ClutNum, real Percent)=0;
    /// return a color in the tables
    Color & operator () (int ClutNum, int ClutValue) const;
    /// return the number of cluts in the table
    int ClutsCount() const { return fClutsCount; }
  };

  /**
  * MultiShadeClut :
  *   regroup many shade cluts
  */
  class MultiShadeClut : public MultiClut
  {
    typedef MultiClut inherited;
  public :
    /// 
    MultiShadeClut(const Palette &, int NbCluts);
    /// 
    virtual void SetClut(int ClutNum, real percent);
  };

  /**
  * TransluentClut :
  *  just add some methods to initializee the clut
  */
  class TransluentClut : public ColorLookUpTable
  {
    typedef ColorLookUpTable inherited;
  protected :
    const Palette * pPalette;

  public :
    /// 
    TransluentClut(const Palette &);
    /// 
    TransluentClut & Init(const RGBColor &);
    /// 
    TransluentClut & Init(const Palette &);
  };

  /**
  * ClutDrawer :
  * Draw a pixel by looking it's value in a clut or a multiclut.
  */
  class ClutDrawer : public Image
  {
    typedef Image inherited;
  public :
    /// 
    ClutDrawer(int Width, int Height, bool autoalloc=true);
    ///  draw a pixel by looking it's value in a simple Clut
    void ClutPixel(int x, int y, Color color, const ColorLookUpTable &);
    ///  draw a pixel by looking it's value in a simple Clut, more over insure that the pixel is in the image
    void ClutClipPixel(int x, int y, Color color, const ColorLookUpTable &);
    ///  draw a pixel by looking it's value in a multiple Clut
    void MultiClutPixel(int x, int y, Color color, const MultiClut &, int ClutNum);
    ///  draw a pixel by looking it's value in a multiple Clut, more over insure that the pixel is in the image
    void MultiClutClipPixel(int x, int y, Color color, const MultiClut &, int ClutNum);
  };

  /******************************************************************************\
  * Inline method and properties
  \******************************************************************************/

  inline void ClutDrawer::ClutPixel(int x, int y, Color color,
    const ColorLookUpTable & aClut)
  {
    assert(color < aClut.Count());
    Pixel(x, y, aClut[color]);
  }

  inline void ClutDrawer::ClutClipPixel(int x, int y, Color color,
    const ColorLookUpTable & aClut)
  {
    assert(color < aClut.Count());
    ClipPixel(x, y, aClut[color]);
  }

  inline void ClutDrawer::MultiClutPixel(int x, int y, Color color,
    const MultiClut & aClut, int aClutNum)
  {
    Pixel(x, y, aClut(aClutNum,color));
  }

  inline void ClutDrawer::MultiClutClipPixel(int x, int y, Color color,
    const MultiClut & aClut, int aClutNum)
  {
    ClipPixel(x, y, aClut(aClutNum,color));
  }

}
#endif

