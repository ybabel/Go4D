/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : sdrawer.h
 *CREATED: 09/03/96
 *DATE   : 09/03/96
 *OBJECT : drawing algo, using s-buffer
 *DEFS   : usefixed, usefloat
\*****************************************************************************/


#ifndef _SDRAWER_H_
#define _SDRAWER_H_

#include "drawer.h"
#include "memmgm.h"


/*****************************************************************************\
 *SEGMENT : basic segment (uniform color). Specifie the clipping area, and 
 *  coordinate of the segment
 *
 *Clip return TRUE in case the entire segment is not visible on the screen.
 *  return FALSE in the other case. Clip the segment in other that no pixel
 *  of the segment belong out screen.
 *Cut : specifie new x-coordinates for the segment. Clip the new segment
 *Draw : call the SDrawer basic drawing routine.
 *SplitLeft : split segment in 2 parts, return the left part. the newx where
 *  the segment is splitted must  be in the segment
 *SplitLeft : same but return the right part, *this become the left part.
 *Intersect : calculate the x where the 2 segments intersects;
 *Visible : return FALSE in case the segment is too small to be visible
 *
 *SHADESEGMENT : segment with gouraud shading
 *MAPSEGMENT : segment with an image mapped on it.
 *QUADMAPSEGMENT : the method of mapping is different (more realistic).
 *HYPMAPSEGMENT : hypperbolic mapped segment
 *
\*****************************************************************************/

class Segment 
{
  public :
  Segment() {}
  Segment(int xmin, int ymin, int xmax, int ymax,
	  realtype xb, realtype xe, realtype zb, realtype ze, 
	  int y, byte color=0);
  int Clip();
  void Cut(int newxd, int newxf);
  virtual void Draw(SDrawer &) const;
  realtype Intersect(const Segment &) const;
  int Behind(const Segment &) const;
  int Visible() const {return ! (xd==xf);}
  int GetXd() const {return xd;}
  int GetXf() const {return xf;}
  real GetZd() const {return zd;}
  real GetZf() const {return zf;}
  int GetY() const {return y;}
  
  #ifdef LIBGPP
  friend ostream & operator << (ostream &, const Segment &);
  #endif

  protected :
  int BehindZ(const Segment &) const; // just simple Z-compare
  int BehindF(const Segment &) const; // compare the 2 points
  int AboveF(const Segment &) const; 
  virtual int BasicClip();
  int Behind(int x, real z) const;
  int Upon(int x, real z) const;
  
  int xmin, xmax, ymin, ymax;
  realtype xb;
  realtype xe;
  realtype zb;
  realtype ze;
  int y;

  // result of clipping and cutting
  int xd, xf;
  real zd, zf;

  private :
  byte color;
  real deltax, deltaz, osign;
};

class ShadeSegment : public Segment
{
  public :
  ShadeSegment() :Segment() {}
  ShadeSegment(int xmin, int ymin, int xmax, int ymax,
	       realtype xb, realtype xe, realtype zb, realtype ze,
	       int y, realtype cb, realtype ce);
  void Cut(int newxd, int newxf);
  void Draw(SDrawer &) const;
  
  private :
  int BasicClip();
  realtype cb;
  realtype ce;
  realtype c, ic; // gouraud shading
};

class MapSegment : public Segment
{
  public :
  MapSegment() : Segment() {}
  MapSegment(int xmin, int ymin, int xmax, int ymax,
	     realtype xb, realtype xe,
	     realtype zb, realtype ze, int y, 
	     realtype mxb, realtype myb, realtype mxe, realtype mye, 
	     Image& ima );
  void Cut(int newxd, int newxf);
  void Draw(SDrawer &) const;
  
  protected :
  int BasicClip();
  
  realtype mxb;
  realtype myb;
  realtype mxe;
  realtype mye;
  Image * ima;
  realtype mx, my;
  
  private :
  realtype imx, imy;
};

class QuadMapSegment : public MapSegment
{
  public :
  QuadMapSegment() : MapSegment() {}
  QuadMapSegment(int xmin, int ymin, int xmax, int ymax,
		 realtype xb, realtype xe, 
		 realtype zb, realtype ze, int y, 
		 realtype mxb, realtype myb, 
		 realtype mxe, realtype mye, 
		 Image& ima);
  void Cut(int newxd, int newxf);
  void Draw(SDrawer &) const;
  
  private :
  int BasicClip();
  
  realtype l, tx, ty, ax, ay;
};

class HypMapSegment : public MapSegment
{
  public :
  HypMapSegment() : MapSegment() {}
  HypMapSegment(int xmin, int ymin, int xmax, int ymax,
		realtype xb, realtype xe, 
		realtype zb, realtype ze, int y, 
		realtype mxb, realtype myb, 
		realtype mxe, realtype mye, 
		Image& ima);
  void Cut(int newxd, int newxf);
  void Draw(SDrawer &) const;
  
  private :
  int BasicClip();
};

/*****************************************************************************\
 * SEGMENTMANGER
\*****************************************************************************/

typedef MemManager<Segment> SegmentMemManager;
typedef MemManager<ShadeSegment> ShadeSegmentMemManager;
typedef MemManager<MapSegment> MapSegmentMemManager;
typedef MemManager<QuadMapSegment> QuadMapSegmentMemManager;
typedef MemManager<HypMapSegment> HypMapSegmentMemManager;

class AbstractSegmentManager
{
  public :
   virtual Segment * New(const Segment *)=0;
};

class SegmentManager 
: public SegmentMemManager, public AbstractSegmentManager
{
  public :
   Segment * New(const Segment *);
};

class ShadeSegmentManager 
: public ShadeSegmentMemManager, public AbstractSegmentManager
{
  public :
   Segment * New(const Segment *);
};

class MapSegmentManager 
: public MapSegmentMemManager, public AbstractSegmentManager
{
  public :
   Segment * New(const Segment *);
};

class QuadMapSegmentManager 
: public QuadMapSegmentMemManager, public AbstractSegmentManager
{
  public :
   Segment * New(const Segment *);
};

class HypMapSegmentManager 
: public HypMapSegmentMemManager, public AbstractSegmentManager
{
  public :
   Segment * New(const Segment *);
};

/*****************************************************************************\
 *SDRAWER : same thing than drawer, but s-buffer is added, to all the routines
 *  there is a list of segment for each lines of the screen. the SDrawer is 
 *  is the owner of all the segments.
 *
 *Flush : when all the desired segments have been added to the list, call this 
 *  method to draw the screen. If you don't call this nothing will be drawn.
 *Purge : delete the list of segments (and all the segments, SDrawer is the 
 *  owner).
 *Insert : insert a segment in the list of segments to draw. You must garant
 *  that the segment had been clipped, and that it is well ordered (means
 *  xb < xe). Then the routine insert the segment at the good position
 *  and cut it if necessary, in order that no segment overlap another.
 *  The segment are also cutted in order that ervy pixel of the segment
 *  is not hidden by an other segment
 *  An AbstractSegmentManager must be passed in parameter because in some
 *  case we must allocate a new segment.
 *InsertBeforeNode : insert the segment in the list before the specified 
 *  node.
 *Behind : return TRUE in case the entrire segment is behind the other
 *...Horiz : insert the appropriate segment in the segments list.
 *NoClip...Horiz : Draw the segment. basic drawing : no clipping, no
 *  calculation, all the  coefs must have been pre-calculated
\*****************************************************************************/

typedef Segment * PSegment;

struct SegmentList
{
  PSegment seg; // need pointer because can be Segmment or children
  SegmentList * next;
  SegmentList();
  SegmentList(Segment *);
};

typedef SegmentList * PSegmentList;
typedef MemManager<SegmentList> SegmentListManager;

class SDrawer : public Drawer
{
  friend Segment;
  friend ShadeSegment;
  friend MapSegment;
  friend QuadMapSegment;
  friend HypMapSegment;
  public :
  SDrawer(int width, int height, int border=0);
  virtual ~SDrawer();
  void Flush();

  protected :
  void Purge();
  void Insert(Segment *, AbstractSegmentManager &);
  void InsertBeforeNode(Segment *, PSegmentList cur, PSegmentList prec);
  void InsertAfterNode(Segment *, PSegmentList cur);
  void Horiz(realtype xb, realtype xe, 
	     realtype zb, realtype ze, int y, byte color);
  void ShadeHoriz(realtype xb, realtype xe, realtype zb, realtype ze,
		  int y, realtype cb, realtype ce);
  void MapHoriz(realtype xb, realtype xe, 
		realtype zb, realtype ze, int y, 
		realtype mxb, realtype myb, 
		realtype mxe, realtype mye, 
		Image&);
  void QuadMapHoriz(realtype xb, realtype xe, 
		    realtype zb, realtype ze, 
		    int y, realtype mxb, realtype myb, 
		    realtype mxe, realtype mye, Image&);
  void HypMapHoriz(realtype xb, realtype xe, 
		   realtype zb, realtype ze, 
		   int y, realtype mxb, realtype myb, 
		   realtype mxe, realtype mye, Image&);
  
  void NoClipHoriz(int y, byte color,
		   int xd, int xf);
  void NoClipShadeHoriz(int y,
			int xd, int xf,
			realtype c, realtype ic);
  void NoClipMapHoriz(int y, 
		      realtype mxb, realtype myb, 
		      realtype mxe, realtype mye, 
		      Image&,
		      int xd, int xf,
		      realtype mx, realtype imx,
		      realtype my, realtype imy);
  void NoClipQuadMapHoriz(realtype xb, realtype xe, realtype l,
			  int y, 
			  realtype mxb, realtype myb, 
			  realtype mxe, realtype mye, 
			  Image&,
			  int xd, int xf,
			  realtype mx, realtype my,
			  realtype tx, realtype ty,
			  realtype ax, realtype ay);
  void NoClipHypMapHoriz(realtype xb, realtype xe, 
			 realtype zb, realtype ze, 
			 int y, 
			 int xd, int xf,
			 realtype mxb, realtype myb, 
			 realtype mxe, realtype mye, Image&);
  
  PSegmentList * segs;
  SegmentListManager seglistmgm;
  SegmentManager segmgm;
  ShadeSegmentManager shadesegmgm;
  MapSegmentManager mapsegmgm;
  QuadMapSegmentManager quadmapsegmgm;
  HypMapSegmentManager hypmapsegmgm;
};

#endif
