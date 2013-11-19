#ifndef _TEMPLATEFACEDRAWER_H_
#define _TEMPLATEFACEDRAWER_H_

/*
*AUTHOR : Babel yoann
*TITLE  : TempateFaceDrawer.h, 3D faces drawing
*CREATED: 19/05/99
*DATE   : 19/05/99
*OBJECT : This unit provides a generic algorithm to draw faces. It performs
*         clipping and polygon filling with easily customizable methods in
*         order to adapt the drawing to various problems. The drawer take
*         a Plotter, a Shader, and a Mapper in parameter to perform this task
*/

#include "FaceDrawer.h"

namespace Go4D
{

  /**
  *TFACEDRAWERDATA
  *  This class is used to store some information necessary to draw the face
  *  by the drawers. For example it can be the Image for a mapper, or
  *  a Clut (color lookup table) for a Shader...
  *  This is an abstract class that must be derived.
  *  Note : it would be better to make 3 classes, one for the Plotter, one
  *    for the Shader, and one for the Mapper, but for most cases, one is
  *    sufficient.
  */
  class FaceDrawerData
  {
  };

  /// contains an image (used for mapping)
  class DrawerImageData : public FaceDrawerData
  {
    typedef FaceDrawerData inherited;

  public :
    const Image * fMappedImage;
    DrawerImageData(const Image *);
  };

  /// contains an image an a multiclut
  class DrawerImageClutData : public DrawerImageData
  {
    typedef DrawerImageData inherited;

  public :
    const MultiClut * fMultiClut;
    DrawerImageClutData(const Image *, const MultiClut *);
  };

  /**
  *TFACEDRAWER
  *  This is the template face drawer super-class. It's an abstract class, that
  *  only have an Horiz method. Note that the SplitExtrem method is emptied by
  *  default.
  */
  class TFaceDrawer : public AFaceDrawer
  {
    friend class Filler;
  protected :
    typedef AFaceDrawer inherited;
    const FaceDrawerData * fData;

  public :
    TFaceDrawer(const Image &aImage, int PointsCount,
      const FaceDrawerData * data=NULL);
  };

  /**
  *FACEDRAWER
  *  This class is a template that get in parameter a class that inherit from
  *  TFiller class. It contains the basic polygon filling routine, with an
  *  horizontal drawing method that override the abstract method. The Clone
  *  method is also overrided. This class used the inline methods of the TFiller
  *  class to draw the polygon. By changing the TFiller class (wich is
  *  aggregated) you can define several drawing methods. The advantage is that
  *  the drawing algo is only written once. This can limit bugs. If you want to
  *  add a new drawing method, all you have to do is to write a class derivated
  *  from TFiller, that override (no virtual method) the old one.
  *  NOTE : the template could have been avoided, and replaced by some virtual
  *  methods (the ones in the TFiller class, see below). This would have been
  *  a best conception, but the drawbacks would have been a great speed loose.
  *<br><br>
  *Horiz : draw an horizontal line between two points. The line is clipped
  *  before being drawed. Note that there is many calls to the TFiller class
  *  that allow you to define your own drawing algo.
  *<br><br>
  *Draw : fill the polygon. The points order is changed, so becareful to make
  *  copy somewhere, if it's required. As the Horiz method, the filling algo
  *  makes many calls to the TFiller that allow a user defined algo. The
  *  advantage is that you don't have to write the drawing algo twice, you just
  *  have to write what is specific. Note that this algo only support convex
  *  polygons (with as many edges as you want).
  *<br><br>
  *Clone : used by cutting algo, return a class (not entirely initialized, look
  *  the method itself for more info) that had the same type of the class. This
  *  is a sort of virtual constructor (not allowed in C++).
  *<br><br>
  *fFiller : this fied is mutable, that means that you dont have to specify
  *  const methods in the TFiller class.
  *
  */
  template <class TPlotter, class TShader, class TMapper> class FaceDrawer
    : public TFaceDrawer
  {
  protected :
    typedef TFaceDrawer inherited;
    mutable TPlotter fPlotter;
    mutable TShader fShader;
    mutable TMapper fMapper;

  public :
    ///
    FaceDrawer<TPlotter, TShader, TMapper>(const Image & aImage, int aPointsCount,
      const FaceDrawerData * data=NULL)
      : inherited(aImage, aPointsCount, data)
    {
      fPlotter.Init(&aImage, data);
      fShader.Init(&aImage, data);
      fMapper.Init(&aImage, data);
    }
    ///
    virtual void SetData(const FaceDrawerData * data)
    {
      assert(data != NULL);
      fData = data;
      fPlotter.Init(fImage, data);
      fShader.Init(fImage, data);
      fMapper.Init(fImage, data);
    }
    ///
    virtual AFaceDrawer * Clone(int NewPointsCount) const
    {
      return new FaceDrawer<TPlotter, TShader, TMapper>(*fImage, NewPointsCount, fData);
    }
    ///
    virtual void SplitExtrem(AFaceDrawer * Parent, int FirstPositive,
      int FirstNegative, int PositiveCount, int FirstSign,
      real FirstCoord, real SecondCoord)
    {
      // nothing to be done with face drawer, it must only be done with MappedFaceDrawer
    }

    /** this method draw a horizontal line between 2 points. using the plotter/shader/mapper
    * it can draw line with various algorithms, like Flat (the same color is used all along)
    * hypperbolic mapping (an image is mapped with no deformation on the face)...
    */
    void Horiz(realtype xb, realtype xe,
      realtype zb, realtype ze, int y) const
    {
      // Asssertions (only for mapping)
      //-------------------------------
      fPlotter.HorizAssertions();
      fShader.HorizAssertions();
      fMapper.HorizAssertions();

      // ensure that rxb < rxe
      if (xb>xe)
      {
        SwapR(xb,xe);
        SwapR(zb, ze);
        fPlotter.HorizSwap();
        fShader.HorizSwap();
        fMapper.HorizSwap();
      }
      xe+=conv(1);

      // Clipping and ordering
      //----------------------

      int rxb = round(xb);
      int rxe = round(xe);

      // realy need to draw ?
      //if ( abs(xe-xb) < conv(1))
      //  return;

      // test if the entire line is out of the screen
      if ( (y<ymin)||(y>=ymax)
        || (rxb<xmin)&&(rxe<xmin)
        || (rxb>=xmax)&&(rxe>=xmax) )
        return;

      // coeficiants initilization
      fPlotter.HorizCoefInit(xb, xe, zb, ze);
      fShader.HorizCoefInit(xb, xe, zb, ze);
      fMapper.HorizCoefInit(xb, xe, zb, ze);

      // clipping is effectivelly done here
      int xd, xf; // roundf(xb), roundf(xe) after clipping
      xd=rxb, xf=rxe;
      if (rxb<xmin)
      {
        fPlotter.HorizClip(xmin, rxb);
        fShader.HorizClip(xmin, rxb);
        fMapper.HorizClip(xmin, rxb);
        xd = xmin;
      }
      if (rxe>xmax-1) // because horiz go to xf+1  (because xf=round(xe))
        xf = xmax-1;

      if (xd==xf)
        return;

      // Drawing of the line
      //--------------------

      fPlotter.HorizLocalsClipped(xb, xe, xd, xf, zb, ze, rxb, rxe);
      fShader.HorizLocalsClipped(xb, xe, xd, xf, zb, ze, rxb, rxe);
      fMapper.HorizLocalsClipped(xb, xe, xd, xf, zb, ze, rxb, rxe);

      // pointers declaration
      fPlotter.HorizInitPixel(Width()*y+xd);
      fShader.HorizInitPixel(Width()*y+xd);
      fMapper.HorizInitPixel(Width()*y+xd);

      // main loop
      //  xd+1 because round(xb)+1   x must be >= xb (k>=0)
      //---------------------------------------------------
      for (int x=xd+1; x<=xf; x++)
      {
        int offs = fMapper.HorizGetOffs();
        fPlotter.HorizPutPixel(fShader.HorizShadePixel(fMapper.HorizGetPixel(offs), offs));
        fPlotter.HorizNextPixel();
        fShader.HorizNextPixel();
        fMapper.HorizNextPixel();
      }
    }

    /**  description of the alogrithm FILL SHAPE WITH N POINTS
    *
    *           1  *-------------------* 2
    *             /                     \
    *            /          Z1           \
    *        3  *-------------------------\
    *           |                          \                      |
    *   |       |             Z2            \                     |  xe
    *xb |       |                            \ 4                  |
    *   |       |-----------------------------*-----------* 5     v
    *   v       |              Z3                        /
    *        6  *---------------------------------------/
    *            \                                     /
    *             \              Z5                   /
    *              \                                 /
    *           7   *-------------------------------/
    *                      --------      Z6        /
    *                              --------       /
    *                                      ------* 8
    *
    * -  The plots are sorted, and the shape is cutted in zones, each zone is
    *draw basically. the first point P1 is the point with the minimal y.
    *the last point P8 is the point with the maximal y.
    * -  There is 2 virtual points that are following the edges : one folow
    *the left edges (indiced bye 'l'), the other follow the right edges
    *(indiced by 'r') : (1,3,6,7) are the lefts points, whereas (2,4,5,8)
    *are the right points.
    *
    *[Q] How are the points determined to be left or right ?
    *[A] The plots are sorted in the 'points' list, at the begining the current
    *'left' point and the current 'right' point are equal to points[i=0], then
    *next left point will be points[i++], next right point will be points[i--]
    *(assuming i--=npoints if i==0)
    *
    *[Q] How do i draw a 'zone'?
    *[A] The limit of the zone is determine by the old left and right points
    *and the coordinates of the news left and right points. The you can calculate
    *the coef dir of the left and right edges. You have to draw an horizontal
    *line between xl and xr, and when its done calculate the new xl and xr for
    *the next line (simply increment xl and xr by ixl and ixr).
    *
    *                   *--------------------------*
    *            ixl   /                          /  ixr (coef of the line)
    *                 /                          /
    *         xl ---->           ry             <---  xr
    * xl+=ixl ->    /          ry=ry+1         /   <-xe+=ixr
    *              /            ...           /
    *             /                          /
    *            *--------------------------* ry==ryf
    *
    *
    * -  Then when you reach the end of the zone a new ixl (or ixr) must be
    *calculated (due to the changing left (or right) point)
    * -  You had to iterates these step until left==right
    *
    *[Q] How do i draw a gouraud shaded shape, or a mapped shape ?
    *[A] for gouraud shading and linear mapping its very simple : you have to
    *determine the coefs ic (or imx, imy) to increment the color of the current
    *point (or the coordinate of the new mapping point).
    *
    *note : to calculate the coef dir of the lines, the macro frapport is used
    *rather than div or rapport because if precision is two great it will return
    *zero (it's not well handled by gcc). this is a little bit slower (the
    *division is made in float, instead of being made in int) due to the
    *conversion
    *but it's more secure.
    *
    */
    virtual void Draw(int npoints, PointZ * points) const
    {
      fPlotter.FillInitDrawing(npoints, points);
      fShader.FillInitDrawing(npoints, points);
      fMapper.FillInitDrawing(npoints, points);

      // becarful the list can be modified (rotated)
      //don't verify that the shape is convex
      assert(npoints>2); //at least 3 points
      fPlotter.FillAssertions(npoints, points);
      fShader.FillAssertions(npoints, points);
      fMapper.FillAssertions(npoints, points);

      //search the point with the y max
      //-------------------------------

      int minp = 0;
      int miny = points[0].y;
      for (int ks=1; ks<npoints; ks++)
        if (points[ks].y < miny) { miny=points[ks].y; minp = ks; }

        // the first point of the list must be the one with the y min
        // so we make a rotation in the list
        //-----------------------------------------------------------

        for (int kr=0; kr<minp; kr++)
        {
          PointZ tmp = points[0];
          for (int i=0; i<npoints-1; i++)
            points[i] = points[i+1];
          points[npoints-1] = tmp;
          fPlotter.FillPointsRot(npoints);
          fShader.FillPointsRot(npoints);
          fMapper.FillPointsRot(npoints);
        }

        // coef's initialization
        //----------------------

        int right=0, left=0;
        int ry=points[0].y;
        int ryf;  //initialized later, <=> y when the next pallier is reached

        realtype xl=conv(points[0].x);
        realtype xr=conv(points[0].x);
        realtype ixl=0, ixr=0;

        realtype zl=fconv(points[0].z);
        realtype zr=fconv(points[0].z);
        realtype izl=0, izr=0;

        fPlotter.FillInitLocals(zl, zr, ry);
        fShader.FillInitLocals(zl, zr, ry);
        fMapper.FillInitLocals(zl, zr, ry);

        // draw the differents zones
        //--------------------------

        do
        {

          // ryf = the limit of the zone
          //---------------------------

          if (points[left].y < points[right].y)
            ryf  = points[left].y;
          else
            ryf  = points[right].y;

          // draw the zone (depending on zbuffer)
          //-------------------------------------

          if (ryf != ry)
          {
            while (ry<ryf)
            {
              fPlotter.FillInitHoriz(xl, xr, zl, zr, ry);
              fShader.FillInitHoriz(xl, xr, zl, zr, ry);
              fMapper.FillInitHoriz(xl, xr, zl, zr, ry);
              Horiz(xl, xr, zl, zr, ry);
              ry++;
              xl+=ixl; xr+=ixr;
              zl+=izl; zr+=izr;
              fPlotter.FillNextLine(ry);
              fShader.FillNextLine(ry);
              fMapper.FillNextLine(ry);
            }
          }

          // calculate the new coef for the next zone
          //-----------------------------------------

          if (points[left].y < points[right].y)
          {
            fPlotter.FillLeftChanging(left);
            fShader.FillLeftChanging(left);
            fMapper.FillLeftChanging(left);
            left++;
            fPlotter.FillLeftChanged(points, left);
            fShader.FillLeftChanged(points, left);
            fMapper.FillLeftChanged(points, left);

            if (points[left].y == ry)
            {
              xl = conv(points[left].x);
              zl = fconv(points[left].z);
              fPlotter.FillLeftSameCoef(left);
              fShader.FillLeftSameCoef(left);
              fMapper.FillLeftSameCoef(left);
            }
            else
            {
              realtype deltax = conv(points[left].x) - xl;
              realtype deltay = conv(points[left].y - ry);
              realtype deltaz = fconv(points[left].z) - zl;
              fPlotter.FillLeftNotSameInitLocals(left);
              fShader.FillLeftNotSameInitLocals(left);
              fMapper.FillLeftNotSameInitLocals(left);
              ixl = frapport(deltax, deltay);
              izl = frapport(deltaz, deltay);
              fPlotter.FillLeftNotSameCoef(deltay);
              fShader.FillLeftNotSameCoef(deltay);
              fMapper.FillLeftNotSameCoef(deltay);
            }
          }
          else
          {
            fPlotter.FillRightChanging(right);
            fShader.FillRightChanging(right);
            fMapper.FillRightChanging(right);
            if (right==0) right = npoints-1; else right--;
            fPlotter.FillRightChanged(points, right);
            fShader.FillRightChanged(points, right);
            fMapper.FillRightChanged(points, right);

            if (points[right].y == ry)
            {
              xr = conv(points[right].x);
              zr = fconv(points[right].z);
              fPlotter.FillRightSameCoef(right);
              fShader.FillRightSameCoef(right);
              fMapper.FillRightSameCoef(right);
            }
            else
            {
              realtype deltax = conv(points[right].x) - xr;
              realtype deltay = conv(points[right].y - ry);
              realtype deltaz = fconv(points[right].z) -zr;
              fPlotter.FillRightNotSameInitLocals(right);
              fShader.FillRightNotSameInitLocals(right);
              fMapper.FillRightNotSameInitLocals(right);
              ixr = frapport(deltax, deltay);
              izr = frapport(deltaz, deltay);
              fPlotter.FillRightNotSameCoef(deltay);
              fShader.FillRightNotSameCoef(deltay);
              fMapper.FillRightNotSameCoef(deltay);
            }
          }
        }
        while (right >= left
          && !(right==0 && left == npoints-1)
          && !(right==0 && left==0) );
    }
  };

  /// used to specify the points in the source image for the mapping
  struct MapPoint
  {
    int x;
    int y;
  };
  // margin define a margin in the mapped image
#define MARGIN 1
#define NEXT(i) ((i)==aParent->Count()-1?0:(i)+1)
#define PREV(i) ((i)==0?aParent->Count()-1:(i)-1)
#define PARENTMPOINT ((MappedFaceDrawer *)aParent)->mpoints

  /**
  *MAPPEDFACEDRAWER
  *  This class is specialy designed to draw mapped faces. It contains a pointer
  *  toward the mapped image, and the coordinates of all the MapPoints. The
  *  MapPoint list define a polygon, within the image, that will be mapped onto
  *  an other polygon that have the same number of edges. This class contains
  *  also a specific cutting algo that create the new MapPoints when the face is
  *  cutted. The Clone method is also override.
  *
  *SplitExtrem : Special cutting algo that calculates the new polygon that will
  *  be mapped.
  *
  */
  template <class TPlotter, class TShader, class TMapper> class MappedFaceDrawer
    : public FaceDrawer<TPlotter, TShader, TMapper>
  {
    // YOB GCC 4.1
 //   #define fCount inherited::fCount
    #define fMapper inherited::fMapper

    friend class Mapper;
  protected :
    typedef FaceDrawer<TPlotter, TShader, TMapper> inherited;
    MapPoint * mpoints, * dupmpoints;
    int fMapWidth;
    int fMapHeight;

  public :
    ///
    MappedFaceDrawer<TPlotter, TShader, TMapper>(const Image & aImage, int aPointsCount,
      const FaceDrawerData * drawerData=NULL)
      : inherited(aImage, aPointsCount, drawerData)
    {
//      alloc(mpoints = new MapPoint[fCount]); // YBA2013
//      alloc(dupmpoints = new MapPoint[fCount]); // YBA2013
      alloc(mpoints = new MapPoint[aPointsCount]);
      alloc(dupmpoints = new MapPoint[aPointsCount]);
      fMapper.InitMapPoints(dupmpoints);

      if (drawerData==NULL) return;
      const Image * aMappedImage = static_cast<const DrawerImageData *>(drawerData)->fMappedImage;
      fMapWidth = aMappedImage->Width()-MARGIN;
      fMapHeight = aMappedImage->Height()-MARGIN;
    }
    ///
    MappedFaceDrawer<TPlotter, TShader, TMapper>(const Image & aImage,
      const FaceDrawerData * drawerData=NULL)
      : inherited(aImage, 4, drawerData)
    {
//      alloc(mpoints = new MapPoint[fCount]);
//      alloc(dupmpoints = new MapPoint[fCount]);
      alloc(mpoints = new MapPoint[4]); //YBA2013
      alloc(dupmpoints = new MapPoint[4]); //YBA2013

      if (drawerData==NULL) return;
      const Image * aMappedImage = static_cast<const DrawerImageData *>(drawerData)->fMappedImage;
      SetMapPoint(0, MARGIN, MARGIN);
      SetMapPoint(1, aMappedImage->Width()-MARGIN-1, MARGIN);
      SetMapPoint(2, aMappedImage->Width()-MARGIN-1, aMappedImage->Height()-MARGIN-1);
      SetMapPoint(3, 1, aMappedImage->Height()-MARGIN-1);
      fMapWidth = aMappedImage->Width()-MARGIN;
      fMapHeight = aMappedImage->Height()-MARGIN;
      fMapper.InitMapPoints(dupmpoints);
    }
    ///
    virtual void SetData(const FaceDrawerData * data)
    {
      assert(data != NULL);
      inherited::SetData(data);
      const Image * aMappedImage = static_cast<const DrawerImageData *>(data)->fMappedImage;
      fMapWidth = aMappedImage->Width()-MARGIN;
      fMapHeight = aMappedImage->Height()-MARGIN;
      fMapper.InitMapPoints(dupmpoints);
    }
    ///
    ~MappedFaceDrawer()
    {
      delete [] mpoints;
      delete [] dupmpoints;
    }
    ///
    virtual void Draw(int npoints, PointZ * points) const
    {
      for (int i=0; i<npoints; i++)
        dupmpoints[i] = mpoints[i];
      inherited::Draw(npoints, points);
    }
    ///
    virtual void SplitExtrem(AFaceDrawer * aParent, int FirstPositive,
      int FirstNegative,  int PositiveCount, int,
      real FirstCoord, real SecondCoord)
    {
      int ir=FirstPositive;
      for (int q=0; q<PositiveCount; q++, ir=NEXT(ir))
        mpoints[q] = PARENTMPOINT[ir];

      mpoints[PositiveCount].x = (int)(
        (PARENTMPOINT[PREV(FirstNegative)].x-PARENTMPOINT[FirstNegative].x)*
        SecondCoord+PARENTMPOINT[FirstNegative].x);
      mpoints[PositiveCount].y = (int)(
        (PARENTMPOINT[PREV(FirstNegative)].y-PARENTMPOINT[FirstNegative].y)*
        SecondCoord+PARENTMPOINT[FirstNegative].y);

      mpoints[PositiveCount+1].x = (int)(
        (PARENTMPOINT[PREV(FirstPositive)].x-PARENTMPOINT[FirstPositive].x)*
        FirstCoord+PARENTMPOINT[FirstPositive].x);
      mpoints[PositiveCount+1].y = (int)(
        (PARENTMPOINT[PREV(FirstPositive)].y-PARENTMPOINT[FirstPositive].y)*
        FirstCoord+PARENTMPOINT[FirstPositive].y);
    }
    ///
    virtual void SetMapPoint(int aPointIndice, int aMapX, int aMapY)
    {
      mpoints[aPointIndice].x = aMapX;
      mpoints[aPointIndice].y = aMapY;
    }
    ///
    virtual int GetMapWidth()
    {
      return fMapWidth;
    }
    ///
    virtual int GetMapHeight()
    {
      return fMapHeight;
    }
    ///
    AFaceDrawer * Clone(int NewPointsCount) const
    {
      return new  MappedFaceDrawer(*inherited::fImage, NewPointsCount, inherited::fData);
    }
  };

  /**
  *FILLER
  *  This class implement a null drawing algo (the polygon is fiiled with 0).
  *  Most of the inline methods are empty. This class also show you all the
  *  methods that you must overide to implement a special drawing algo (like
  *  mapping, see below). This is not a useful class, just a supper-class for
  *  more specific drawing classes. Note that all the methods are const (wich is
  *  not obliged).
  *
  *PreInit : This method MUST be called after the constructor, it initialize a
  *  reference to a TFaceDrawer. The class wont wirk without that. Because it
  *  calls back the Horiz method of the TFaceDrawer. Maybe this can be
  *  ameliorated.
  *  fill with 0, non used
  *
  */
  class Filler
  {
    //friend Filler;
  public :
    ///
    Filler() {}
    ///
    void Init(const Image *, const FaceDrawerData *) { }
    // fill methods
    ///
    inline void FillInitDrawing(int , PointZ * ) const { }
    ///
    inline void FillAssertions(int , PointZ * ) const { }
    ///
    inline void FillPointsRot(int) const { }
    ///
    inline void FillInitLocals(realtype, realtype, int) const { }
    inline void FillInitHoriz(realtype, realtype, realtype, realtype, int) const { }
    inline void FillNextLine(int) const { }
    ///
    inline void FillLeftChanging(int) const { }
    ///
    inline void FillLeftChanged(PointZ *, int) const { }
    ///
    inline void FillLeftSameCoef(int) const { }
    ///
    inline void FillLeftNotSameInitLocals(int) const { }
    ///
    inline void FillLeftNotSameCoef(realtype) const { }
    ///
    inline void FillRightChanging(int) const { }
    ///
    inline void FillRightChanged(PointZ *, int) const { }
    ///
    inline void FillRightSameCoef(int) const { }
    ///
    inline void FillRightNotSameInitLocals(int) const { }
    ///
    inline void FillRightNotSameCoef(realtype) const { }
    // horiz methods
    ///
    inline void HorizAssertions() const { }
    ///
    inline void HorizSwap() const { }
    ///
    inline void HorizCoefInit(realtype, realtype, realtype, realtype) const {}
    inline void HorizLocalsClipped(realtype, realtype, int, int, realtype, realtype, int, int) const { }
    inline void HorizClip(int , int) const { }
    ///
    inline void HorizInitPixel(int ) const { }
    ///
    inline int HorizGetOffs() const { return 0; }
    inline Color HorizGetPixel(int offs) const { return 0; }
    inline Color HorizShadePixel(Color color, int offs) const { return color; }
    inline void HorizPutPixel(Color color) const { }
    inline void HorizNextPixel() const { }
  };

}

#endif
