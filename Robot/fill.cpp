/*****************************************************************************\
 *AUTHOR : Babel yoann
 *TITLE  : image.c, image.h implementation
 *CREATED: 22/10/95
 *DATE   : 18/03/96
 *DEFS   : 'gouraud' for shading
 *         'mapping' for mapping (mpoints and ima must exist)
 *	   'flat' for normal filling (color must exist)
 *         'quadmapping' for quadradtic mapping (cf horiz.cc)
 *         'hypmapping for hypperbolic mapping (cf horiz.cc)
 *BUGS   :
\*****************************************************************************/

/*****************************************************************************\
 *  description of the alogrithm FILL SHAPE WITH N POINTS
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
 *
 *the subroutine behind must be defined
 *  Horiz
 *  ShadeHoriz
 *  MapHoriz
 *  QuadMapHoriz
 *  HypMapHoriz
 *
 *
 *void Drawer::Fill(int npoints, 
 *  Point * points, // the list of points required
 *  MapPoint * mpoints, // the list of points in the image
 *  const Image * ima, // the image itself
 *	color // the color for flat filling
 *	)
 *
\*****************************************************************************/

{
  // becarful the list can be modified (rotated)
  //don't verify that the shape is convex
  assert(npoints>2); //at least 3 points

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
       #ifdef mapping
       MapPoint mtmp = mappoints[0];
       for (int mi=0; mi<npoints-1; mi++)
         mappoints[mi] = mappoints[mi+1];
       mappoints[npoints-1] = mtmp;
       #endif
       #ifdef quadmapping
       MapPoint mtmp = mappoints[0];
       for (int mi=0; mi<npoints-1; mi++)
         mappoints[mi] = mappoints[mi+1];
       mappoints[npoints-1] = mtmp;
       #endif
       #ifdef hypmapping
       MapPoint mtmp = mappoints[0];
       for (int mi=0; mi<npoints-1; mi++)
         mappoints[mi] = mappoints[mi+1];
       mappoints[npoints-1] = mtmp;
       #endif
    }

  // coef's initialization
  //----------------------

  int right=0, left=0;
  int ry=points[0].y;
  int ryf=ry; //y when the next pallier is reached

  realtype xl=conv(points[0].x);
  realtype xr=conv(points[0].x);
  realtype ixl=0, ixr=0;

  realtype zl=fconv(points[0].z);
  realtype zr=fconv(points[0].z);
  realtype izl=0, izr=0;

  #ifdef gouraud
  realtype cl=conv(points[0].color);
  realtype cr=conv(points[0].color);
  realtype icl=0, icr=0;  
  #endif
  #ifdef mapping
  realtype mxl=conv(mappoints[0].x);
  realtype mxr=conv(mappoints[0].x);
  realtype myl=conv(mappoints[0].y);
  realtype myr=conv(mappoints[0].y);
  realtype imxl=0, imxr=0, imyl=0, imyr=0;
  #endif
  #ifdef quadmapping
  realtype mxl=conv(mappoints[0].x);
  realtype mxr=conv(mappoints[0].x);
  realtype myl=conv(mappoints[0].y);
  realtype myr=conv(mappoints[0].y);
  realtype ll=0, lr=0;
  realtype mxlb=mxl, mxle=mxl, mylb=myl, myle=myl;
  realtype mxrb=mxr, mxre=mxr, myrb=myr, myre=myr;
  realtype zlb=zl, zle=zl, zrb=zr, zre=zr;
  int rylb=ry, ryle=ry, ryrb=ry, ryre=ry;
  realtype txl=0, axl=0, tyl=0, ayl=0;
  realtype txr=0, axr=0, tyr=0, ayr=0;
  #endif
  #ifdef hypmapping
  int rmxl=mappoints[0].x;
  int rmxr=mappoints[0].x;
  int rmyl=mappoints[0].y;
  int rmyr=mappoints[0].y;
  int rzl=round(zl);
  int rzr=round(zr);
  int rmxlb=rmxl, rmxle=rmxl; 
  int rmylb=rmyl, rmyle=rmyl;
  int rmxrb=rmxr, rmxre=rmxr; 
  int rmyrb=rmyr, rmyre=rmyr;
  int rylb=ry,    ryle=ry;
  int ryrb=ry,    ryre=ry;
  int rzlb=rzl,   rzle=rzl;
  int rzrb=rzr,   rzre=rzr;
  int umxl=0, umxr=0, umyl=0, umyr=0, uzl=0, uzr=0; //reinit later
  int smxl=0, smxr=0, smyl=0, smyr=0, szl=0, szr=0; //init later
  #endif

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
              #ifdef flat
      	      Horiz(xl, xr, zl, zr, ry,color);
              #endif
              #ifdef gouraud
              ShadeHoriz(xl, xr, zl, zr, ry, cl, cr);
              #endif
              #ifdef mapping
              MapHoriz(xl, xr, zl, zr, ry,
                mxl, myl, mxr, myr, ima);
              #endif
              #ifdef quadmapping
              QuadMapHoriz(xl, xr, zl, zr, ry,
            		mxl, myl, mxr, myr, ima);
              #endif
              #ifdef hypmapping
              HypMapHoriz(xl, xr, conv(rzl), conv(rzr), ry,
                conv(rmxl), conv(rmyl), conv(rmxr), conv(rmyr), ima);
              #endif
      	      ry++;
      	      xl+=ixl; xr+=ixr;
              zl+=izl; zr+=izr;
              #ifdef gouraud
              cl+=icl; cr+=icr;
              #endif
              #ifdef mapping
              mxl+=imxl; myl+=imyl;
              mxr+=imxr; myr+=imyr;
              #endif
              #ifdef quadmapping
      	      mxl += txl;  mxr += txr;
      	      txl += axl;  txr += axr;
      	      myl += tyl;  myr += tyr;
      	      tyl += ayl;  tyr += ayr;
              #endif
              #ifdef hypmapping
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
              #endif
      	    }
      	}

      // calculate the new coef for the next zone
      //-----------------------------------------

      if (points[left].y < points[right].y)
        {
      	  #ifdef quadmapping
      	  zlb = zle;
      	  mxlb=mxle;
      	  mylb=myle;
      	  rylb=ryle;
      	  #endif
      	  #ifdef hypmapping
      	  rzlb =rzle;
      	  rmxlb=rmxle;
      	  rmylb=rmyle;
      	  rylb =ryle;
      	  #endif
      	  left++;
      	  #ifdef quadmapping
      	  zle =fconv(points[left].z);
      	  mxle=conv(mappoints[left].x);
      	  myle=conv(mappoints[left].y);
      	  ryle=points[left].y;
      	  #endif
      	  #ifdef hypmapping
      	  rzle =int(points[left].z);
      	  rmxle=mappoints[left].x;
      	  rmyle=mappoints[left].y;
      	  ryle =points[left].y;
      	  smxl=sign(rmxle-rmxlb);
      	  smyl=sign(rmyle-rmylb);
      	  szl =sign(rzle-rzlb);
      	  umxl=0;
      	  umyl=0;
      	  uzl =0;
      	  #endif
      	  if (points[left].y == ry)
      	    {
      	      xl = conv(points[left].x);
      	      zl = fconv(points[left].z);
              #ifdef gouraud
              cl = conv(points[left].color);
              #endif
              #ifdef mapping
              mxl = conv(mappoints[left].x);
              myl = conv(mappoints[left].y);
              #endif
              #ifdef quadmapping
              mxl = conv(mappoints[left].x);
              myl = conv(mappoints[left].y);
      	      ll = frapport(zle,zlb);
              #endif
              #ifdef hypmapping
              rmxl = mappoints[left].x;
              rmyl = mappoints[left].y;
              #endif
      	    }
      	  else
      	    {
      	      realtype deltax = conv(points[left].x) - xl;
      	      realtype deltay = conv(points[left].y - ry);
      	      realtype deltaz = fconv(points[left].z) - zl;
              #ifdef gouraud
              realtype deltac = conv(points[left].color) - cl;
              #endif
              #ifdef mapping
              realtype deltamx = conv(mappoints[left].x) - mxl;
              realtype deltamy = conv(mappoints[left].y) - myl;
              #endif
              #ifdef quadmapping
              #endif
              #ifdef hypmapping
              #endif
      	      ixl = frapport(deltax, deltay);
      	      izl = frapport(deltaz, deltay);
              #ifdef gouraud
              icl = frapport(deltac, deltay);
              #endif
              #ifdef mapping
              imxl = frapport(deltamx, deltay);
              imyl = frapport(deltamy, deltay);
              #endif
              #ifdef quadmapping
      	      ll = frapport(zle,zlb);
      	      QuadMapCoefsInit(conv(rylb), conv(ryle), ll, conv(rylb),
                mxlb, mxle, mylb, myle,
                mxl, myl, txl, tyl, axl, ayl);
              #endif
              #ifdef hypmapping
              #endif
      	    }
       	}
      else
      	{
          #ifdef quadmapping
      	  zrb =zre;
          mxrb=mxre;
          myrb=myre;
      	  ryrb=ryre;
          #endif
          #ifdef hypmapping
      	  rzrb =rzre;
          rmxrb=rmxre;
          rmyrb=rmyre;
      	  ryrb =ryre;
          #endif
      	  if (right==0) right = npoints-1; else right--;
      	  #ifdef quadmapping
      	  zre =fconv(points[right].z);
      	  mxre=conv(mappoints[right].x);
      	  myre=conv(mappoints[right].y);
      	  ryre=points[right].y;
      	  #endif
      	  #ifdef hypmapping
      	  rzre =int(points[right].z);
      	  rmxre=mappoints[right].x;
      	  rmyre=mappoints[right].y;
      	  ryre =points[right].y;
      	  smxr=sign(rmxre-rmxrb);
      	  smyr=sign(rmyre-rmyrb);
    	    szr =sign(rzre-rzrb);
      	  umxr=0;
      	  umyr=0;
      	  uzr =0;
      	  #endif
      	  if (points[right].y == ry)
      	    {
      	      xr = conv(points[right].x);
      	      zr = fconv(points[right].z);
              #ifdef gouraud
              cr = conv(points[right].color);
              #endif
              #ifdef mapping
              mxr = conv(mappoints[right].x);
              myr = conv(mappoints[right].y);
              #endif
              #ifdef quadmapping
              mxr = conv(mappoints[right].x);
              myr = conv(mappoints[right].y);
      	      lr = frapport(zre,zrb);
              #endif
              #ifdef hypmapping
              rmxr = mappoints[right].x;
              rmyr = mappoints[right].y;
              #endif
      	    }
      	  else
      	    {
      	      realtype deltax = conv(points[right].x) - xr;
      	      realtype deltay = conv(points[right].y - ry);
      	      realtype deltaz = fconv(points[right].z) -zr;
              #ifdef gouraud
              realtype deltac = conv(points[right].color) - cr;
              #endif
              #ifdef mapping
              realtype deltamx = conv(mappoints[right].x) - mxr;
              realtype deltamy = conv(mappoints[right].y) - myr;
              #endif
              #ifdef quadmapping
              #endif
              #ifdef hypmapping
              #endif
      	      ixr = frapport(deltax, deltay);
      	      izr = frapport(deltaz, deltay);
              #ifdef gouraud
              icr = frapport(deltac, deltay);
              #endif
              #ifdef mapping
              imxr = frapport(deltamx, deltay);
              imyr = frapport(deltamy, deltay);
              #endif
              #ifdef quadmapping
      	      lr = frapport(zre,zrb);
      	      QuadMapCoefsInit(conv(ryrb), conv(ryre), lr, conv(ryrb),
                mxrb, mxre, myrb, myre,
                mxr, myr, txr, tyr, axr, ayr);
              #endif
              #ifdef hypmapping
              #endif
      	    }
      	}
    }
  while (right >= left
         && !(right==0 && left == npoints-1)
         && !(right==0 && left==0)
        );
}
