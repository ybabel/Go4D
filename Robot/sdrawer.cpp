/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : sdrawer.cc, sdrawer.h implementation
 *CREATED: 09/03/96
 *DATE   : 09/03/96
 *BUGS   :
 *NOTE   : improvements in SDrawer :
 *         - use a binary tree for each line instead of a list
\*****************************************************************************/

#include "sdrawer.h"

/*****************************************************************************\
 *   Segment
\*****************************************************************************/

#define sbuffer

Segment::Segment(int pxmin, int pymin, int pxmax, int pymax,
		 realtype pxb, realtype pxe, realtype pzb, realtype pze,
		 int py, byte pcolor)
{
  xmin = pxmin;
  ymin = pymin;
  xmax  = pxmax;
  ymax = pymax;
  xb = pxb;  xe = pxe;
  zb = pzb;  ze = pze;
  y = py;
  color = pcolor;
  deltax = roundf(xe-xb);
  deltaz = roundf(ze-zb);
  osign = deltaz*roundf(xb) - deltax*roundf(zb); 
  xd = 0; xf = 0;
  zd = 0; zf = 0;
}

 int Segment::Clip()
{
  int result = BasicClip();
  if ( !result ) //the segment is visible
    {
      real pente = deltaz/deltax;
      zd = pente*(real(xd)-roundf(xb)) + roundf(zb);
      zf = pente*(real(xf)-roundf(xb)) + roundf(zb);
    }
  else
    {
      xd = 0; xf = 0;
      zd = 0; zf = 0;
    }
  return result;
}

 int Segment::BasicClip()
{
  #define flat
  #include "cliph.cpp"
  #undef flat
}

 void Segment::Cut(int newxd, int newxf)
{
  assert(newxd <= newxf);
  xmin = maximum(xmin, newxd);
  xmax = minimum(xmax, newxf+1); // because clip to xmax-1
  Clip();
}

 void Segment::Draw(SDrawer & d) const
{
  if (xd!=0 || xf!=0)
    d.NoClipHoriz(y, color, xd, xf);
}

//INTERSECT : the segments must be large enough. (otherwise division pb).
//  find the x intersecting the 2 segments
//-----------------------------------------------------------------------------
 realtype Segment::Intersect(const Segment & seg) const
{
  real deltaxp = roundf(seg.xe-seg.xb);
  real deltax = roundf(xe-xb);
  assert( abs(deltaxp)>= conv(1) );
  assert( abs(deltax) >= conv(1) );

  real cnump = roundf(seg.xe)*roundf(seg.zb) - roundf(seg.xb)*roundf(seg.ze);
  real cnum = roundf(xe)*roundf(zb) - roundf(xb)*roundf(ze);
  real deltazp = roundf(seg.ze-seg.zb);
  real deltaz = roundf(ze-zb);
  
  real num = cnump/deltax - cnum/deltaxp;
  real denom = deltaz/deltaxp - deltazp/deltax;

  return frapport(num, denom);
}

#define ZPRECISION 2
 int Segment::BehindZ(const Segment & seg) const
{
  return (zf>=(seg.zf)) && (zd>=(seg.zd));
}

 int Segment::BehindF(const Segment & seg) const
{
  return seg.Behind(xd, zd) && seg.Behind(xf, zf);
}

 int Segment::AboveF(const Segment & seg) const
{
  return seg.Upon(xd, zd) && seg.Upon(xf, zf);
}

#define SIGNPREC 0
 int Segment::Behind(int x, real z) const
{
  real psign = deltaz*real(x) - deltax*z - osign;
  return (psign<=SIGNPREC && (-osign)>=-SIGNPREC) 
         || (psign>=-SIGNPREC && (-osign)<=SIGNPREC);
}

 int Segment::Upon(int x, real z) const
{
  real psign = deltaz*real(x) - deltax*z - osign;
  return (psign<=SIGNPREC && (-osign)<=SIGNPREC) 
         || (psign>=-SIGNPREC && (-osign)>=-SIGNPREC);
}

//BEHIND : determine if *this is behind (from the observer (0,0,0) point of
//  view) or not. This test is working well, provided the segments are not
//  interpenetrationg. The first test (the one thaht is in commentary don't
//  work in all the case but is mush more faster, maybe it could be useful
//  in some pecular cases, so don't discardd it) :
//     ^ z                   /
//     |               zd   /
//     |               /   /     this case cannot be resolved by 1srt test
//     |              /   seg.zf
//     +---->x       / 
//-----------------------------------------------------------------------------
int Segment::Behind(const Segment & seg) const
{
  if (xd > seg.xd) 
    if (zd > seg.zf) return TRUE;
  else if (xd < seg.xd)
    if (zf > seg.zd) return TRUE;

  if ( (xd==seg.xd) && (abs(zd-seg.zd)<ZPRECISION) )
    return zf > seg.zf;
  if ( (xf==seg.xf) && (abs(zf-seg.zf)<ZPRECISION) )
    return zd > seg.zd;
      
  if (BehindZ(seg)) return TRUE;
  
  if (BehindF(seg)) return TRUE;
  if (seg.AboveF(*this)) return TRUE;

  return FALSE;
}

#ifdef LIBGPP
ostream & operator << (ostream & os, const Segment & seg)
{
  os << "(" << seg.xd << "," << seg.zd << ")";
  os << "(" << seg.xf << "," << seg.zf << ") ";
  return os;
}
#endif


/*****************************************************************************\
 *   ShadeSegment
\*****************************************************************************/


ShadeSegment::ShadeSegment(int pxmin, int pymin, int pxmax, int pymax,
			   realtype pxb, realtype pxe, realtype pzb,
			   realtype pze, int py, realtype pcb,
			   realtype pce)
:Segment(pxmin, pymin, pxmax, pymax,
	 pxb, pxe, pzb, pze, py)
{
  cb = pcb;
  ce = pce;
}

int ShadeSegment::BasicClip()
{
  #define gouraud
  #include "cliph.cpp"
  #undef gouraud
}

void ShadeSegment::Cut(int newxd, int newxf)
{
  int deltaxd= newxd-xd;
  Segment::Cut(newxd, newxf);
  c += ic*deltaxd;
}

void ShadeSegment::Draw(SDrawer &d) const
{
  if (xd!=0 || xf!=0)
    d.NoClipShadeHoriz(y, xd, xf, c, ic);
}


/*****************************************************************************\
 *   MapSegment
\*****************************************************************************/


MapSegment::MapSegment(int pxmin, int pymin, int pxmax, int pymax,
		       realtype pxb, realtype pxe, realtype pzb,
		       realtype pze, int py, realtype pmxb, realtype pmyb,
		       realtype pmxe, realtype pmye, Image &pima)
:Segment(pxmin, pymin, pxmax, pymax,
	 pxb, pxe, pzb, pze, py)
{
  mxb = pmxb;
  mxe = pmxe;
  myb = pmyb;
  mye = pmye;
  ima = &pima;
}

int MapSegment::BasicClip()
{
  #define mapping
  #include "cliph.cpp"
  #undef mapping
}

void MapSegment::Cut(int newxd, int newxf)
{
  int deltaxd= newxd-xd;
  Segment::Cut(newxd, newxf);
  mx += imx*deltaxd;
  my += imy*deltaxd;  
}

void MapSegment::Draw(SDrawer & d) const
{
  if (xd!=0 || xf!=0)
    d.NoClipMapHoriz(y, mxb, myb, mxe, mye, *ima,
		     xd, xf, mx, imx, my, imy);
}


/*****************************************************************************\
 *   QuadMapSegment
\*****************************************************************************/


QuadMapSegment::QuadMapSegment(int pxmin, int pymin, int pxmax, int pymax,
			       realtype pxb, realtype pxe, realtype pzb,
			       realtype pze, int py, realtype pmxb, 
			       realtype pmyb, realtype pmxe, 
			       realtype pmye, Image &pima)
:MapSegment(pxmin, pymin, pxmax, pymax,
	    pxb, pxe, pzb, pze, py, pmxb, pmyb, pmxe, pmye, pima)
{ }

int QuadMapSegment::BasicClip()
{
  #define quadmapping
  #include "cliph.cpp"
  #undef quadmapping

}

void QuadMapSegment::Cut(int newxd, int newxf)
{
  Segment::Cut(newxd, newxf);
}

void QuadMapSegment::Draw(SDrawer & d) const
{
  if (xd!=0 || xf!=0)
    d.NoClipQuadMapHoriz(xb, xe, l,
			 y, mxb, myb, mxe, mye, *ima,
			 xd, xf, mx, my, tx, ty, ax, ay);
}

/*****************************************************************************\
 *   HypMapSegment
\*****************************************************************************/


HypMapSegment::HypMapSegment(int pxmin, int pymin, int pxmax, int pymax,
			     realtype pxb, realtype pxe, realtype pzb,
			     realtype pze, int py, realtype pmxb, 
			     realtype pmyb, realtype pmxe,
			     realtype pmye, Image &pima)
:MapSegment(pxmin, pymin, pxmax, pymax,
	    pxb, pxe, pzb, pze, py, pmxb, pmyb, pmxe, pmye, pima)
{ }

int HypMapSegment::BasicClip()
{
  #define hypmapping
  #include "cliph.cpp"
  #undef hypmapping
}

void HypMapSegment::Cut(int newxd, int newxf)
{
  Segment::Cut(newxd, newxf);
}

void HypMapSegment::Draw(SDrawer & d) const
{
  if (xd!=0 || xf!=0)
    d.NoClipHypMapHoriz(xb, xe, zb, ze,
			y, xd, xf, mxb, myb, mxe, mye, *ima);
}


/*****************************************************************************\
 *   SegmentManager
\*****************************************************************************/

Segment * SegmentManager::New(const Segment * seg)
{
  return SegmentMemManager::New(*(Segment*)seg);
}

Segment * ShadeSegmentManager::New(const Segment * seg)
{
  return ShadeSegmentMemManager::New(*(ShadeSegment*)seg);
}

Segment * MapSegmentManager::New(const Segment * seg)
{
  return MapSegmentMemManager::New(*(MapSegment*)seg);
}

Segment * QuadMapSegmentManager::New(const Segment * seg)
{
  return QuadMapSegmentMemManager::New(*(QuadMapSegment*)seg);
}

Segment * HypMapSegmentManager::New(const Segment * seg)
{
  return HypMapSegmentMemManager::New(*(HypMapSegment*)seg);
}

/*****************************************************************************\
 *   SDrawer
\*****************************************************************************/


SegmentList::SegmentList(Segment * pseg)
{
  seg = pseg;
  next = NULL;
}

SegmentList::SegmentList()
{
  seg = NULL;
  next = NULL;
}

SDrawer::SDrawer(int pwidth, int pheight, int pborder)
:Drawer(pwidth, pheight, pborder)
{
  alloc( segs = new PSegmentList[fHeight]);
  for (int i=0; i<fHeight; i++)
    segs[i] = NULL;
}

SDrawer::~SDrawer()
{
  Purge();
  delete [] segs;
}

//PURGE : free the memory allocated for the segments, or if a memory manager
//  is present purge it.
//-----------------------------------------------------------------------------
void SDrawer::Purge()
{
  segmgm.Purge();
  shadesegmgm.Purge();
  mapsegmgm.Purge();
  quadmapsegmgm.Purge();
  hypmapsegmgm.Purge();
  seglistmgm.Purge(); 
  for (int i=0; i<fHeight; i++)
      segs[i] = NULL;
}

//FLUSH : draw all the segments and purge them.
//-----------------------------------------------------------------------------
void SDrawer::Flush()
{
  for (int y=0; y<fHeight; y++)
    for(PSegmentList root = segs[y]; root != NULL; root=root->next)
      root->seg->Draw(*this);
  Purge();
}

//HORIZ : insert an horizontal flat segment in the list of segments. (there
//  is a list for each line of the screen, so insert it in the right list)
//-----------------------------------------------------------------------------
void SDrawer::Horiz(realtype xb, realtype xe, 
		    realtype zb, realtype ze, int y,byte color)
{
  Segment tmp(xmin, ymin, xmax, ymax, xb, xe, zb, ze, y, color);
  if ( !tmp.Clip() )
    Insert( segmgm.New(&tmp), segmgm );
}

//SHADEHORIZ : insert an horizontal gouraud shaded segment
//-----------------------------------------------------------------------------
void SDrawer::ShadeHoriz(realtype xb, realtype xe, realtype zb, realtype ze,
			 int y, realtype cb, realtype ce)
{
  ShadeSegment tmp(xmin, ymin, xmax, ymax, xb, xe, zb, ze, y, cb, ce);
  if ( !tmp.Clip() )
    Insert( shadesegmgm.New(&tmp), shadesegmgm );
}

//MAPHORIZ : insert a linear mapped segment
//-----------------------------------------------------------------------------
void SDrawer::MapHoriz(realtype xb, realtype xe, realtype zb, realtype ze,
		      int y, realtype mxb, realtype myb, 
		      realtype mxe, realtype mye, Image & ima)
{
  MapSegment tmp(xmin, ymin, xmax, ymax, 
		 xb, xe, zb, ze, y, mxb, myb, mxe, mye, ima);
  if ( !tmp.Clip() )
    Insert( mapsegmgm.New(&tmp), mapsegmgm );
}

//QUADMAPHORIZ : insert a quadratic mapped segment
//-----------------------------------------------------------------------------
void SDrawer::QuadMapHoriz(realtype xb,realtype xe,realtype zb, realtype ze, 
			   int y, realtype mxb, realtype myb, 
			   realtype mxe, realtype mye, 
			   Image & ima)
{
  QuadMapSegment tmp(xmin, ymin, xmax, ymax, 
		     xb, xe, zb, ze, y, mxb, myb, mxe, mye, ima);
  if ( !tmp.Clip() )
    Insert( quadmapsegmgm.New(&tmp), quadmapsegmgm);
}

//HYPMAPHORIZ : insert a quadratic mapped segment
//-----------------------------------------------------------------------------
void SDrawer::HypMapHoriz(realtype xb,realtype xe,realtype zb, realtype ze, 
			  int y, realtype mxb, realtype myb, 
			  realtype mxe, realtype mye, 
			  Image & ima)
{
  HypMapSegment tmp(xmin, ymin, xmax, ymax, 
		    xb, xe, zb, ze, y, mxb, myb, mxe, mye, ima);
  if ( !tmp.Clip() )
    Insert( hypmapsegmgm.New(&tmp), hypmapsegmgm);
}

#define MESS(x)

//INSERT : insert a segment, whatever is his type. (find is position in the
//  list, and if its necessary cut it or the other segments).
//-----------------------------------------------------------------------------
void SDrawer::Insert(Segment *seg, AbstractSegmentManager & segmgm)
// simplified version, not supporting segments interpenetrating
{
  //these assertion verifies that the clipping has well fonctionned
  assert(seg->GetY() >= 0);
  assert(seg->GetY() < fHeight );
  assert(seg->GetXd() < seg->GetXf() );

  #define root segs[seg->GetY()]
  #define NEXT_SEGMENT prec=cur; cur=cur->next; continue

  PSegmentList prec=NULL, cur=root;
  if (root == NULL) 
    {root = seglistmgm.New(SegmentList(seg)); return;}

  while (cur != NULL)
    {
      if (!cur->seg->Visible())
	{
	  NEXT_SEGMENT;
	}
      else if ( seg->GetXd() >= cur->seg->GetXf() )
	{ //        nnnnn     -> *seg
	  // ccccc            -> *cur->seg
	  //just compare with the next segment
	  //----------------------------------
	  NEXT_SEGMENT;
	}
      else if ( seg->GetXf()  <= cur->seg->GetXd() )
	{ // nnnnnn           -> *seg
	  //         cccccc   -> *cur->seg
          //just insert the segment
	  //------------------------------
	  InsertBeforeNode(seg, cur, prec);
	  return;
	}
      else if ( seg->GetXd() < cur->seg->GetXd() )
	{ 
	  if  (seg->Behind(*cur->seg))
	    { 
	      if (cur->seg->GetXf() <= seg->GetXf())
		{ //  nnnnnnnnn
		  //     cccc
		  // insert the two visible parts (the middle is not visible)
		  //---------------------------------------------------------
		  MESS("case 1");
		  Segment * tmpseg = segmgm.New(seg);
		  tmpseg->Cut(seg->GetXd(), cur->seg->GetXd());
		  InsertBeforeNode(tmpseg, cur, prec);
		  seg->Cut(cur->seg->GetXf(), seg->GetXf());
		  NEXT_SEGMENT;
		}
	      else
		{ // nnnnnnn
		  //     cccccc
		  // insert seg left-most part, delete seg right-most part
		  //------------------------------------------------------
		  MESS("case 2");
		  seg->Cut(seg->GetXd(), cur->seg->GetXd());
		  InsertBeforeNode(seg, cur, prec);
		  return;
		}
	    }
	  else
	    { 
	      if (cur->seg->GetXf() <= seg->GetXf())
		{ //     ccccc
		  //  nnnnnnnnnn
		  //current segment must replace by new one
		  //---------------------------------------
		  MESS("case 3");
		  cur->seg->Cut(cur->seg->GetXd(), cur->seg->GetXd());
		  NEXT_SEGMENT;
		}
	      else
		{ //     cccccc
		  // nnnnnnn
		  // insert seg, delete cur's left-most part
		  //----------------------------------------
		  MESS("case 4");
		  cur->seg->Cut(seg->GetXf(), cur->seg->GetXf());
		  InsertBeforeNode(seg, cur, prec);
		  return;
		}
	    }
	}
      else 
	{	  
	  if (cur->seg->Behind(*seg))
	    { 
	      if (seg->GetXf() <= cur->seg->GetXf())
		{ //  cccccccccc
		  //     nnnnn
		  //current must be cut in 3 part, 1 and 3 inserted
		  //-----------------------------------------------
		  MESS("case 5");
		  Segment * tmpseg = segmgm.New(cur->seg);
		  //cout << "y:" << seg->GetY() << " "<< *seg ;
		  //cout << *cur->seg << *tmpseg << kCR;
		  tmpseg->Cut(seg->GetXf(), cur->seg->GetXf());
		  cur->seg->Cut(cur->seg->GetXd(), seg->GetXd());
		  //cout << " ->   "<< *seg << *cur->seg << *tmpseg << kCR;
		  InsertAfterNode(tmpseg, cur);
		  InsertAfterNode(seg, cur);
		  return;
		}
	      else
		{ //  ccccccc
		  //     nnnnnnnn
		  //delete cur right-most part, insert seg after cur
		  //------------------------------------------------
		  MESS("case 6");
		  cur->seg->Cut(cur->seg->GetXd(), seg->GetXd());
		  NEXT_SEGMENT;
		}
	    }
	  else
	    {
	      if (seg->GetXf() <= cur->seg->GetXf())
		{ //    nnnnnn
		  // cccccccccccc
		  //new segment is enterly hidden by the older -> remove it
		  //-------------------------------------------------------
		  MESS("case 7");
		  return;
		}
	      else
		{ //     nnnnnnnn
		  //  ccccccc
		  //delete seg left-most part, insert seg after cur
		  //-----------------------------------------------
		  MESS("case 8");
		  seg->Cut(cur->seg->GetXf(), seg->GetXf());
		  NEXT_SEGMENT;
		}
	    }
	}
      assert(0); // never reach this line
    }
   
  assert(prec != NULL);
  assert(prec->next == NULL);
  prec->next = seglistmgm.New(SegmentList(seg));
}

void SDrawer::InsertBeforeNode(Segment * seg,
			       PSegmentList cur,
			       PSegmentList prec)
{
  #define root segs[seg->GetY()]
  PSegmentList tmp = seglistmgm.New(SegmentList(seg));
  tmp->next = cur;
  if (prec == NULL) root = tmp; else prec->next = tmp;
}

void SDrawer::InsertAfterNode(Segment * seg,
			      PSegmentList cur)
{
  PSegmentList tmp = seglistmgm.New(SegmentList(seg));
  tmp->next = cur->next;
  cur->next = tmp;
}

void SDrawer::NoClipHoriz(int y, byte color,
			  int xd, int xf)
{
  #define flat
  #include "horiz.cpp"
  #undef flat
}

void SDrawer::NoClipShadeHoriz(int y,
			       int xd, int xf,
			       realtype c, realtype ic)
{
  #define gouraud
  #include "horiz.cpp"
  #undef gouraud
}

void SDrawer::NoClipMapHoriz(int y,
			     realtype mxb, realtype myb,
			     realtype mxe, realtype mye,
			     Image& ima,
			     int xd, int xf,
			     realtype mx, realtype imx,
			     realtype my, realtype imy)
{
  #define mapping
  #include "horiz.cpp"
  #undef mapping
}

void SDrawer::NoClipQuadMapHoriz(realtype xb, realtype xe, realtype l,
				 int y, realtype mxb, realtype myb,
				 realtype mxe, realtype mye,
				 Image& ima,
				 int xd, int xf,
				 realtype mx, realtype my,
				 realtype tx, realtype ty,
				 realtype ax, realtype ay)
{
  #define quadmapping
  #include "horiz.cpp"
  #undef quadmapping
}

void SDrawer::NoClipHypMapHoriz(realtype xb, realtype xe,
				realtype zb, realtype ze,
				int y,
				int xd, int xf,
				realtype mxb, realtype myb,
				realtype mxe, realtype mye, Image& ima)
{
  int rxb=round(xb);
  int rxe=round(xe);
  #define hypmapping
  #include "horiz.cpp"
  #undef hypmapping
}

#undef sbuffer
