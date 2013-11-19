#ifndef _FILLER_H_
#define _FILLER_H_

/**
*AUTHOR     : Babel Yoann
*CREATION   : 04/08/96
*DATE       : 04/08/96
*TITLE      : Filler.h, face filling template
*OBJECT     : all the method to draw a face
*             NOTE : this unit is OBSOLETE, use TemplateFaceDrawer instead
*/

#include "FaceDrawer.h"

namespace Go4D
{

  namespace Obsolete
  {


    /**
    *TFACEDRAWER
    *  This is the template face drawer super-class. It's an abstract class, that
    *  only have an Horiz method. Note that the SplitExtrem method is emptied by
    *  default.
    *  OBSOLETE
    */
    class TFaceDrawer : public AFaceDrawer
    {
      typedef AFaceDrawer inherited;
      friend class Filler;
    protected :
      virtual void Horiz(realtype xb, realtype xe,
        realtype zb, realtype ze, int y) const=0;

    public :
      ///
      TFaceDrawer(const Image &aImage, int PointsCount) : inherited(aImage, PointsCount) {}
      ///
      void SplitExtrem(AFaceDrawer *, int, int, int, int, real, real) { }
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
    *
    *Horiz : draw an horizontal line between two points. The line is clipped
    *  before being drawed. Note that there is many calls to the TFiller class
    *  that allow you to define your own drawing algo.
    *
    *Draw : fill the polygon. The points order is changed, so becareful to make
    *  copy somewhere, if it's required. As the Horiz method, the filling algo
    *  makes many calls to the TFiller that allow a user defined algo. The
    *  advantage is that you don't have to write the drawing algo twice, you just
    *  have to write what is specific. Note that this algo only support convex
    *  polygons (with as many edges as you want).
    *
    *Clone : used by cutting algo, return a class (not entirely initialized, look
    *  the method itself for more info) that had the same type of the class. This
    *  is a sort of virtual constructor (not allowed in C++).
    *
    *fFiller : this fied is mutable, that means that you dont have to specify
    *  const methods in the TFiller class.
    *
    * OBSOLETE
    */
    template <class TFiller> class FaceDrawer : public TFaceDrawer
    {
      typedef TFaceDrawer inherited;
    protected :
      mutable TFiller fFiller;

    public :
      ///
      FaceDrawer<TFiller>(const Image & aImage, int aPointsCount)
        : inherited(aImage, aPointsCount)
      {
        fFiller.PreInit(this);
      }
      ///
      virtual void Horiz(realtype xb, realtype xe,
        realtype zb, realtype ze, int y) const
      {
        // Asssertions (only for mapping)
        //-------------------------------
        fFiller.HorizAssertions();

        // Variables declaration
        //----------------------

        int xd, xf; // roundf(xb), roundf(xe) after clipping

#ifdef zbuffer
        realtype z, iz;
#endif

        // Clipping and ordering
        //----------------------

        int rxb = round(xb);
        int rxe = round(xe);

        // realy need to draw ?
        if ( abs(xe-xb) < conv(1))
          return;
        // test if the entire line is out of the screen
        if ( (y<ymin)||(y>=ymax)
          || (rxb<xmin)&&(rxe<xmin)
          || (rxb>=xmax)&&(rxe>=xmax) )
          return;

        // ensure that rxb < rxe
        if (xb>xe)
        {
          Swap(rxb,rxe);
          SwapR(xb,xe);
          SwapR(zb, ze);
          fFiller.HorizSwap();
        }

        // coeficiants initilization
        fFiller.HorizCoefInit(xb, xe, zb, ze);
#ifdef zbuffer
        iz = frapport( (ze-zb), (xe-xb) );
        z = zb;
#endif

        // clipping is effectivelly done here
        xd=rxb, xf=rxe;
        if (rxb<xmin)
        {
          fFiller.HorizClip(xmin, rxb);
#ifdef zbuffer
          z += iz*(xmin-rxb);
#endif
          xd = xmin;
        }
        if (rxe>xmax-1) // because horiz go to xf+1  (because xf=round(xe))
          xf = xmax-1;

        if (xd==xf)
          return;

        // Drawing of the line
        //--------------------

        fFiller.HorizLocalsClipped(xb, xe, xd, xf, zb, ze, rxb, rxe);

        // pointers declaration
        Color * Cur = fBuffer+Width()*y+xd;
#ifdef zbuffer
        realtype *CurZ = fZBuffer+Width()*y+xd;
#endif

        // main loop
        //  xd+1 because round(xb)+1   x must be >= xb (k>=0)
        //---------------------------------------------------
        for (int x=xd+1; x<=xf; x++, Cur++)
          //for (int x=xd+1; x<=xf+1; x++, Cur++) fill the lacking space but bug some time
        {
#ifdef zbuffer
          if (z< *CurZ)
          {
#endif
            assert(int(Cur-GetBuffer())>=0);
            assert(int(Cur-GetBuffer())<Len());
            *Cur = fFiller.HorizPutPixel();
#ifdef zbuffer
            *CurZ = z;
          }
          z += iz;
          CurZ++;
#endif
          fFiller.HorizNextPixel();
        }
      }
      ///
      virtual void Draw(int npoints, PointZ * points) const
      {
        // becarful the list can be modified (rotated)
        //don't verify that the shape is convex
        assert(npoints>2); //at least 3 points
        fFiller.FillAssertions(points);

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
            fFiller.FillPointsRot();
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

          fFiller.FillInitLocals(zl, zr, ry);

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
                fFiller.FillCallHoriz(xl, xr, zl, zr, ry);
                ry++;
                xl+=ixl; xr+=ixr;
                zl+=izl; zr+=izr;
                fFiller.FillNextLine(ry);
              }
            }

            // calculate the new coef for the next zone
            //-----------------------------------------

            if (points[left].y < points[right].y)
            {
              fFiller.FillLeftChanging(left);
              left++;
              fFiller.FillLeftChanged(points, left);

              if (points[left].y == ry)
              {
                xl = conv(points[left].x);
                zl = fconv(points[left].z);
                fFiller.FillLeftSameCoef(left);
              }
              else
              {
                realtype deltax = conv(points[left].x) - xl;
                realtype deltay = conv(points[left].y - ry);
                realtype deltaz = fconv(points[left].z) - zl;
                fFiller.FillLeftNotSameInitLocals(left);
                ixl = frapport(deltax, deltay);
                izl = frapport(deltaz, deltay);
                fFiller.FillLeftNotSameCoef(deltay);
              }
            }
            else
            {
              fFiller.FillRightChanging(right);
              if (right==0) right = npoints-1; else right--;
              fFiller.FillRightChanged(points, right);

              if (points[right].y == ry)
              {
                xr = conv(points[right].x);
                zr = fconv(points[right].z);
                fFiller.FillRightSameCoef(right);
              }
              else
              {
                realtype deltax = conv(points[right].x) - xr;
                realtype deltay = conv(points[right].y - ry);
                realtype deltaz = fconv(points[right].z) -zr;
                fFiller.FillRightNotSameInitLocals(right);
                ixr = frapport(deltax, deltay);
                izr = frapport(deltaz, deltay);
                fFiller.FillRightNotSameCoef(deltay);
              }
            }
          }
          while (right >= left
            && !(right==0 && left == npoints-1)
            && !(right==0 && left==0) );
      }
      AFaceDrawer * Clone(int NewPointsCount) const
      {
        return new  FaceDrawer(*fImage, NewPointsCount);
      }
    };

    struct MapPoint
    {
      int x;
      int y;
    };
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
    * OBSOLETE
    */
    template <class TMapper> class MappedFaceDrawer
      : public FaceDrawer<TMapper>
    {
      typedef FaceDrawer<TMapper> inherited;
      friend class Mapper;

      // YOB GCC4.1
      #define fCount inherited::fCount
      #define fMapper inherited::fMapper
      #define fFiller inherited::fFiller

    protected :
      const Image * fMappedImage;
      MapPoint * mpoints, * dupmpoints;

    public :
      ///
      MappedFaceDrawer<TMapper>(const Image & aImage, int aPointsCount,
        const Image & aMappedImage)
        : inherited(aImage, aPointsCount)
      {
        fMappedImage = &aMappedImage;
        alloc(mpoints = new MapPoint[fCount]);
        alloc(dupmpoints = new MapPoint[fCount]);
        fFiller.Init((MappedFaceDrawer<TMapper> *)this); // YOB GCC4.1
      }
      ///
      MappedFaceDrawer<TMapper>(const Image & aImage,
        const Image & aMappedImage)
        : inherited(aImage, 4)
      {
        fMappedImage = &aMappedImage;
        alloc(mpoints = new MapPoint[fCount]);
        alloc(dupmpoints = new MapPoint[fCount]);
        SetMapPoint(0, MARGIN, MARGIN);
        SetMapPoint(1, fMappedImage->Width()-MARGIN-1, MARGIN);
        SetMapPoint(2, fMappedImage->Width()-MARGIN-1, fMappedImage->Height()-MARGIN-1);
        SetMapPoint(3, 1, fMappedImage->Height()-MARGIN-1);
        fFiller.Init((MappedFaceDrawer<TMapper> *)this); // YOB GCC4.1
      }
      ///
      ~MappedFaceDrawer<TMapper>()
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
      void SplitExtrem(AFaceDrawer * aParent, int FirstPositive,
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
      void SetMapPoint(int aPointIndice, int aMapX, int aMapY)
      {
        mpoints[aPointIndice].x = aMapX;
        mpoints[aPointIndice].y = aMapY;
      }
      ///
      AFaceDrawer * Clone(int NewPointsCount) const
      {
        return new  MappedFaceDrawer(*inherited::fImage, NewPointsCount, *fMappedImage);
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
    *  This class fill with 0, non used
    *
    * OBSOLETE
    */
    class Filler
    {
    protected :
      mutable TFaceDrawer * fFaceDrawer;
      mutable int npoints;

    public :
      ///
      Filler() {}
      ///
      void PreInit(TFaceDrawer * aFaceDrawer)
      {
        fFaceDrawer = aFaceDrawer;
        npoints = fFaceDrawer->fCount;
      }
      // fill methods
      ///
      inline void FillAssertions(PointZ *) const { }
      ///
      inline void FillPointsRot() const { }
      ///
      inline void FillInitLocals(realtype, realtype, int) const { }
      ///
      inline void FillCallHoriz(realtype xl, realtype xr, realtype zl, realtype zr, int ry) const
      {
        fFaceDrawer->Horiz(xl, xr, zl, zr, ry);
      }
      ///
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
      ///
      inline void HorizLocalsClipped(realtype, realtype, int, int, realtype, realtype, int, int) const { }
      ///
      inline void HorizClip(int , int) const { }
      ///
      inline Color HorizPutPixel() const { return 0; }
      ///
      inline void HorizNextPixel() const { }
    };

    /**
    *MAPPER
    *  This class is the super-class for all classes that want to implement a
    *  mapping algo (like linear mapping, quadratic, and hypperbolic). Some methods
    *  that are common to all the mapping classes are predefined.
    *  just a base class for all mappers
    * OBSOLETE
    */
    class Mapper : public Filler
    {
      typedef Filler inherited;
    protected :
      mutable MapPoint * mappoints;
      mutable const Image * ima;
      static realtype mxb, myb, mxe, mye;
      static int bl;
      static Color * Ima;

    public :
      ///
      Mapper() {}
      ///
      void Init(MappedFaceDrawer<Mapper> * aMappedFaceDrawer)
      {
        mappoints = aMappedFaceDrawer->dupmpoints;
        ima = aMappedFaceDrawer->fMappedImage;
      }
      ///
      inline void FillPointsRot() const
      {
        inherited::FillPointsRot();
        MapPoint mtmp = mappoints[0];
        for (int mi=0; mi<npoints-1; mi++)
          mappoints[mi] = mappoints[mi+1];
        mappoints[npoints-1] = mtmp;
      }
      ///
      inline void HorizAssertions() const
      {
        inherited::HorizAssertions();
        assert(ima != NULL);
        assert(round(mxb)>=-1 && round(mxb)<ima->Width());
        assert(round(mxe)>=-1 && round(mxe)<ima->Width());
        assert(round(myb)>=-1 && round(myb)<ima->Height());
        assert(round(mye)>=-1 && round(mye)<ima->Height());
        assert(round(mxb)+ima->Width()*round(myb)<ima->Len());
        assert(round(mxe)+ima->Width()*round(mye)<ima->Len());
        bl=ima->Width();
      }
      ///
      inline void HorizSwap() const
      {
        inherited::HorizSwap();
        SwapR(mxb, mxe); SwapR(myb, mye);
      }
      ///
      inline void HorizLocalsClipped(realtype xb, realtype xe, int xd, int xf, realtype zb, realtype ze, int rxb, int rxe) const
      {
        inherited::HorizLocalsClipped(xb, xe, xd, xf, zb, ze, rxb, rxe);
        Ima = ima->GetBuffer();
      }
    };

    /**
    *LINEARMAPPER
    *  Linear mapping algo. This algo is the faster one, but it don't gives the
    *  best result. You can use it when MaxZ/MinZ is near 1 ( 0.7<MaxZ/MinZ<1.5 )
    *  This values must be tested. MaxZ is the Z max of all the Z of the polygon's
    *  3D points. When the face is far from the observer used this algo (the
    *  deformation is not visible any more).
    *  The class's fields are static, wich is not anoying, and much faster.
    *  NOTE :
    *  static is faster than mutable (static = global->easier access)
    * OBSOLETE
    */
    class LinearMapper : public Mapper
    {
      typedef Mapper inherited;
    private :
      // fill locals
      static realtype mxl;
      static realtype mxr;
      static realtype myl;
      static realtype myr;
      static realtype imxl, imxr, imyl, imyr;
      static realtype deltamx;
      static realtype deltamy;
      // horiz locals
      static realtype mx, imx, my, imy;

    public :
      ///
      LinearMapper() {}
      ///
      inline void FillInitLocals(realtype zl, realtype zr, int ry) const
      {
        inherited::FillInitLocals(zl, zr, ry);
        mxl=conv(mappoints[0].x);
        mxr=conv(mappoints[0].x);
        myl=conv(mappoints[0].y);
        myr=conv(mappoints[0].y);
        imxl=0; imxr=0; imyl=0; imyr=0;
      }
      ///
      inline void FillCallHoriz(realtype xl, realtype xr, realtype zl, realtype zr, int ry) const
      {
        mxb = mxl; myb = myl; mxe = mxr; mye = myr;
        inherited::FillCallHoriz(xl, xr, zl, zr, ry);
      }
      ///
      inline void FillNextLine(int ry) const
      {
        inherited::FillNextLine(ry);
        mxl+=imxl; myl+=imyl;
        mxr+=imxr; myr+=imyr;
      }
      ///
      inline void FillLeftSameCoef(int left) const
      {
        inherited::FillLeftSameCoef(left);
        mxl = conv(mappoints[left].x);
        myl = conv(mappoints[left].y);
      }
      ///
      inline void FillLeftNotSameInitLocals(int left) const
      {
        inherited::FillLeftNotSameInitLocals(left);
        deltamx = conv(mappoints[left].x) - mxl;
        deltamy = conv(mappoints[left].y) - myl;
      }
      ///
      inline void FillLeftNotSameCoef(realtype deltay) const
      {
        inherited::FillLeftNotSameCoef(deltay);
        imxl = frapport(deltamx, deltay);
        imyl = frapport(deltamy, deltay);
      }
      ///
      inline void FillRightSameCoef(int right) const
      {
        inherited::FillRightSameCoef(right);
        mxr = conv(mappoints[right].x);
        myr = conv(mappoints[right].y);
      }
      ///
      inline void FillRightNotSameInitLocals(int right) const
      {
        inherited::FillRightNotSameInitLocals(right);
        deltamx = conv(mappoints[right].x) - mxr;
        deltamy = conv(mappoints[right].y) - myr;
      }
      ///
      inline void FillRightNotSameCoef(realtype deltay) const
      {
        inherited::FillRightNotSameCoef(deltay);
        imxr = frapport(deltamx, deltay);
        imyr = frapport(deltamy, deltay);
      }
      ///
      inline void HorizCoefInit(realtype xb, realtype xe, realtype zb, realtype ze) const
      {
        inherited::HorizCoefInit(xb, xe, zb, ze);
        imx = frapport( (mxe-mxb), (xe-xb+1) );
        mx = mxb;
        imy = frapport( (mye-myb), (xe-xb+1) );
        my = myb;
      }
      ///
      inline void HorizClip(int xmin, int rxb) const
      {
        inherited::HorizClip(xmin, rxb);
        mx += imx*(xmin-rxb);
        my += imy*(xmin-rxb);
      }
      ///
      inline Color HorizPutPixel() const
      {
        assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())>=0);
        assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())<ima->Len());
        return *(Ima+ round(mx)+ bl*round(my));
      }
      ///
      inline void HorizNextPixel() const
      {
        inherited::HorizNextPixel();
        mx += imx;
        my += imy;
      }
    };

    void QuadMapCoefs(realtype xb,realtype xe,realtype l,
      real & fa, real & fb, real & fc);
    void QuadMapInit(real fa, real fb, real fc, realtype xd,
      realtype mxb, realtype mxe,
      realtype myb, realtype mye,
      realtype &mx, realtype &my,
      realtype &tx, realtype &ty,
      realtype &ax, realtype &ay);
    void QuadMapCoefsInit(realtype xb, realtype xe, realtype l,
      realtype xd,
      realtype mxb, realtype mxe,
      realtype myb, realtype mye,
      realtype &mx, realtype &my,
      realtype &tx, realtype &ty,
      realtype &ax, realtype &ay);

    /**
    *QUADRATICMAPPER
    *  This mapping algo is slower, but gives some better result (visualy talking).
    *  You can used it when faces are closer (but not when a part of the face is
    *  behind the observer).
    * OSBOLETE
    */
    class QuadraticMapper : public Mapper
    {
      typedef Mapper inherited;
    private :
      // fill locals
      static realtype mxl;
      static realtype mxr;
      static realtype myl;
      static realtype myr;
      static realtype ll, lr;
      static realtype mxlb, mxle, mylb, myle;
      static realtype mxrb, mxre, myrb, myre;
      static realtype zlb, zle, zrb, zre;
      static int rylb, ryle, ryrb, ryre;
      static realtype txl, axl, tyl, ayl;
      static realtype txr, axr, tyr, ayr;
      //horiz locals
      static realtype l, mx, my, tx, ty, ax, ay;

    public :
      ///
      QuadraticMapper() { }
      ///
      inline void FillInitLocals(realtype zl, realtype zr, int ry) const
      {
        inherited::FillInitLocals(zl, zr, ry);
        mxl=conv(mappoints[0].x);
        mxr=conv(mappoints[0].x);
        myl=conv(mappoints[0].y);
        myr=conv(mappoints[0].y);
        ll=0; lr=0;
        mxlb=mxl; mxle=mxl; mylb=myl; myle=myl;
        mxrb=mxr; mxre=mxr; myrb=myr; myre=myr;
        zlb=zl; zle=zl; zrb; zre=zr;
        rylb=ry; ryle=ry; ryrb; ryre=ry;
        txl=0; axl=0; tyl=0; ayl=0;
        txr=0; axr=0; tyr=0; ayr=0;
      }
      ///
      inline void FillCallHoriz(realtype xl, realtype xr, realtype zl, realtype zr, int ry) const
      {
        mxb = mxl; myb = myl; mxe = mxr; mye = myr;
        inherited::FillCallHoriz(xl, xr, zl, zr, ry);
      }
      ///
      inline void FillNextLine(int ry) const
      {
        inherited::FillNextLine(ry);
        mxl += txl;  mxr += txr;
        txl += axl;  txr += axr;
        myl += tyl;  myr += tyr;
        tyl += ayl;  tyr += ayr;
      }
      ///
      inline void FillLeftChanging(int left) const
      {
        inherited::FillLeftChanging(left);
        zlb = zle;
        mxlb=mxle;
        mylb=myle;
        rylb=ryle;
      }
      ///
      inline void FillLeftChanged(PointZ * points, int left) const
      {
        inherited::FillLeftChanged(points, left);
        zle =fconv(points[left].z);
        mxle=conv(mappoints[left].x);
        myle=conv(mappoints[left].y);
        ryle=points[left].y;
      }
      ///
      inline void FillLeftSameCoef(int left) const
      {
        inherited::FillLeftSameCoef(left);
        mxl = conv(mappoints[left].x);
        myl = conv(mappoints[left].y);
        ll = frapport(zle,zlb);
      }
      ///
      inline void FillLeftNotSameCoef(realtype deltay) const
      {
        inherited::FillLeftNotSameCoef(deltay);
        ll = frapport(zle,zlb);
        QuadMapCoefsInit(conv(rylb), conv(ryle), ll, conv(rylb),
          mxlb, mxle, mylb, myle,
          mxl, myl, txl, tyl, axl, ayl);
      }
      ///
      inline void FillRightChanging(int right) const
      {
        inherited::FillRightChanging(right);
        zrb =zre;
        mxrb=mxre;
        myrb=myre;
        ryrb=ryre;
      }
      ///
      inline void FillRightChanged(PointZ * points, int right) const
      {
        inherited::FillRightChanged(points, right);
        zre =fconv(points[right].z);
        mxre=conv(mappoints[right].x);
        myre=conv(mappoints[right].y);
        ryre=points[right].y;
      }
      ///
      inline void FillRightSameCoef(int right) const
      {
        inherited::FillRightSameCoef(right);
        mxr = conv(mappoints[right].x);
        myr = conv(mappoints[right].y);
        lr = frapport(zre,zrb);
      }
      ///
      inline void FillRightNotSameCoef(realtype deltay) const
      {
        inherited::FillRightNotSameCoef(deltay);
        lr = frapport(zre,zrb);
        QuadMapCoefsInit(conv(ryrb), conv(ryre), lr, conv(ryrb),
          mxrb, mxre, myrb, myre,
          mxr, myr, txr, tyr, axr, ayr);
      }
      ///
      inline void HorizCoefInit(realtype xb, realtype xe, realtype zb, realtype ze) const
      {
        inherited::HorizCoefInit(xb, xe, zb, ze);
        assert(zb!=0);
        assert(ze!=0);
        assert( (zb>0&&ze>0)||(zb<0&&ze<0) );
        l = frapport(ze,zb);
      }
      ///
      inline void HorizLocalsClipped(realtype xb, realtype xe, int xd, int xf, realtype zb, realtype ze, int rxb, int rxe) const
      {
        inherited::HorizLocalsClipped(xb, xe, xd, xf, zb, ze, rxb, rxe);
        QuadMapCoefsInit(xb, xe, l, conv(xd+1), mxb, mxe, myb, mye,
          mx, my, tx, ty, ax, ay);
      }
      ///
      inline Color HorizPutPixel() const
      {
        assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())>=0);
        assert(int(Ima+ round(mx)+ bl*round(my)-ima->GetBuffer())<ima->Len());
        return *(Ima+ round(mx)+ bl*round(my));
      }
      ///
      inline void HorizNextPixel() const
      {
        inherited::HorizNextPixel();
        mx += tx;
        tx += ax;
        my += ty;
        ty += ay;
      }
    };

#define HMPRES 256
    /**
    *HYPPERBOLICMAPPER
    *  Hypperbolic mapping algo. This algo introduce no deformations. The face is
    *  mapped correctly. The only deformation can be due to precision problems. In
    *  that case you must modify the HMPRES define. You can replace it by a
    *  variable if needed. This algo should be used when face is cutted (a part is
    *  behind the observer, the other is in front of it for example). The
    *  performance is varying. When there is a pixelization effect this algo is
    *  faster than the Quadratic mapping algo. But is the pixel of the face are
    *  draw only one evry N pixel (when the face is far) this algo is slower. Note
    *  that the HorizNextPixel is not expanded inline (in BC5), due to while
    *  instruction, wich is slower.
    * OBSOLETE
    */
    class HypperbolicMapper : public Mapper
    {
      typedef Mapper inherited;
    private :
      // fill locals
      static int rmxl;
      static int rmxr;
      static int rmyl;
      static int rmyr;
      static int rzl;
      static int rzr;
      static int rmxlb, rmxle;
      static int rmylb, rmyle;
      static int rmxrb, rmxre;
      static int rmyrb, rmyre;
      static int rylb,  ryle;
      static int ryrb,  ryre;
      static int rzlb,  rzle;
      static int rzrb,  rzre;
      static int umxl, umxr, umyl, umyr, uzl, uzr;
      static int smxl, smxr, smyl, smyr, szl, szr;
      //horiz locals
      static int ux, uy;
      static int smx, smy;
      static int rmxe, rmye;
      static int rmxb, rmyb;
      static int rze, rzb;
      static int rden;
      static int rmx;
      static int rmy;
      static int p;
      static int ip;
      static int qx;
      static int iqx;
      static int qy;
      static int iqy;

    public :
      ///
      HypperbolicMapper() { }
      ///
      inline void FillAssertions(PointZ * points) const
      {
        for (int i=0; i<npoints; i++)
          points[i].z = points[i].z*HMPRES; // precision problem
      }
      ///
      inline void FillInitLocals(realtype zl, realtype zr, int ry) const
      {
        inherited::FillInitLocals(zl, zr, ry);
        rmxl=mappoints[0].x;
        rmxr=mappoints[0].x;
        rmyl=mappoints[0].y;
        rmyr=mappoints[0].y;
        rzl=round(zl);
        rzr=round(zr);
        rmxlb=rmxl; rmxle=rmxl;
        rmylb=rmyl; rmyle=rmyl;
        rmxrb=rmxr; rmxre=rmxr;
        rmyrb=rmyr; rmyre=rmyr;
        rylb=ry;    ryle=ry;
        ryrb=ry;    ryre=ry;
        rzlb=rzl;   rzle=rzl;
        rzrb=rzr;   rzre=rzr;
        umxl=0; umxr=0; umyl=0; umyr=0; uzl=0; uzr=0;
        smxl=0; smxr=0; smyl=0; smyr=0; szl=0; szr=0;
      }
      ///
      inline void FillCallHoriz(realtype xl, realtype xr, realtype zl, realtype zr, int ry) const
      {
        mxb = conv(rmxl); myb = conv(rmyl); mxe = conv(rmxr); mye = conv(rmyr);
        inherited::FillCallHoriz(xl, xr, conv(rzl), conv(rzr), ry);
      }
      ///
      inline void FillNextLine(int ry) const
      {
        inherited::FillNextLine(ry);
#define HYPINC(u,s,x,xb,xe,y,yb,ye,zb,ze)\
  u+=s*(zb*(ye-y)+ze*(y-yb));\
  while (u>0)\
        {\
        y+=s;\
        u+=zb*(xb-x)+ze*(x-xe);\
        }
        HYPINC(umxl,smxl,ry,rylb,ryle,rmxl,rmxlb,rmxle,rzlb,rzle);
        HYPINC(umyl,smyl,ry,rylb,ryle,rmyl,rmylb,rmyle,rzlb,rzle);
        HYPINC(umxr,smxr,ry,ryrb,ryre,rmxr,rmxrb,rmxre,rzrb,rzre);
        HYPINC(umyr,smyr,ry,ryrb,ryre,rmyr,rmyrb,rmyre,rzrb,rzre);
        HYPINC(uzl, szl, ry,rylb,ryle, rzl,rzlb, rzle, rzlb,rzle);
        HYPINC(uzr, szr, ry,ryrb,ryre, rzr,rzrb, rzre, rzrb,rzre);
      }
      ///
      inline void FillLeftChanging(int left) const
      {
        inherited::FillLeftChanging(left);
        rzlb =rzle;
        rmxlb=rmxle;
        rmylb=rmyle;
        rylb =ryle;
      }
      ///
      inline void FillLeftChanged(PointZ * points, int left) const
      {
        inherited::FillLeftChanged(points, left);
        rzle =int(points[left].z);
        rmxle=mappoints[left].x;
        rmyle=mappoints[left].y;
        ryle =points[left].y;
        smxl=sign(rmxle-rmxlb);
        smyl=sign(rmyle-rmylb);
        szl =sign(rzle-rzlb);
        umxl=0; umyl=0; uzl=0;
      }
      ///
      inline void FillLeftSameCoef(int left) const
      {
        inherited::FillLeftSameCoef(left);
        rmxl = mappoints[left].x;
        rmyl = mappoints[left].y;
      }
      ///
      inline void FillRightChanging(int right) const
      {
        inherited::FillRightChanging(right);
        rzrb =rzre;
        rmxrb=rmxre;
        rmyrb=rmyre;
        ryrb =ryre;
      }
      ///
      inline void FillRightChanged(PointZ * points, int right) const
      {
        inherited::FillRightChanged(points, right);
        rzre =int(points[right].z);
        rmxre=mappoints[right].x;
        rmyre=mappoints[right].y;
        ryre =points[right].y;
        smxr=sign(rmxre-rmxrb);
        smyr=sign(rmyre-rmyrb);
        szr =sign(rzre-rzrb);
        umxr=0; umyr=0; uzr=0;
      }
      ///
      inline void FillRightSameCoef(int right) const
      {
        inherited::FillRightSameCoef(right);
        rmxr = mappoints[right].x;
        rmyr = mappoints[right].y;
      }
      ///
      inline void HorizLocalsClipped(realtype xb, realtype xe, int xd, int xf, realtype zb, realtype ze, int rxb, int rxe) const
      {
        inherited::HorizLocalsClipped(xb, xe, xd, xf, zb, ze, rxb, rxe);
        ux=0; uy=0;
        smx=sign(mxe-mxb); smy=sign(mye-myb);
        rmxe=round(mxe)-smx; rmye=round(mye)-smy;
        rmxb=round(mxb); rmyb=round(myb);
        rze=round(ze); rzb=round(zb);
        rden=rzb*(rxb-(xd+1))+rze*((xd+1)-rxe);
        rmx=(rzb*rmxe*(rxb-(xd+1))+rze*rmxb*((xd+1)-rxe))/rden;
        rmy=(rzb*rmye*(rxb-(xd+1))+rze*rmyb*((xd+1)-rxe))/rden;
        p=rzb*(rxb-(xd+1))+rze*((xd+1)-rxe);
        ip=rze-rzb;
        qx=smx*(rzb*(rmxe-rmx)+rze*(rmx-rmxb));
        iqx=(rze-rzb);
        qy=smy*(rzb*(rmye-rmy)+rze*(rmy-rmyb));
        iqy=(rze-rzb);
      }
      ///
      inline Color HorizPutPixel() const
      {
        // assertions don't work, see why
        //assert(int(Ima+ rmx+ bl*rmy-ima->GetBuffer())>=0);
        //assert(int(Ima+ rmx+ bl*rmy-ima->GetBuffer())<ima->Len());
        return *(Ima+ rmx+ bl*rmy);
      }
      ///
      inline void HorizNextPixel() const
      {
        inherited::HorizNextPixel();
        // not expanded inline -> slower
        ux += qx;
        while (ux>0)
        {
          rmx+=smx;
          qx +=iqx;
          ux += p;
        }
        uy += qy;
        while (uy>0)
        {
          rmy+=smy;
          qy +=iqy;
          uy += p;
        }
        p += ip;
      }
    };

  }
}

#endif
