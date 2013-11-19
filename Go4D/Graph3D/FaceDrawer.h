#ifndef _FACE_DRAWER_H_
#define _FACE_DRAWER_H_

/*
*AUTHOR : Babel yoann
*TITLE  : FaceDrawer.h : all routines to draw in a face
*CREATED: 13/07/96
*DATE   : 13/07/96
*OBJECT : all the face drawing algo : z-buffer, flat, gouraud, mapping, bump
*NOTE   : now this unit is OBSOLETE, use TemplateFaceDrawer.h instead
*         because the other method is more powerful.
*/

#include "Graph2D/PlugDrawer.h"
#include "Graph2D/Clut.h"

namespace Go4D
{

    /**
    * AFACEDRAWER
    *   this is the basis for all face drawer, abstract class that just has an
    *   abstract method : Draw, and an other to cut faces
    *   OBSOLETE
    */
    class AFaceDrawer : public PluggedDrawer
    {
    protected :
      typedef PluggedDrawer inherited; // YOB GCC4.1
      int fCount;
      const Image * fImage;

    public :
      ///
      AFaceDrawer(const Image &, int PointsCount);
      ///
      virtual void Draw(int PointsCount, PointZ * Points) const =0;
      ///
      virtual AFaceDrawer * Clone(int NewPointsCount) const =0;
      ///
      virtual void SplitExtrem(AFaceDrawer * Parent, int FirstPositive,
        int FirstNegative, int PositiveCount, int FirstSign,
        real FirstCoord, real SecondCoord)=0;
      ///
      int Count() const { return fCount; }
      ///
      const Image * GetPluggedImage() const { return fImage; }

      ///  this method is set here for the moment
      virtual void SetMapPoint(int PointIndice, int MapX, int MapY) {}
      ///
      virtual int GetMapWidth() { return 0; }
      ///
      virtual int GetMapHeight() { return 0; }
    };

  namespace Obsolete
  {

    /// abstract Draw for flat faces
    /// OBSOLETE
    class AFlatFaceDrawer : public AFaceDrawer
    {
      typedef AFaceDrawer inherited;

    public :
      AFlatFaceDrawer(const Image &, int PointsCount);
      virtual void Draw(int PointsCount, PointZ * Points) const =0;
      virtual AFaceDrawer * Clone(int NewPointsCount) const =0;
      virtual void SplitExtrem(AFaceDrawer * Parent, int FirstPositive,
        int FirstNegative, int PositiveCount, int FirstSign,
        real FirstCoord, real SecondCoord);
    };

    /// draw flat faces
    /// OBSOLETE
    class FlatFaceDrawer : public AFlatFaceDrawer
    {
      typedef AFlatFaceDrawer inherited;
    private :
      void Horiz(realtype xb, realtype xe, realtype zb, realtype ze, int y, Color color) const;

    public :
      FlatFaceDrawer(const Image &, int PointsCount);
      virtual AFaceDrawer * Clone(int NewPointsCount) const;
      virtual void Draw(int PointsCount, PointZ * Points) const; // teh color of the first point is used
    };

    /// draw goudaud mapped faces
    /// OBSOLETE
    class GouraudFaceDrawer : public AFlatFaceDrawer
    {
      typedef AFlatFaceDrawer inherited;
    private :
      void Horiz(realtype xb, realtype xe, realtype zb, realtype ze, int y, int cb, int ce) const;

    public :
      GouraudFaceDrawer(const Image &, int PointsCount);
      virtual AFaceDrawer * Clone(int NewPointsCount) const;
      virtual void Draw(int PointsCount, PointZ * Points) const;
    };

    /// draw image mapped face
    /// OBSOLETE
    class AMappedFaceDrawer : public AFaceDrawer
    {
      typedef AFaceDrawer inherited;
    protected :
      struct MapPoint
      {
        int x;
        int y;
      };
      const Image * fMappedImage;
      MapPoint * mpoints, * dupmpoints;

    public :
      AMappedFaceDrawer(const Image &, int PointsCount, const Image &);
      AMappedFaceDrawer(const Image &, const Image *);
      virtual ~AMappedFaceDrawer();
      virtual void Draw(int PointsCount, PointZ * Points) const =0;
      virtual AFaceDrawer * Clone(int NewPointsCount) const =0;
      virtual void SplitExtrem(AFaceDrawer * Parent, int FirstPositive,
        int FirstNegative, int PositiveCount, int FirstSign,
        real FirstCoord, real SecondCoord);

      void SetMapImage(const Image * mapImage);
      virtual void SetMapPoint(int PointIndice, int MapX, int MapY);
      virtual int GetMapWidth() { return fMappedImage->Width(); }
      virtual int GetMapHeight() { return fMappedImage->Height(); }
    };

    /// draw linear mapped faces
    /// OBSOLETE
    class LinearMappedFaceDrawer : public AMappedFaceDrawer
    {
      typedef AMappedFaceDrawer inherited;
    private :
      void Horiz(realtype xb, realtype xe, realtype zb, realtype ze,
        int y, realtype mxb, realtype myb,
        realtype mxe, realtype mye, const Image * ima) const;

    public :
      LinearMappedFaceDrawer(const Image &, int PointsCount, const Image &);
      LinearMappedFaceDrawer(const Image &, const Image *);
      void Draw(int PointsCount, PointZ * Points) const;
      AFaceDrawer * Clone(int NewPointsCount) const;
    };

    /// draw quadratic mapped faces
    /// OBSOLETE
    class QuadraticMappedFaceDrawer : public AMappedFaceDrawer
    {
      typedef AMappedFaceDrawer inherited;
    private :
      void Horiz(realtype xb, realtype xe, realtype zb, realtype ze,
        int y, realtype mxb, realtype myb,
        realtype mxe, realtype mye, const Image * ima) const;

    public :
      QuadraticMappedFaceDrawer(const Image &, int PointsCount, const Image &);
      QuadraticMappedFaceDrawer(const Image &, const Image *);
      void Draw(int PointsCount, PointZ * Points) const;
      AFaceDrawer * Clone(int NewPointsCount) const;
    };

    /// draw hypperbolic mapped faces
    /// OBSOLETE
    class HypperbolicMappedFaceDrawer : public AMappedFaceDrawer
    {
      typedef AMappedFaceDrawer inherited;
    private :
      void Horiz(realtype xb, realtype xe, realtype zb, realtype ze,
        int y, realtype mxb, realtype myb,
        realtype mxe, realtype mye, const Image * ima) const;

    public :
      HypperbolicMappedFaceDrawer(const Image &, int PointsCount, const Image &);
      HypperbolicMappedFaceDrawer(const Image &, const Image *);
      void Draw(int PointsCount, PointZ * Points) const;
      AFaceDrawer * Clone(int NewPointsCount) const;
    };

    /// draw faces with clut
    /// OBSOLETE
    class AClutMappedFaceDrawer : public AMappedFaceDrawer
    {
      typedef AMappedFaceDrawer inherited;
    protected :
      const MultiClut * fMultiClut;

    public :
      AClutMappedFaceDrawer(const Image &, int PointsCount, const Image &, const MultiClut &);
      AClutMappedFaceDrawer(const Image &, const Image *, const MultiClut &);
      virtual void Draw(int PointsCount, PointZ * Points) const =0;
      virtual AFaceDrawer * Clone(int NewPointsCount) const =0;
    };

    /// draw faces with mapped & shaded image
    /// OBSOLETE
    class AShadedMappedFaceDrawer : public AClutMappedFaceDrawer
    {
      typedef AClutMappedFaceDrawer inherited;

    public :
      AShadedMappedFaceDrawer(const Image &, int PointsCount, const Image &, const MultiShadeClut &);
      AShadedMappedFaceDrawer(const Image &, const Image *, const MultiShadeClut &);
      virtual void Draw(int PointsCount, PointZ * Points) const =0;
      virtual AFaceDrawer * Clone(int NewPointsCount) const =0;
    };

    /// linear mapped & shaded face drawer
    /// OBSOLETE
    class ShadedLinearMappedFaceDrawer : public AShadedMappedFaceDrawer
    {
      typedef AShadedMappedFaceDrawer inherited;
    private :
      void Horiz(realtype xb, realtype xe, realtype zb, realtype ze,
        int y, realtype mxb, realtype myb,
        realtype mxe, realtype mye, const Image * ima, Color shade) const;

    public :
      ShadedLinearMappedFaceDrawer(const Image &, int PointsCount, const Image &, const MultiShadeClut &);
      ShadedLinearMappedFaceDrawer(const Image &, const Image *, const MultiShadeClut &);
      virtual void Draw(int PointsCount, PointZ * Points) const;
      virtual AFaceDrawer * Clone(int NewPointsCount) const;
    };

    /// Draw transluent faces
    /// OBSOLETE
    class TransluentFaceDrawer : AFlatFaceDrawer
    {
      typedef AFlatFaceDrawer inherited;
    private :
      const TransluentClut * fTransluentClut;
      void Horiz(realtype xb, realtype xe, realtype zb, realtype ze, int y, Color color) const;

    public :
      TransluentFaceDrawer(const Image &, int PointsCount, const TransluentClut &);
      virtual AFaceDrawer * Clone(int NewPointsCount) const;
      virtual void Draw(int PointsCount, PointZ * Points) const;
    };

  }

}

#endif
